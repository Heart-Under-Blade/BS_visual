#pragma once

#include <QGraphicsView>

#include "ParticleModel.h"

class NumberedFacet;

class ParticleView : public QGraphicsView
{
	Q_OBJECT
public:
	ParticleView(QWidget *parent = 0);
	~ParticleView();

	void DrawParticle(const QVector<NumberedFacet> &facets, bool isShowNumbers);
	void DrawAxes(const QVector<QPointF> &axes);

	QGraphicsScene *scene;

public slots:
	void Redraw();

	// QWidget interface
protected:
	void wheelEvent(QWheelEvent *event) override;

private:
	double zoomFactor;

private:
	void DrawFacetNumber(const QPointF &pos, int num);
	void DrawAxis(const QPointF &axis, const QString &letter);
};
