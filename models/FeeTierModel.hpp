#pragma once
#include "models/IFeesModel.hpp"

/// Fee model based on maker/taker tiers.
class FeeTierModel : public IFeesModel
{
public:
    FeeTierModel(double makerFee = 0.0002, double takerFee = 0.0005);
    double compute(const OrderBookSnapshot &snapshot,
                   double orderSizeUsd) override;

private:
    double makerFee_;
    double takerFee_;
};