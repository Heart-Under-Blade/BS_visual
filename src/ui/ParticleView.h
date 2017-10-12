#pragma once

#include <QGraphicsView>

#include "ParticleModel.h"

class ParticleView : public QGraphicsView
{
	Q_OBJECT
public:
	ParticleView(QWidget *parent = 0);
	~ParticleView();

	// QWidget interface
protected:
	void wheelEvent(QWheelEvent *event) override;

private:
	double zoomFactor;
};
