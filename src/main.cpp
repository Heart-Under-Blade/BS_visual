#include "MainWindow.h"
#include <QApplication>

#include "ParticleView.h"

/**
 * TODO delete .qrc files
 * TODO delete ParticleModel/View
 */

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
