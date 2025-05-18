#include "ui/UIUpdater.hpp"
#include <QMetaObject>

UIUpdater::UIUpdater(EventDispatcher<Metrics> &dispatcher, QObject *parent)
    : QObject(parent)
{
    // Subscribe to metrics events
    dispatcher.subscribe([this](const Metrics &m)
                         {
        // Ensure slot is invoked in Qt GUI thread
        QMetaObject::invokeMethod(this, "onMetrics", Qt::QueuedConnection,
            Q_ARG(Metrics, m)); });
}

void UIUpdater::onMetrics(const Metrics &m)
{
    emit metricsUpdated(m.slippage, m.fees, m.impact, m.netCost);
}