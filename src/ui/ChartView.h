#pragma once

#include <QtCharts/QChartView>

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
public:
	ChartView(QWidget *parent = 0);

protected:
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event) override;
};
