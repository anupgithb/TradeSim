// #include <memory>
// #include "core/AppController.hpp"
// #include "network/OKXWebSocketConnector.hpp"
// #include "core/OrderBookSnapshot.hpp"
// #include "models/AlmgrenChrissModel.hpp"
// #include "models/LinearSlippageModel.hpp"
// #include "models/FeeTierModel.hpp"
// #include "utils/Logger.hpp"

// AppController::AppController()
// {
//     // Initialize connector with rawQueue
//     connector_ = std::make_unique<OKXWebSocketConnector>(rawQueue_);

//     // Initialize models (could be parameterized)
//     slippageModel_ = std::make_unique<LinearSlippageModel>();
//     impactModel_ = std::make_unique<AlmgrenChrissModel>();
//     feesModel_ = std::make_unique<FeeTierModel>();
// }

// AppController::~AppController()
// {
//     stop();
// }

// void AppController::run()
// {
//     running_ = true;

//     wsThread_ = std::thread(&AppController::wsLoop, this);
//     bookThread_ = std::thread(&AppController::bookLoop, this);
//     modelThread_ = std::thread(&AppController::modelLoop, this);

//     wsThread_.detach();
//     bookThread_.detach();
//     modelThread_.detach();
// }

// void AppController::stop()
// {
//     if (!running_)
//         return;
//     running_ = false;
//     connector_->stop();
// }

// void AppController::wsLoop()
// {
//     connector_->start([&](const std::string &msg)
//                       {
//                           // callback unused because rawQueue_ is pushed directly in connector
//                       });
//     // Block until stop() is called
//     while (running_)
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
// }

// void AppController::bookLoop()
// {
//     while (running_)
//     {
//         std::string raw;
//         if (rawQueue_.pop(raw))
//         {
//             // Parse the incoming raw JSON into a new OrderBook
//             std::unique_ptr<OrderBook> orderBook = OrderBook::fromJson(raw);

//             // Publish the event with ownership transferred
//             orderBookUpdated_.publish(OrderBookUpdatedEvent{std::move(orderBook)});

//             // Update the main order book (assuming orderBook_ is a member)
//             // orderBook_.updateBids(orderBook->bids);
//             // orderBook_.updateAsks(orderBook->asks);

//             // Generate and publish a snapshot
//             auto snapshot = OrderBookSnapshot::fromBook(orderBook_, dailyVolume_, makerRatio_);
//             bookUpdated_.publish(snapshot);
//         }
//     }
// }

// void AppController::modelLoop()
// {
//     bookUpdated_.subscribe([&](const OrderBookSnapshot &snapshot)
//                            {
//         Metrics m;
//         m.slippage = slippageModel_->compute(snapshot);
//         m.impact   = impactModel_->compute(snapshot);
//         m.fees     = feesModel_->compute(snapshot);
//         m.netCost  = m.slippage + m.impact + m.fees;
//         metricsReady_.publish(m); });

//     while (running_)
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
// }

// #include "core/AppController.hpp"
// #include "events/OrderBookUpdatedEvent.hpp"
// #include "core/Metrics.hpp"
// #include "core/OrderBookSnapshot.hpp"
// #include "models/AlmgrenChrissModel.hpp"
// #include "models/LinearSlippageModel.hpp"
// #include "models/FeeTierModel.hpp"
// #include "utils/Logger.hpp"
// #include "network/OKXWebSocketConnector.hpp"

// AppController::AppController()
// {
//     connector_ = std::make_unique<OKXWebSocketConnector>(rawQueue_);
//     slippageModel_ = std::make_unique<LinearSlippageModel>();
//     impactModel_ = std::make_unique<AlmgrenChrissModel>();
//     feesModel_ = std::make_unique<FeeTierModel>();
// }

// AppController::~AppController()
// {
//     stop();
// }

// void AppController::run()
// {
//     running_ = true;
//     uiServer_ = std::make_unique<UIBroadcastServer>(ioc_for_ui, 9000);
//     uiServer_->run();
//     wsThread_ = std::thread(&AppController::wsLoop, this);
//     bookThread_ = std::thread(&AppController::bookLoop, this);
//     modelThread_ = std::thread(&AppController::modelLoop, this);
// }

