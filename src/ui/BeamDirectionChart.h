#pragma once

#include <QObject>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>

QT_CHARTS_USE_NAMESPACE

class BeamDirectionChart : public QPolarChart
{
	Q_OBJECT
public:
	explicit BeamDirectionChart(QGraphicsItem *parent = nullptr);

	void Draw(const QMap<int, QPointF> &directions);

private:
	QVector<QColor> colors;

	void SetAxes();
	void FindMinMax(const QMap<int, QPointF> directions,
					int &min, int &max);
	void GetRanges(int min, int max, QVector<QPair<int, int> > &ranges);
	bool FillSeries(const QMap<int, QPointF> &directions, const QPair<int, int> &range,
					QScatterSeries *series);
	void HideSeries();
	void ShowSeries(QScatterSeries *s, const QPair<int, int> &r);
};
