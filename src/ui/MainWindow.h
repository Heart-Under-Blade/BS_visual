#pragma once

#include <QMainWindow>
#include <QDialog>
#include <QChartView>

#include "ParticleProxy.h"
#include "BeamDirectionChart.h"

class BeamItemModel;
class ParticleView;

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
	void on_comboBox_types_currentIndexChanged(int);
	void on_pushButton_clicked();
	void on_treeView_tracks_clicked(const QModelIndex &index);
	void on_lineEdit_search_textChanged(const QString &arg1);
	void on_toolButton_resetRot_clicked();
	void on_toolButton_resetView_clicked();

	void DrawParticle();
	void DrawParticle(int);
	void DrawParticle(double);

	void ParticleChanged(int);
	void ParticleChanged(double);

	void on_toolButton_expandTree_clicked();

private:
	Ui::MainWindow *ui;
	ParticleProxy *p_proxy;
	BeamItemModel *model;
	QChartView *chartView;

	ParticleView *particleView;
	int precision;
	char format;
	QMap<QString, QVariant> state;
	bool hasAdditional;
	double coordinateOffset = 90;
	int beamNumber;
	bool drawTrack;
	bool isTreeExpanded;

	BeamDirectionChart *dirChart;
	QWidget *widget;

private:
	void FillParticleTypes();
	void SetAdditionalParamName();
	void SetDirectionChart();
	void SetBeamTree();
	Angle GetRotateAngle();
	Angle GetViewAngle();
	void SetParticle();
	void RecoverState();
	void SaveState();
	void WriteState();
	void ConnectWidgets();
	void SetParticleView();
	void FillResultBeamData(const BeamInfo &info);
	void SetViewAngle(const GlobalAngle &value);
	void DeleteModel();
	void GetDirections(QMap<int, QPointF> &directions);
};