// void AppController::stop()
// {
//     if (!running_)
//         return;
//     running_ = false;
//     connector_->stop();
// }

// void AppController::wsLoop()
// {
//     connector_->start(nullptr);
//     while (running_)
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
// }

// void AppController::bookLoop()
// {
//     while (running_)
//     {
//         std::string raw;
//         if (rawQueue_.pop(raw))
//         {
//             // Parse and publish full OrderBook
//             spdlog::info("[BookLoop] Got raw tick ({} bytes)", raw.size());
//             auto obPtr = OrderBook::fromJson(raw);
//             orderBookUpdated_.publish({std::move(obPtr)});

//             // Build and publish snapshot
//             auto snapshot = OrderBookSnapshot::fromBook(
//                 orderBook_, dailyVolume_, makerRatio_);
//             bookUpdated_.publish(snapshot);
//         }
//     }
// }

// void AppController::modelLoop()
// {
//     bookUpdated_.subscribe([&](const OrderBookSnapshot &snapshot)
//                            {
//         Metrics m;
//         m.slippage = slippageModel_->compute(snapshot, orderSizeUsd_);
//         m.impact   = impactModel_->compute(snapshot,   orderSizeUsd_);
//         m.fees     = feesModel_->compute(snapshot,     orderSizeUsd_);
//         m.netCost  = m.slippage + m.impact + m.fees;
//         spdlog::info("[ModelLoop] Slippage={:.2f}  Impact={:.2f}  Fees={:.2f}  Net={:.2f}",
//                      m.slippage, m.impact, m.fees, m.netCost);
//         metricsReady_.publish(m); });

//     while (running_)
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
// }

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

    // Start the UI broadcast server on port 9000
    uiServer_ = std::make_unique<UIBroadcastServer>(ioc_for_ui_, 9000);
    uiServer_->run();
    // Run the UI io_context in its own thread
    uiThread_ = std::thread([this]()
                            { ioc_for_ui_.run(); });

    // Start core processing threads
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
            spdlog::info("[BookLoop] Got raw tick ({} bytes)", raw.size());

            auto obPtr = OrderBook::fromJson(raw);
            orderBookUpdated_.publish({std::move(obPtr)});

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
        // Metrics m;
        // m.slippage = slippageModel_->compute(snapshot, orderSizeUsd_);
        // m.impact   = impactModel_->compute(snapshot,   orderSizeUsd_);
        // m.fees     = feesModel_->compute(snapshot,     orderSizeUsd_);
        // m.netCost  = m.slippage + m.impact + m.fees;
        // spdlog::info("[ModelLoop] Slippage={:.2f}  Impact={:.2f}  Fees={:.2f}  Net={:.2f}",
        //              m.slippage, m.impact, m.fees, m.netCost);
        // Measure start time
    auto t0 = std::chrono::high_resolution_clock::now();

        Metrics m;
        m.slippage          = slippageModel_->compute(snapshot, orderSizeUsd_);
        m.impact            = impactModel_->compute(snapshot,   orderSizeUsd_);
        m.fees              = feesModel_->compute(snapshot,     orderSizeUsd_);
        m.netCost           = m.slippage + m.impact + m.fees;

        // Compute maker/taker proportion via your logistic‐regression model
        // (assuming you have an IOrderTypeModel or similar)
        // m.makerTakerRatio   = makerTakerModel_->compute(snapshot, orderSizeUsd_);
        m.makerTakerRatio =0;

      // Measure end time & compute internal latency
        auto t1 = std::chrono::high_resolution_clock::now();
        m.internalLatency   = std::chrono::duration<double, std::milli>(t1 - t0).count();
+
+        spdlog::info(
+          "[ModelLoop] Slippage={:.4f}  Impact={:.4f}  Fees={:.4f}  Net={:.4f}  M/T={:.4f}  Latency={:.2f}ms",
+          m.slippage, m.impact, m.fees, m.netCost, m.makerTakerRatio, m.internalLatency
+        );

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
