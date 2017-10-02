#include "MainWindow.h"
#include <QApplication>
#include <QtDataVisualization>

#include "ParticleView.h"

using namespace QtDataVisualization;

/**
 * TODO delete .qrc files
 * TODO delete ParticleModel/View
 */

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	Q3DSurface surface;
	surface.setFlags(surface.flags() ^ Qt::FramelessWindowHint);
	QSurfaceDataArray *data = new QSurfaceDataArray;
	QSurfaceDataRow *dataRow1 = new QSurfaceDataRow;
	QSurfaceDataRow *dataRow2 = new QSurfaceDataRow;

//	*dataRow1 << QVector3D(0.0f, 0.1f, 0.5f) << QVector3D(1.0f, 0.5f, 0.5f);
//	*dataRow2 << QVector3D(0.0f, 1.8f, 1.0f) << QVector3D(1.0f, 1.2f, 1.0f);
//	*dataRow1 << QVector3D(0, 0, 0) << QVector3D(1, 1, 0) /*<< QVector3D(0, 1, 0)*/;
//	*dataRow2 << QVector3D(0, 1, 1) << QVector3D(1, 1, 1) /*<< QVector3D(0, 1, 1)*/;
	*data << dataRow1 << dataRow2;

	QSurface3DSeries *series = new QSurface3DSeries;
	series->dataProxy()->resetArray(data);
	surface.addSeries(series);
	surface.resize(800, 600);
//	surface.show();

//	ParticleView pview;
//	pview.show();

	return a.exec();
}
