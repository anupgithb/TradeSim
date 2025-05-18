#include "models/AlmgrenChrissModel.hpp"

AlmgrenChrissModel::AlmgrenChrissModel(double eta, double gamma)
    : eta_(eta), gamma_(gamma) {}

double AlmgrenChrissModel::compute(const OrderBookSnapshot &snapshot,
                                   double orderSizeUsd)
{
    // Simplified formula: impact = eta * (orderSize / dailyVolume)^gamma
    double dailyVolume = snapshot.estimatedDailyVolume;
    if (dailyVolume <= 0)
        return 0.0;
    double ratio = orderSizeUsd / dailyVolume;
    return eta_ * std::pow(ratio, gamma_) * orderSizeUsd;
}