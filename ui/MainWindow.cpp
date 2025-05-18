#include "ui/MainWindow.hpp"

MainWindow::MainWindow(UIUpdater *updater, QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    // Connect UIUpdater signal to our slot
    connect(updater, &UIUpdater::metricsUpdated,
            this, &MainWindow::handleMetrics);
}

void MainWindow::handleMetrics(double slippage,
                               double fees,
                               double impact,
                               double netCost)
{
    // Update UI widgets
    slippageValueLabel->setText(QString::number(slippage, 'f', 2));
    feesValueLabel->setText(QString::number(fees, 'f', 2));
    impactValueLabel->setText(QString::number(impact, 'f', 2));
    netCostValueLabel->setText(QString::number(netCost, 'f', 2));
}