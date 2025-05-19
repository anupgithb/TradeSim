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
        for (const auto &lvl : bids)
            bids_[lvl.price] = lvl.size;
    }

    /// Update the ask side with a new snapshot of levels.
    void updateAsks(const std::vector<Level> &asks)
    {
        std::unique_lock lock(mutex_);
        asks_.clear();
        for (const auto &lvl : asks)
            asks_[lvl.price] = lvl.size;
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

    /// Extract all bid levels as a vector sorted best-first.
    std::vector<Level> getBids() const
    {
        std::shared_lock lock(mutex_);
        std::vector<Level> levels;
        levels.reserve(bids_.size());
        for (auto &[price, size] : bids_)
            levels.push_back({price, size});
        return levels;
    }

    /// Extract all ask levels as a vector sorted best-first.
    std::vector<Level> getAsks() const
    {
        std::shared_lock lock(mutex_);
        std::vector<Level> levels;
        levels.reserve(asks_.size());
        for (auto &[price, size] : asks_)
            levels.push_back({price, size});
        return levels;
    }

    /// Alias for getBids()
    std::vector<Level> extractBids() const { return getBids(); }

    /// Alias for getAsks()
    std::vector<Level> extractAsks() const { return getAsks(); }

    /// Return up to N levels for bids or asks.
    std::vector<Level> topLevels(size_t n, bool isBid) const
    {
        auto all = isBid ? getBids() : getAsks();
        if (all.size() > n)
            all.resize(n);
        return all;
    }

    /// Parse raw JSON message into levels.
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

    /// Construct a new OrderBook from raw JSON.
    static std::unique_ptr<OrderBook> fromJson(const std::string &raw)
    {
        auto ob = std::make_unique<OrderBook>();
        auto j = nlohmann::json::parse(raw);

        // Support nested data structures, e.g., some feeds wrap bids/asks in a "data" array
        const nlohmann::json *bidsArr = nullptr;
        const nlohmann::json *asksArr = nullptr;
        if (j.contains("bids") && j.contains("asks"))
        {
            bidsArr = &j["bids"];
            asksArr = &j["asks"];
        }
        else if (j.contains("data") && j["data"].is_array() && !j["data"].empty())
        {
            auto &item = j["data"][0];
            bidsArr = &item["bids"];
            asksArr = &item["asks"];
        }
        else
        {
            throw std::runtime_error("OrderBook::fromJson: unexpected JSON format");
        }

        ob->updateBids(parseLevels(*bidsArr));
        ob->updateAsks(parseLevels(*asksArr));
        return ob;
    }

private:
    mutable std::shared_mutex mutex_;
    std::map<double, double, std::greater<>> bids_;
    std::map<double, double, std::less<>> asks_;
};
