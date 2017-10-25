#include "BeamDirectionChart.h"

BeamDirectionChart::BeamDirectionChart(QGraphicsItem *parent)
	: QPolarChart(parent)
{
	setTitle("Beam direction distribution dependence on scattering direction");

	colors.append(QColor(0, 0, 255));
	colors.append(QColor(109, 0, 255));
	colors.append(QColor(164, 0, 255));
	colors.append(QColor(255, 0, 255));
	colors.append(QColor(255, 0, 144));
	colors.append(QColor(255, 0, 0));

	SetAxes();
}

void BeamDirectionChart::FindMinMax(const QMap<int, QPointF> directions,
									int &min, int &max)
{
	QList<int> counts = directions.keys();
	min = INT_MAX;
	max = 0;

	foreach (int count, counts)
	{
		if (count < min)
		{
			min = count;
		}

		if (count > max)
		{
			max = count;
		}
	}
}

void BeamDirectionChart::GetRanges(int min, int max, QVector<QPair<int, int>> &ranges)
{
	float coef = (float)(max - min)/colors.size();

	QPair<int, int> range;
	range.first = min;
	range.second = min + lround(coef);
	ranges.append(range);

	for (int i = 1; i < colors.size(); ++i)
	{
		range.first = ranges.last().second + 1;
		range.second = min + lround(coef*(i+1));

		if (range.second < range.first)
		{
			range.second = range.first;
		}

		ranges.append(range);
	}
}

bool BeamDirectionChart::FillSeries(const QMap<int, QPointF> &directions, const QPair<int, int> &range,
									QScatterSeries *series)
{
	bool exist = false;

	foreach (int count, directions.keys())
	{
		if (count >= range.first && count <= range.second)
		{
			exist = true;
			auto values = directions.values(count);

			foreach (QPointF v, values)
			{
				series->append(v.x(), v.y());
			}
		}
	}

	return exist;
}

void BeamDirectionChart::HideSeries()
{
	foreach (QAbstractSeries *s, series())
	{
		s->setVisible(false);
		((QScatterSeries*)s)->clear();
	}
}

void BeamDirectionChart::ShowSeries(QScatterSeries *s, const QPair<int, int> &r)
{
	s->setVisible(true);

	if (r.first == r.second)
	{
		s->setName(QString("%1").arg(r.first));
	}
	else
	{
		s->setName(QString("%1-%2").arg(r.first).arg(r.second));
	}
}

void BeamDirectionChart::Draw(const QMap<int, QPointF> &directions)
{
	HideSeries();

	int min, max;
	FindMinMax(directions, min, max);

	QVector<QPair<int, int>> ranges;
	GetRanges(min, max, ranges);

	auto c_series = series();

	for (int i = 0; i < c_series.size(); ++i)
	{
		QScatterSeries *s = (QScatterSeries*)c_series[i];
		QPair<int, int> r = ranges[i];

		bool exist = FillSeries(directions, r, s);

		if (exist)
		{
			ShowSeries(s, r);
		}
	}
}

void BeamDirectionChart::SetAxes()
{
	QValueAxis *phiAxis = new QValueAxis();
	phiAxis->setTickCount(17); // First and last ticks are co-located on 0/360 angle.
	phiAxis->setLabelFormat("%.1f");
	phiAxis->setShadesVisible(true);
	phiAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
	phiAxis->setReverse(true);
	addAxis(phiAxis, QPolarChart::PolarOrientationAngular);

	QValueAxis *thetaAxis = new QValueAxis();
	thetaAxis->setTickCount(7);
	thetaAxis->setLabelFormat("%d");
	addAxis(thetaAxis, QPolarChart::PolarOrientationRadial);

	thetaAxis->setRange(0, 180);
	phiAxis->setRange(0, 360);

	// set series
	foreach (const QColor &color, colors)
	{
		QScatterSeries *angleSeries = new QScatterSeries();
		angleSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
		angleSeries->setMarkerSize(10);
		angleSeries->setColor(color);

		addSeries(angleSeries);
		angleSeries->attachAxis(phiAxis);
		angleSeries->attachAxis(thetaAxis);
	}
}
