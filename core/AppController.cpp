#include "core/AppController.hpp"
#include "events/OrderBookUpdatedEvent.hpp"
#include "core/Metrics.hpp"
#include "core/OrderBookSnapshot.hpp"
#include "models/AlmgrenChrissModel.hpp"
#include "models/LinearSlippageModel.hpp"
#include "models/FeeTierModel.hpp"
#include "utils/Logger.hpp"
#include "network/OKXWebSocketConnector.hpp"
#include "network/UIBroadcastServer.hpp"
#include <nlohmann/json.hpp>
#include <thread>

AppController::AppController()
{
    connector_ = std::make_unique<OKXWebSocketConnector>(rawQueue_);
    slippageModel_ = std::make_unique<LinearSlippageModel>();
    impactModel_ = std::make_unique<AlmgrenChrissModel>();
    feesModel_ = std::make_unique<FeeTierModel>();
}

AppController::~AppController()
{
    stop();
}

void AppController::run()
{
    running_ = true;

    // 1) Launch the UI WebSocket server
    uiServer_ = std::make_unique<UIBroadcastServer>(ioc_for_ui_, 9000);

    // 2) Register callback to handle { type:"settings", params: {...} } messages
    uiServer_->setOnControl([this](const nlohmann::json &params)
                            {
                                // Apply new parameters from React form
                                if (params.contains("quantity"))
                                {
                                    orderSizeUsd_ = params["quantity"].get<double>();
                                    spdlog::info("[Control] orderSizeUsd_ set to {}", orderSizeUsd_);
                                }
                                if (params.contains("volatility"))
                                {
                                    volatility_ = params["volatility"].get<double>();
                                    spdlog::info("[Control] volatility_ set to {}", volatility_);
                                }
                                if (params.contains("feeTier"))
                                {
                                    auto tier = params["feeTier"].get<std::string>();
                                    // Map a string tier to makerRatio_, for example:
                                    if (tier == "Tier 1")
                                        makerRatio_ = 0.3;
                                    else if (tier == "Tier 2")
                                        makerRatio_ = 0.2;
                                    else if (tier == "Tier 3")
                                        makerRatio_ = 0.1;
                                    spdlog::info("[Control] makerRatio_ set to {} for {}", makerRatio_, tier);
                                }
                                // You can also handle exchange, spotAsset, orderType if needed...
                            });

    // Start accepting UI connections
    uiServer_->run();

    // Run the UI io_context in its own thread
    uiThread_ = std::thread([this]()
                            { ioc_for_ui_.run(); });

    // 3) Start core processing threads
    wsThread_ = std::thread(&AppController::wsLoop, this);
    bookThread_ = std::thread(&AppController::bookLoop, this);
    modelThread_ = std::thread(&AppController::modelLoop, this);
}

void AppController::stop()
{
    if (!running_)
        return;
    running_ = false;
    connector_->stop();
    ioc_for_ui_.stop();
    if (uiThread_.joinable())
        uiThread_.join();
}

void AppController::wsLoop()
{
    connector_->start(nullptr);
    while (running_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void AppController::bookLoop()
{
    while (running_)
    {
        std::string raw;
        if (rawQueue_.pop(raw))
        {
            // spdlog::info("[BookLoop] Got raw tick ({} bytes)", raw.size());
            // std::cout << "[DEBUG] Raw JSON: " << raw << std::endl;

            auto obPtr = OrderBook::fromJson(raw);
            orderBookUpdated_.publish({std::move(obPtr)});

            // ACTUALLY update the shared orderBook_
            auto tmp = OrderBook::fromJson(raw);
            orderBook_.updateBids(tmp->extractBids());
            orderBook_.updateAsks(tmp->extractAsks());

            // Now snapshot has fresh data
            auto snapshot = OrderBookSnapshot::fromBook(
                orderBook_, dailyVolume_, makerRatio_);
            bookUpdated_.publish(snapshot);
        }
    }
}

void AppController::modelLoop()
{
    bookUpdated_.subscribe([&](const OrderBookSnapshot &snapshot)
                           {
        auto t0 = std::chrono::high_resolution_clock::now();

        Metrics m;
        m.slippage          = slippageModel_->compute(snapshot, orderSizeUsd_);
        m.impact            = impactModel_->compute(snapshot,   orderSizeUsd_);
        m.fees              = feesModel_->compute(snapshot,     orderSizeUsd_);
        m.netCost           = m.slippage + m.impact + m.fees;
        // Compute maker/taker proportion via your logistic‐regression model
        // m.makerTakerRatio   = makerTakerModel_->compute(snapshot, orderSizeUsd_);
        m.makerTakerRatio =0;

        auto t1 = std::chrono::high_resolution_clock::now();
        m.internalLatency   = std::chrono::duration<double, std::milli>(t1 - t0).count();

        spdlog::info(
          "[ModelLoop] Slippage={:.8f}  Impact={:.6f}  Fees={:.6f}  Net={:.6f}  M/T={:.6f}  Latency={:.6f}ms",
          m.slippage, m.impact, m.fees, m.netCost, m.makerTakerRatio, m.internalLatency
        );

        // Publish internally
        metricsReady_.publish(m);

        // Broadcast to UI clients
        try {
            nlohmann::json j = {
                { "slippage", m.slippage },
                { "fees",      m.fees },
                { "impact",    m.impact },
                { "netCost",   m.netCost },
                { "makerTakerRatio", m.makerTakerRatio },
                { "internalLatency",   m.internalLatency }
            };
            uiServer_->broadcast(j.dump());
        } catch (const std::exception &ex) {
            spdlog::error("[UI] Broadcast failed: {}", ex.what());
        } });

    while (running_)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
