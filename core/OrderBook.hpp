#pragma once

#include <map>
#include <shared_mutex>
#include <mutex>
#include <vector>
#include <nlohmann/json.hpp>

/// Represents a price level in the order book.
struct Level
{
    double price;
    double size;
};

/// Thread-safe L2 order book storing bids and asks.
class OrderBook
{
public:
    OrderBook() = default;

    /// Update the bid side with a new snapshot of levels.
    void updateBids(const std::vector<Level> &bids)
    {
        std::unique_lock lock(mutex_);
        bids_.clear();
        for (auto &lvl : bids)
        {
            bids_[lvl.price] = lvl.size;
        }
    }

    /// Update the ask side with a new snapshot of levels.
    void updateAsks(const std::vector<Level> &asks)
    {
        std::unique_lock lock(mutex_);
        asks_.clear();
        for (auto &lvl : asks)
        {
            asks_[lvl.price] = lvl.size;
        }
    }

    /// Return the best bid and ask as a pair.
    std::pair<Level, Level> topOfBook() const
    {
        std::shared_lock lock(mutex_);
        Level bestBid{0, 0}, bestAsk{0, 0};
        if (!bids_.empty())
        {
            auto it = bids_.begin();
            bestBid = {it->first, it->second};
        }
        if (!asks_.empty())
        {
            auto it = asks_.begin();
            bestAsk = {it->first, it->second};
        }
        return {bestBid, bestAsk};
    }

    /// Parse raw JSON message into bids & asks and update book.
    static std::vector<Level> parseLevels(const nlohmann::json &arr)
    {
        std::vector<Level> levels;
        for (auto &entry : arr)
        {
            double price = std::stod(entry[0].get<std::string>());
            double size = std::stod(entry[1].get<std::string>());
            levels.push_back({price, size});
        }
        return levels;
    }

    static std::unique_ptr<OrderBook> fromJson(const std::string &raw)
    {
        auto ob = std::make_unique<OrderBook>();
        auto j = nlohmann::json::parse(raw);

        auto bids = parseLevels(j["bids"]);
        auto asks = parseLevels(j["asks"]);

        ob->updateBids(bids);
        ob->updateAsks(asks);

        return ob;
    }

private:
    mutable std::shared_mutex mutex_;
    std::map<double, double, std::greater<>> bids_;
    std::map<double, double, std::less<>> asks_;
};