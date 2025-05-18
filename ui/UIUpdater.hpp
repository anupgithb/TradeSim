#pragma once
#include <QObject>
#include "core/EventDispatcher.hpp"
#include "core/OrderBookSnapshot.hpp"
#include "core/Metrics.hpp"

/// QObject wrapper to forward Metrics events into Qt signals.
class UIUpdater : public QObject
{
    Q_OBJECT
public:
    explicit UIUpdater(EventDispatcher<Metrics> &dispatcher, QObject *parent = nullptr);

signals:
    void metricsUpdated(double slippage,
                        double fees,
                        double impact,
                        double netCost);

private:
    void onMetrics(const Metrics &m);
};