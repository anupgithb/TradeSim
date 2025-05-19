#include "models/FeeTierModel.hpp"

double FeeTierModel::compute(const OrderBookSnapshot &snapshot,
                             double orderSizeUsd)
{
    // 1) Sum up bid‐side vs ask‐side dollar liquidity
    double bidLiquidity = 0.0;
    for (auto &lvl : snapshot.bids)
        bidLiquidity += lvl.price * lvl.size;

    double askLiquidity = 0.0;
    for (auto &lvl : snapshot.asks)
        askLiquidity += lvl.price * lvl.size;

    double totalLiquidity = bidLiquidity + askLiquidity;

    // 2) Dynamic maker ratio: proportion of bid‐side depth
    double makerRatio = (totalLiquidity > 0.0)
                            ? (bidLiquidity / totalLiquidity)
                            : 0.5; // fallback 50/50 if no depth

    // 3) Blend maker/taker fees
    double blendedFeeRate = makerRatio * makerFee_ + (1.0 - makerRatio) * takerFee_;

    // 4) Return USD fees
    return blendedFeeRate * orderSizeUsd;
}
