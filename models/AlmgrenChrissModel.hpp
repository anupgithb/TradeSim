#pragma once
#include "models/IMarketImpactModel.hpp"

/// Implements basic Almgren–Chriss impact model.
class AlmgrenChrissModel : public IMarketImpactModel
{
public:
    AlmgrenChrissModel(double eta = 0.1, double gamma = 0.6);
    double compute(const OrderBookSnapshot &snapshot,
                   double orderSizeUsd) override;

private:
    double eta_;   ///< temporary impact coefficient
    double gamma_; ///< permanent impact coefficient
};