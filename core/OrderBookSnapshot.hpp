#pragma once

#include "core/OrderBook.hpp"
#include <chrono>

/// A lightweight snapshot of the order book and derived metrics
struct OrderBookSnapshot
{
    Level topBid;                                    ///< best bid level
    Level topAsk;                                    ///< best ask level
    double estimatedDailyVolume;                     ///< e.g., from historical data or config
    double makerTakerRatio;                          ///< from recent trades (0.0–1.0)
    std::chrono::steady_clock::time_point timestamp; ///< when snapshot taken

    /// Create a snapshot directly from an OrderBook
    static OrderBookSnapshot fromBook(const OrderBook &book,
                                      double dailyVolume,
                                      double makerRatio)
    {
        auto [bid, ask] = book.topOfBook();
        return OrderBookSnapshot{
            bid, ask,
            dailyVolume,
            makerRatio,
            std::chrono::steady_clock::now()};
    }
};