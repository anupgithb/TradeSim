#include "models/LinearSlippageModel.hpp"

LinearSlippageModel::LinearSlippageModel(double k)
    : k_(k) {}

double LinearSlippageModel::compute(const OrderBookSnapshot &snapshot,
                                    double orderSizeUsd)
{
    // Use top-level liquidity as approximation
    double liquidity = snapshot.topBid.size + snapshot.topAsk.size;
    if (liquidity <= 0)
        return 0.0;
    return k_ * orderSizeUsd / liquidity;
}