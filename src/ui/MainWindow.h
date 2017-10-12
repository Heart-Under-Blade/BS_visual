#pragma once

#include <QMainWindow>
#include <QtCharts/QScatterSeries>
#include <QChartView>

QT_CHARTS_USE_NAMESPACE

class Angle;
class ParticleProxy;
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

	void DrawParticle(double);
	void DrawParticle(int);

private:
	Ui::MainWindow *ui;
	ParticleProxy *p_proxy;
	BeamItemModel *model;
	QChartView *chartView;
	QScatterSeries *angleSeries;
	ParticleView *particleView;
	QGraphicsScene *scene;
	int precision;
	QMap<QString, QVariant> state;
	bool hasAdditional;

private:
	void FillParticleTypes();
	void SetAdditionalParamName();
	void SetAngleChart();
	void DrawBeamAnglePoints();
	void SetTrackTree();
	Angle GetRotateAngle();
	Angle GetViewAngle();
	void SetParticle();
	void DrawAxes(const QVector<QPointF> &axes);
	void RecoverState();
	void SaveState();
	void WriteState();
	void ConnectWidgets();
	void SetParticleView();
	void DrawFacetNumber(QPointF pos, int num);
	void DrawAxis(const QPointF &axis, const QString &letter);
};
