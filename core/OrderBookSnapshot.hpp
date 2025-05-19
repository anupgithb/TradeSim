#pragma once

#include "core/OrderBook.hpp"
#include <chrono>
#include <vector>

/// A snapshot of the top N levels of the L2 order book plus meta‑data.
struct OrderBookSnapshot
{
    std::vector<Level> bids; ///< Top N bids (best‑first)
    std::vector<Level> asks; ///< Top N asks (best‑first)

    double estimatedDailyVolume; ///< Historical/configured daily volume (USD)
    double makerTakerRatio;      ///< Recent maker vs taker fraction (0.0–1.0)

    std::chrono::steady_clock::time_point timestamp; ///< When snapshot was taken

    /// Build a snapshot from the live OrderBook.
    /// - `depth`   : how many levels on each side to capture
    /// - `dailyVol`: historical or config daily volume (USD)
    /// - `makerR`  : recent maker/taker ratio
    static OrderBookSnapshot fromBook(
        const OrderBook &book,
        size_t depth = 10,
        double dailyVol = 1e6,
        double makerR = 0.3)
    {
        OrderBookSnapshot s;
        s.bids = book.topLevels(depth, /*isBid=*/true);
        s.asks = book.topLevels(depth, /*isBid=*/false);
        s.estimatedDailyVolume = dailyVol;
        s.makerTakerRatio = makerR;
        s.timestamp = std::chrono::steady_clock::now();
        return s;
    }
};
