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

public slots:
	void Redraw();

	// QWidget interface
protected:
	void wheelEvent(QWheelEvent *event) override;

private:
	QGraphicsScene *scene;
	double zoomFactor;

	QPen mainPen;
	QPen dashPen;

	int textSize;

private:
	void DrawFacetNumber(const NumberedFacet &facet, const QColor &color);
	void DrawAxis(const QPointF &axis, const QString &letter,
				  const QPen &pen = QPen(Qt::red));
	void DrawAxes(const QVector<QPointF> &axes);
	void DrawFacets(const QVector<NumberedFacet> &facets, bool drawNumbers,
					const QPen &pen, const QBrush &brush = QBrush());
	void DrawTrack(const QVector<NumberedFacet> &track);
	QPointF CenterOfPolygon(const QPolygonF &pol);
	void DrawColoredText(const QString &text, const QPointF &pos,
						 const QColor &color, QFont font = QFont());
};
