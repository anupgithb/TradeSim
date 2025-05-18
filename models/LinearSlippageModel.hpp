#pragma once
#include "models/ISlippageModel.hpp"

/// Simple linear model: slippage = k * orderSizeUsd / topOfBookLiquidity
class LinearSlippageModel : public ISlippageModel
{
public:
    explicit LinearSlippageModel(double k = 0.001);
    double compute(const OrderBookSnapshot &snapshot,
                   double orderSizeUsd) override;

private:
    double k_; ///< linear coefficient
};