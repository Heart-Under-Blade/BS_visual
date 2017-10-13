#include "ParticleView.h"

#include "ParticleProxy.h"

#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsTextItem>

ParticleView::ParticleView(QWidget *parent)
	: QGraphicsView(parent)
{
	zoomFactor = 0.1;
	scene = new QGraphicsScene(this);
	setScene(scene);
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

ParticleView::~ParticleView()
{
}

void ParticleView::DrawParticle(const QVector<NumberedFacet> &facets, bool isShowNumbers)
{
//	for (int i = facets.size()-1; i >= 0; --i)
	for (int i = 0; i < facets.size(); ++i)
	{
		scene->addPolygon(facets[i].pol, QPen(Qt::blue), QBrush(QColor(Qt::white)));

		if (isShowNumbers)
		{
			QPointF center = facets[i].pol.boundingRect().center();
			DrawFacetNumber(center, facets[i].num);
		}
	}
}

void ParticleView::DrawFacetNumber(const QPointF &pos, int num)
{
	QFont font;
	font.setPointSize(5);
	QString numText = QString::number(num);
	QGraphicsTextItem *facetNumber = scene->addText(numText, font);

	QPointF textCenter = facetNumber->boundingRect().center();
	double x = pos.x() - textCenter.x();
	double y = pos.y() - textCenter.y();
	facetNumber->setPos(x, y);
}

void ParticleView::DrawAxis(const QPointF &axis, const QString &letter)
{
	double x = axis.x();
	double y = axis.y();

	QPen redPen(Qt::red);
	scene->addLine(0, 0, x, y, redPen);

	QGraphicsItem *text = scene->addText(letter);
	text->moveBy(x, y);

//	double len = sqrt(x*x + y*y);
//	double arrowSize = len/15;

//	QPolygonF arrow;
//	arrow << QPointF(x, y) << QPointF(x-arrowSize, y-arrowSize);
}

void ParticleView::DrawAxes(const QVector<QPointF> &axes)
{
//	QBrush redBrush(Qt::red);
	double size = 10;

	DrawAxis(axes[0], "X");
	DrawAxis(axes[1], "Y");
	DrawAxis(axes[2], "Z");
//	QPolygonF arrowX;
//	arrowX << QPointF(size, 0) << QPointF(size-arrowSize, arrowSize/4)
//		   << QPointF(size-arrowSize, -arrowSize/4) << QPointF(size, 0);
//	scene->addPolygon(arrowX, redPen, redBrush);

//	QPolygonF arrowY;
//	arrowY << QPointF(0, size) << QPointF(arrowSize/4, size-arrowSize)
//		   << QPointF(-arrowSize/4, size-arrowSize) << QPointF(0, size);
//	scene->addPolygon(arrowY, redPen, redBrush);
	// pseudo axis 'Z'
	scene->addEllipse(-size, -size, size*2, size*2, QPen(Qt::red));
}

void ParticleView::Redraw()
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
