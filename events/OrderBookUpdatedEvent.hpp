#pragma once
#include <memory>
#include "core/OrderBook.hpp"

struct OrderBookUpdatedEvent
{
    std::unique_ptr<OrderBook> orderBook;
};
