// models/FeeTierModel.hpp
#pragma once

#include "core/OrderBookSnapshot.hpp"
#include "models/IFeesModel.hpp"

/// Blends maker and taker fees according to instantaneous liquidity split.
class FeeTierModel : public IFeesModel
{
public:
    /// @param makerFee  fee rate charged when adding liquidity (e.g. 0.001 = 0.1%)
    /// @param takerFee  fee rate charged when removing liquidity (e.g. 0.002 = 0.2%)
    explicit FeeTierModel(double makerFee = 0.001, double takerFee = 0.002)
        : makerFee_(makerFee), takerFee_(takerFee) {}

    double compute(const OrderBookSnapshot &snapshot,
                   double orderSizeUsd) override;

private:
    double makerFee_;
    double takerFee_;
};
