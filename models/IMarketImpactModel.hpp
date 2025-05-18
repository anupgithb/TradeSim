#pragma once
#include "core/OrderBookSnapshot.hpp"

/// Abstract interface for computing market impact.
class IMarketImpactModel
{
public:
    virtual ~IMarketImpactModel() = default;
    virtual double compute(const OrderBookSnapshot &snapshot,
                           double orderSizeUsd) = 0;
};