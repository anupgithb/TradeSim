// core/Metrics.hpp
#pragma once

/// Holds the calculated metrics for a given order execution snapshot
struct Metrics
{
    double slippage = 0.0;        ///< Expected slippage in USD
    double fees = 0.0;            ///< Expected fees in USD
    double impact = 0.0;          ///< Expected market impact in USD
    double netCost = 0.0;         ///< Total cost (slippage + fees + impact)
    double makerTakerRatio = 0.0; ///< Proportion of maker vs taker orders
    double internalLatency = 0.0; ///< Processing time per tick in milliseconds
};
