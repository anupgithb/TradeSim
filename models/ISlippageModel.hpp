#pragma once

#include "core/OrderBookSnapshot.hpp"

/// Abstract interface for computing slippage from an order book snapshot.
class ISlippageModel
{
public:
    virtual ~ISlippageModel() = default;
    /// Compute expected slippage (in USD) for a given order size.
    virtual double compute(const OrderBookSnapshot &snapshot,
                           double orderSizeUsd) = 0;
};