#pragma once
#include "network/IMarketDataConnector.hpp"
#include "network/UIBroadcastServer.hpp"
#include "utils/LockFreeQueue.hpp"
#include "core/OrderBook.hpp"
#include "core/EventDispatcher.hpp"
#include "models/ISlippageModel.hpp"
#include "models/IMarketImpactModel.hpp"
#include "models/IFeesModel.hpp"
#include "events/OrderBookUpdatedEvent.hpp"
#include <memory>
#include <thread>
#include <atomic>

struct OrderBookSnapshot;
struct Metrics;

class AppController
{
public:
    AppController();
    ~AppController();

    void run();
    void stop();

private:
    void wsLoop();
    void bookLoop();
    void modelLoop();

    LockFreeQueue<std::string> rawQueue_;
    OrderBook orderBook_;
    EventDispatcher<OrderBookSnapshot> bookUpdated_;
    EventDispatcher<Metrics> metricsReady_;
    EventDispatcher<OrderBookUpdatedEvent> orderBookUpdated_;

    std::unique_ptr<IMarketDataConnector> connector_;
    std::unique_ptr<ISlippageModel> slippageModel_;
    std::unique_ptr<IMarketImpactModel> impactModel_;
    std::unique_ptr<IFeesModel> feesModel_;

    // Parameters for snapshot
    double dailyVolume_ = 1000000.0; ///< default daily volume USD
    double makerRatio_ = 0.3;        ///< default maker ratio
    double orderSizeUsd_ = 100.0;

    // UI broadcast server context and thread
    net::io_context ioc_for_ui_;
    std::unique_ptr<UIBroadcastServer> uiServer_;
    std::thread uiThread_;

    std::thread wsThread_, bookThread_, modelThread_;
    std::atomic<bool> running_{false};
};
