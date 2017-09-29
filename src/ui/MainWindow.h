#pragma once

#include <QMainWindow>
#include <QtCharts/QScatterSeries>
#include <QChartView>

#include "ParticleProxy.h"

QT_CHARTS_USE_NAMESPACE

class BeamItemModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_comboBox_types_currentIndexChanged(int index);
	void on_pushButton_clicked();
	void on_treeView_tracks_clicked(const QModelIndex &index);

	void on_lineEdit_search_textChanged(const QString &arg1);

private:
	Ui::MainWindow *ui;
	ParticleProxy p_proxy;
	BeamItemModel *model;
	QChartView *chartView;
	QScatterSeries *angleSeries;
	int precision;

private:
	void FillParticleTypes();
	void SetAdditionalParamName();
	void SetChart();
	void DrawBeamAnglePoints();
	void SetTrackTree();
	void ReadTraceParams(Angle &angle, int &reflNum);
};
