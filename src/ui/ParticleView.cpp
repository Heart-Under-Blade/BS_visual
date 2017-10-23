#include "ParticleView.h"

#include "ParticleProxy.h"

#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsTextItem>

ParticleView::ParticleView(QWidget *parent)
	: QGraphicsView(parent)
{
	mainPen.setColor(Qt::black);

	invisPen.setColor(Qt::gray);
	invisPen.setStyle(Qt::DashLine);
	invisPen.setDashPattern(QVector<qreal>{5, 10});

	zoomFactor = 0.1;
	scene = new QGraphicsScene(this);
	setScene(scene);
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

ParticleView::~ParticleView()
{
}

void ParticleView::DrawTrack(const QVector<NumberedFacet> &track)
{
	if (track.isEmpty())
	{
		return;
	}

	QPen pen(Qt::red);
	QPen pen2 = invisPen;
	pen2.setColor(Qt::red);

	// polygons
	for (const NumberedFacet &pol : track)
	{
		scene->addPolygon(pol.pol, pen, QBrush(Qt::green));
	}

	QPointF c1 = CenterOfPolygon(track.at(0).pol);
	QPointF c2 = CenterOfPolygon(track.at(1).pol);
	scene->addLine(c1.x(), c1.y(), c2.x(), c2.y(), pen);
	scene->addEllipse(c1.x()-2, c1.y()-2, 4, 4, pen, QBrush(Qt::green));

	// lines
	for (int i = 2; i < track.size()-1; ++i)
	{
		c1 = c2;
		c2 = CenterOfPolygon(track.at(i).pol);
		scene->addLine(c1.x(), c1.y(), c2.x(), c2.y(), pen2);
		scene->addEllipse(c1.x()-2, c1.y()-2, 4, 4, pen, QBrush(Qt::green));
	}

	c1 = CenterOfPolygon(track.at(track.size()-2).pol);
	c2 = CenterOfPolygon(track.at(track.size()-1).pol);
	scene->addLine(c1.x(), c1.y(), c2.x(), c2.y(), pen);
	scene->addEllipse(c2.x()-2, c2.y()-2, 4, 4, pen, QBrush(Qt::green));

	// numbers
	for (int i = 1; i < track.size()-1; ++i)
	{
		DrawFacetNumber(track.at(i), mainPen.color());
	}
}

void ParticleView::DrawParticle(const VisualParticle &particle,
								bool drawNumbers, bool drawAxes)
{
	scene->clear();
//	QBrush brush(QColor(QRgba64::fromRgba(0, 0, 255, 255/(1/sqrt(refrIndex*4)))));

	DrawFacets(particle.invisibleFacets, drawNumbers, invisPen);

	DrawFacets(particle.visibleFacets, drawNumbers,
			   QPen(Qt::black), QBrush(Qt::cyan));

	DrawTrack(particle.track);

	if (drawAxes)
	{
		DrawAxes(particle.axes);
	}
}

QPointF ParticleView::CenterOfPolygon(const QPolygonF &pol)
{
	QPointF center;

	foreach (QPointF p, pol)
	{
		center.setX(center.x() + p.x());
		center.setY(center.y() + p.y());
	}

	center /= pol.size();
	return center;
}

void ParticleView::DrawFacetNumber(const NumberedFacet &facet, const QColor &color)
{
	QPointF center = CenterOfPolygon(facet.pol);

	QFont font;
	font.setPointSize(5);
	QString numText = QString::number(facet.num);
	QGraphicsTextItem *facetNumber = scene->addText(numText, font);

	facetNumber->setDefaultTextColor(color);
	QPointF textCenter = facetNumber->boundingRect().center();
	double x = center.x() - textCenter.x();
	double y = center.y() - textCenter.y();
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

void ParticleView::DrawFacets(const QVector<NumberedFacet> &facets, bool drawNumbers,
							  const QPen &pen, const QBrush &brush)
{
	for (int i = 0; i < facets.size(); ++i)
	{
		scene->addPolygon(facets[i].pol, pen, brush);

		if (drawNumbers)
		{
			DrawFacetNumber(facets[i], pen.color());
		}
	}
}

void ParticleView::Redraw()
{
}

void ParticleView::wheelEvent(QWheelEvent *event)
{
	QPoint wheel = event->angleDelta();
//  qDebug() << wheel;

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
