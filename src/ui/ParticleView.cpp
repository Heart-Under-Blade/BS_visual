#include "ParticleView.h"

#include "ParticleProxy.h"

#include <QMouseEvent>
#include <QDebug>
#include <QGraphicsTextItem>

ParticleView::ParticleView(QWidget *parent)
	: QGraphicsView(parent)
{
	isDrawLocalAxes = false;
	textSize = 5;
	mainPen.setColor(Qt::black);

	dashPen.setColor(Qt::gray);
	dashPen.setStyle(Qt::DashLine);
	dashPen.setDashPattern(QVector<qreal>{textSize, textSize*2});

	zoomFactor = 0.1;
	scene = new QGraphicsScene(this);
	setScene(scene);
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

ParticleView::~ParticleView()
{
}

void ParticleView::DrawTrack(const VisualParticle &particle)
{
	auto &track = particle.track;
	auto &facet = particle.trackLastFacet;

	if (track.isEmpty())
	{
		return;
	}

	QPen pen(Qt::red);

	QPen pen2 = dashPen;
	pen2.setColor(Qt::red);

	// polygons
//	for (const NumberedFacet &pol : track)
//	{
//		scene->addPolygon(pol.pol, pen, QBrush(Qt::green));
//	}

	foreach (const QPolygonF p, facet)
	{
		scene->addPolygon(p, pen, QBrush(Qt::green));
	}

//	scene->addPolygon(track.at(track.size()-2).pol, pen, QBrush(Qt::green));

	// lines
	QPointF c1 = track.first().pol.at(0);
	QPointF c2 = CenterOfPolygon(track.at(1).pol);
	scene->addLine(c1.x(), c1.y(), c2.x(), c2.y(), pen);
//	scene->addEllipse(c1.x()-2, c1.y()-2, 4, 4, pen, QBrush(Qt::green));

	for (int i = 2; i < track.size(); ++i)
	{
		c1 = c2;
		c2 = CenterOfPolygon(track.at(i).pol);
		scene->addLine(c1.x(), c1.y(), c2.x(), c2.y(), pen2);
		scene->addEllipse(c1.x()-2, c1.y()-2, 4, 4, pen, QBrush(Qt::red));
	}

	c1 = CenterOfPolygon(track.at(track.size()-2).pol);
	c2 = track.last().pol.at(0);
	scene->addLine(c1.x(), c1.y(), c2.x(), c2.y(), pen);
//	scene->addEllipse(c2.x()-2, c2.y()-2, 4, 4, pen, QBrush(Qt::green));

	// numbers
	QFont font;
	font.setBold(true);

	QPointF c3 = CenterOfPolygon(track.at(track.size()-1).pol);
	DrawColoredText("in", track.at(0).pol.at(0), mainPen.color(), font);
	scene->addEllipse(c3.x()-2, c3.y()-2, 4, 4, pen, QBrush(Qt::yellow));

	for (int i = 1; i < track.size()-1; ++i)
	{
		DrawFacetNumber(track.at(i), mainPen.color());
	}

//	DrawColoredText("out", track.last().pol.at(0), mainPen.color(), font);
}

void ParticleView::DrawParticle(const VisualParticle &particle,
								bool drawNumbers, bool drawAxes)
{
	scene->clear();
//	QBrush brush(QColor(QRgba64::fromRgba(0, 0, 255, 255/(1/sqrt(refrIndex*4)))));

	DrawFacets(particle.invisibleFacets, drawNumbers, dashPen);

	DrawFacets(particle.visibleFacets, drawNumbers,
			   QPen(Qt::blue), QBrush(Qt::cyan));

	DrawTrack(particle);

	if (drawAxes)
	{
		DrawAxes(particle.globalAxes, QPen(Qt::red), "w");
	}

	if (isDrawLocalAxes)
	{
		QPen pen = dashPen;
		pen.setColor(Qt::blue);
		DrawAxes(particle.localAxes, pen);
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

void ParticleView::DrawColoredText(const QString &text, const QPointF &pos,
								   const QColor &color, QFont font)
{
	font.setPointSize(textSize);
	QGraphicsTextItem *facetNumber = scene->addText(text, font);

	facetNumber->setDefaultTextColor(color);
	QPointF textCenter = facetNumber->boundingRect().center();
	double x = pos.x() - textCenter.x();
	double y = pos.y() - textCenter.y();
	facetNumber->setPos(x, y);
}

void ParticleView::DrawFacetNumber(const NumberedFacet &facet, const QColor &color)
{
	QPointF center = CenterOfPolygon(facet.pol);
	QString numText = QString::number(facet.num);
	DrawColoredText(numText, center, color);
}

void ParticleView::DrawAxis(const QPointF &axis, const QString &letter,
							const QPen &pen)
{
	double x = axis.x();
	double y = axis.y();
	scene->addLine(0, 0, x, y, pen);

	QGraphicsItem *text = scene->addText(letter);
	text->moveBy(x, y);

//	double len = sqrt(x*x + y*y);
//	double arrowSize = len/15;

//	QPolygonF arrow;
//	arrow << QPointF(x, y) << QPointF(x-arrowSize, y-arrowSize);
}

void ParticleView::DrawAxes(const QVector<QPointF> &axes, const QPen &pen,
							const QString &suffix)
{
	double size = textSize*2;

	DrawAxis(axes[0], "X" + suffix, pen);
	DrawAxis(axes[1], "Y" + suffix, pen);
	DrawAxis(axes[2], "Z" + suffix, pen);
//	QPolygonF arrowX;
//	arrowX << QPointF(size, 0) << QPointF(size-arrowSize, arrowSize/4)
//		   << QPointF(size-arrowSize, -arrowSize/4) << QPointF(size, 0);
//	scene->addPolygon(arrowX, redPen, redBrush);

//	QPolygonF arrowY;
//	arrowY << QPointF(0, size) << QPointF(arrowSize/4, size-arrowSize)
//		   << QPointF(-arrowSize/4, size-arrowSize) << QPointF(0, size);
//	scene->addPolygon(arrowY, redPen, redBrush);
	scene->addEllipse(-size, -size, size*2, size*2, pen);
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

void ParticleView::wheelEvent(QWheelEvent *event)
{
	QPoint wheel = event->angleDelta();

	double zoomIn = 1 + zoomFactor;
	double zoomOut = 1 - zoomFactor;

	if (!wheel.isNull())
	{
		if (wheel.ry() > 0)
		{
			scale(zoomIn, zoomIn);
		}
		else
		{
			scale(zoomOut, zoomOut);
		}
	}

}

void ParticleView::setDrawLocalAxes(bool value)
{
	isDrawLocalAxes = value;
}
