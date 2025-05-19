#include "models/LinearSlippageModel.hpp"
#include <algorithm> // for std::max

LinearSlippageModel::LinearSlippageModel(double k)
    : k_(k)
{
}

double LinearSlippageModel::compute(
    const OrderBookSnapshot &snapshot,
    double orderSizeUsd)
{
    // 1) Need at least one level on each side
    if (snapshot.bids.empty() || snapshot.asks.empty())
        return 0.0;

    // 2) Estimate instantaneous liquidity by best bid + best ask size (in USD)
    double bidLiquidity = snapshot.bids.front().price * snapshot.bids.front().size;
    double askLiquidity = snapshot.asks.front().price * snapshot.asks.front().size;
    double liquidity = bidLiquidity + askLiquidity;

    // 3) Safeguard
    liquidity = std::max(liquidity, 1e-6);

    // 4) Linear slippage: k * (orderSize / liquidity) * orderSize
    return k_ * (orderSizeUsd / liquidity) * orderSizeUsd;
}
