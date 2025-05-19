// models/AlmgrenChrissModel.cpp
#include "models/AlmgrenChrissModel.hpp"
#include <numeric> // for std::accumulate

AlmgrenChrissModel::AlmgrenChrissModel(double eta, double gamma)
    : eta_(eta), gamma_(gamma) {}

double AlmgrenChrissModel::compute(const OrderBookSnapshot &snapshot,
                                   double orderSizeUsd)
{
    // 1) Estimate current available dailyVolume from top levels:
    //    sum over (price * size) for bids + asks.
    double availLiquidity = 0.0;
    for (auto &lvl : snapshot.bids)
        availLiquidity += lvl.price * lvl.size;
    for (auto &lvl : snapshot.asks)
        availLiquidity += lvl.price * lvl.size;

    // 2) Fallback to a floor if liquidity is too low
    if (availLiquidity < 1e3)
        availLiquidity = 1e3; // e.g. $1k min

    // 3) Almgren–Chriss: impact ∝ (orderSize / liquidity)^γ * orderSize
    double ratio = orderSizeUsd / availLiquidity;
    return eta_ * std::pow(ratio, gamma_) * orderSizeUsd;
}
