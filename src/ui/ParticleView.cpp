#include "ParticleView.h"

#include <QMouseEvent>
#include <QDebug>

ParticleView::ParticleView(QWidget *parent)
	: QGraphicsView(parent)
{
	zoomFactor = 0.1;
}

ParticleView::~ParticleView()
{
}

void ParticleView::wheelEvent(QWheelEvent *event)
{
	QPoint wheel = event->angleDelta();
//    qDebug() << wheel;

	double zoomIn = 1 + zoomFactor;
	double zoomOut = 1 - zoomFactor;

	if (!wheel.isNull())
	{
		if (wheel.ry() > 0)
		{
//			qDebug() << "zoomIn";
			scale(zoomIn, zoomIn);
		}
		else
		{
//			qDebug() << "zoomOut";
			scale(zoomOut, zoomOut);
		}
	}

}
