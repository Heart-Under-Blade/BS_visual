#pragma once

#include <QGraphicsView>

#include "ParticleModel.h"

class NumberedFacet;

struct VisualParticle
{
	double refrIndex;
	QVector<QPointF> localAxes;
	QVector<QPointF> globalAxes;
	QVector<NumberedFacet> track;
	QVector<QPolygonF> trackLastFacet;
	QVector<NumberedFacet> visibleFacets;
	QVector<NumberedFacet> invisibleFacets;
};

class ParticleView : public QGraphicsView
{
	Q_OBJECT
public:
	ParticleView(QWidget *parent = 0);
	~ParticleView();

	void DrawParticle(const VisualParticle &particle,
					  bool drawNumbers, bool drawAxes);

	void setDrawLocalAxes(bool value);

	// QWidget interface
protected:
	void wheelEvent(QWheelEvent *event) override;

private:
	QGraphicsScene *scene;
	double zoomFactor;

	QPen mainPen;
	QPen dashPen;

	int textSize;
	bool isDrawLocalAxes;

private:
	void DrawFacetNumber(const NumberedFacet &facet, const QColor &color);
	void DrawAxis(const QPointF &axis, const QString &letter,
				  const QPen &pen = QPen());
	void DrawAxes(const QVector<QPointF> &axes, const QPen &pen,
				  const QString &suffix = QString());
	void DrawFacets(const QVector<NumberedFacet> &facets, bool drawNumbers,
					const QPen &pen, const QBrush &brush = QBrush());
	void DrawTrack(const VisualParticle &particle);
	QPointF CenterOfPolygon(const QPolygonF &pol);
	void DrawColoredText(const QString &text, const QPointF &pos,
						 const QColor &color, QFont font = QFont());
};
