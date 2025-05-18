#include "models/FeeTierModel.hpp"

FeeTierModel::FeeTierModel(double makerFee, double takerFee)
    : makerFee_(makerFee), takerFee_(takerFee) {}

double FeeTierModel::compute(const OrderBookSnapshot &snapshot,
                             double orderSizeUsd)
{
    double makerRatio = snapshot.makerTakerRatio; // e.g., 0.3 => 30% maker
    double fee = makerRatio * makerFee_ + (1 - makerRatio) * takerFee_;
    return fee * orderSizeUsd;
}