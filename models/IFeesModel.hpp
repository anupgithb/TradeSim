#pragma once
#include "core/OrderBookSnapshot.hpp"

/// Abstract interface for computing fees.
class IFeesModel
{
public:
    virtual ~IFeesModel() = default;
    virtual double compute(const OrderBookSnapshot &snapshot,
                           double orderSizeUsd) = 0;
};