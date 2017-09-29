#include "ChartView.h"
#include <QtGui/QMouseEvent>
#include <QtCore/QDebug>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>

QT_CHARTS_USE_NAMESPACE

#ifdef _DEBUG // DEB
#include <QDebug>
#endif

ChartView::ChartView(QWidget *parent)
	: QChartView(parent)
{
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Plus:
		chart()->zoomIn();
		break;
	case Qt::Key_Minus:
		chart()->zoomOut();
		break;
	case Qt::Key_Left:
		chart()->scroll(-1.0, 0);
		break;
	case Qt::Key_Right:
		chart()->scroll(1.0, 0);
		break;
	case Qt::Key_Up:
		chart()->scroll(0, 1.0);
		break;
	case Qt::Key_Down:
		chart()->scroll(0, -1.0);
		break;
	default:
		QGraphicsView::keyPressEvent(event);
		break;
	}
}

void ChartView::wheelEvent(QWheelEvent *event)
{
	QPoint wheel = event->angleDelta();
//	qDebug() << wheel;

	if (!wheel.isNull())
	{
		if (wheel.ry() > 0)
		{
			qDebug() << "zoomIn";
			chart()->zoomIn();
		}
		else
		{
			qDebug() << "zoomOut";
			chart()->zoomOut();
		}
	}
}
