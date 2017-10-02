#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "BeamItemModel.h"
#include "ParticleProxy.h"
//#include "ParticleView.h"

#include <QDebug>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	model = nullptr;
	precision = 4;
	p_proxy = new ParticleProxy();

	double h = 80, d = 40, ri = 1.31, b = 45, g = 30;
	int ir = 3;

	if (!std::ifstream("state.dat"))
	{
		std::ofstream stateOut("state.dat", std::ios::out);
		stateOut << h << ' ' << d << ' ' << ri << ' '
				 << b << ' ' << g << ' ' << ir << ' ';
		stateOut.close();
	}

	std::ifstream stateIn("state.dat", std::ios::in);
	stateIn >> h >> d >> ri >> b >> g >> ir;
	stateIn.close();

	ui->doubleSpinBox_height->setValue(h);
	ui->doubleSpinBox_diameter->setValue(d);
	ui->doubleSpinBox_refrIndex->setValue(ri);
	ui->doubleSpinBox_beta->setValue(b);
	ui->doubleSpinBox_gamma->setValue(g);
	ui->spinBox_inter->setValue(ir);

	FillParticleTypes();
	SetAdditionalParamName();

	SetChart();

	scene = new QGraphicsScene(this);
	ui->graphicsView_particle->setScene(scene);

	QObject::connect(ui->doubleSpinBox_alpha, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));
	QObject::connect(ui->doubleSpinBox_beta, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));
	QObject::connect(ui->doubleSpinBox_gamma, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));

	DrawParticle(0.0f);
//	ParticleView pview;
//	ui->groupBox_beam->layout()->addWidget(&pview);
}

MainWindow::~MainWindow()
{
	std::ofstream state("state.dat", std::ios::out);
	double h = ui->doubleSpinBox_height->value();
	double d = ui->doubleSpinBox_diameter->value();
	double ri = ui->doubleSpinBox_refrIndex->value();
	double b = ui->doubleSpinBox_beta->value();
	double g = ui->doubleSpinBox_gamma->value();
	int ir = ui->spinBox_inter->value();
	state << h << ' ' << d << ' ' << ri << ' '
		  << b << ' ' << g << ' ' << ir << ' ';
	delete ui;
	delete p_proxy;
}

void MainWindow::DrawParticle(double)
{
	scene->clear();
	SetParticle();

	Particle *particle = p_proxy->GetParticle();

	Angle angle = GetRotateAngle();
	particle->Rotate(DegToRad(angle.beta),
					 DegToRad(angle.gamma),
					 DegToRad(angle.alpha));

	for (int i = 0; i < particle->facetNum; ++i)
	{
		Facet &facet = particle->facets[i];
		QPolygonF pol;

		for (int j = 0; j < facet.size; ++j)
		{
			Point3f &p = particle->facets[i].arr[j];
			pol.append(QPointF(p.c_x, p.c_y));
		}

		scene->addPolygon(pol, QPen()/*, QBrush(Qt::red)*/);
	}
}

void MainWindow::FillParticleTypes()
{
	QStringList types = p_proxy->GetParticleTypes();

	foreach (const QString &type, types)
	{
		ui->comboBox_types->addItem(type);
	}
}

void MainWindow::SetAdditionalParamName()
{
	QString type = ui->comboBox_types->currentText();
	QString paramName = p_proxy->GetAdditionalParticleParam(type);

	if (!paramName.isEmpty())
	{
		ui->label_additional->setText(paramName);
	}
	else
	{
		ui->label_additional->hide();
		ui->doubleSpinBox_additional->hide();
	}
}

void MainWindow::on_comboBox_types_currentIndexChanged(int index)
{
	ui->comboBox_types->setCurrentIndex(index);
}

void MainWindow::DrawBeamAnglePoints()
{
	angleSeries->clear();
	TrackMap trackMap = p_proxy->GetTrackMap();

	foreach (QString tKey, trackMap.keys())
	{
		BeamData data = trackMap.value(tKey);

		foreach (QString dKey, data.keys())
		{
			BeamInfo info = data.value(dKey);
			angleSeries->append(info.phiDeg, info.thetaDeg);
		}
	}
}

void MainWindow::SetTrackTree()
{
	if (model != nullptr)
	{
		delete model;
	}

	QString tracks = p_proxy->GetBeamDataString();
	model = new BeamItemModel(QStringList{"Phi, Theta", "Beam number"}, tracks);
	ui->treeView_tracks->setModel(model);
}

void MainWindow::SetParticle()
{
	QString type = ui->comboBox_types->currentText();
	double ri = ui->doubleSpinBox_refrIndex->value();
	double h = ui->doubleSpinBox_height->value();
	double d = ui->doubleSpinBox_diameter->value();

	if (ui->doubleSpinBox_additional->isVisible())
	{
		double add = ui->doubleSpinBox_additional->value();
		p_proxy->SetParticle(type, ri, h, d, add);
	}
	else
	{
		p_proxy->SetParticle(type, ri, h, d);
	}
}

Angle MainWindow::GetRotateAngle()
{
	Angle a;
	a.alpha = ui->doubleSpinBox_alpha->value();
	a.beta = ui->doubleSpinBox_beta->value();
	a.gamma = ui->doubleSpinBox_gamma->value();
	return a;
}

void MainWindow::on_pushButton_clicked()
{
	SetParticle();

	Angle angle = GetRotateAngle();
	int reflNum = ui->spinBox_inter->value();

	p_proxy->Trace(angle, reflNum);

	SetTrackTree();
	DrawBeamAnglePoints();
}

void MainWindow::SetChart()
{
	QPolarChart *chart = new QPolarChart();

//	chart->setTitle("Use arrow keys to scroll, +/- to zoom, and space to switch chart type.");

	QValueAxis *phiAxis = new QValueAxis();
	phiAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
	phiAxis->setLabelFormat("%.1f");
	phiAxis->setShadesVisible(true);
	phiAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
	phiAxis->setReverse(true);
	chart->addAxis(phiAxis, QPolarChart::PolarOrientationAngular);

	QValueAxis *thetaAxis = new QValueAxis();
	thetaAxis->setTickCount(7);
	thetaAxis->setLabelFormat("%d");
	chart->addAxis(thetaAxis, QPolarChart::PolarOrientationRadial);

	thetaAxis->setRange(0, 180);
	phiAxis->setRange(0, 360);

	angleSeries = new QScatterSeries();
	angleSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
	angleSeries->setMarkerSize(10);

	chart->addSeries(angleSeries);
	angleSeries->attachAxis(phiAxis);
	angleSeries->attachAxis(thetaAxis);

	chartView = new QChartView();
	chartView->setChart(chart);
//	chartView->setRenderHint(QPainter::Antialiasing);

	ui->widget_chart->setLayout(new QGridLayout());
	ui->widget_chart->layout()->addWidget(chartView);
}

void MainWindow::on_treeView_tracks_clicked(const QModelIndex &index)
{
	if (model->hasChildren(index)) // is not root element
	{
		return;
	}

	QModelIndex secondColIndex = model->index(index.row(), 1, index.parent());
	QVariant itemData = model->data(secondColIndex, Qt::DisplayRole);

	int beamNumber = itemData.toString().toInt();

	BeamInfo info = p_proxy->GetBeamByNumber(beamNumber);
	ui->label_track->setText(info.track);
	ui->label_area->setText(QString::number(info.area, 'g', precision));
	ui->label_optPath->setText(QString::number(info.beam.opticalPath, 'g', precision));

	ui->label_m11->setText(QString::number(info.M.at(0), 'g', precision));
	ui->label_m12->setText(QString::number(info.M.at(1), 'g', precision));
	ui->label_m13->setText(QString::number(info.M.at(2), 'g', precision));
	ui->label_m14->setText(QString::number(info.M.at(3), 'g', precision));

	ui->label_m21->setText(QString::number(info.M.at(4), 'g', precision));
	ui->label_m22->setText(QString::number(info.M.at(5), 'g', precision));
	ui->label_m23->setText(QString::number(info.M.at(6), 'g', precision));
	ui->label_m24->setText(QString::number(info.M.at(7), 'g', precision));

	ui->label_m31->setText(QString::number(info.M.at(8), 'g', precision));
	ui->label_m32->setText(QString::number(info.M.at(9), 'g', precision));
	ui->label_m33->setText(QString::number(info.M.at(10), 'g', precision));
	ui->label_m34->setText(QString::number(info.M.at(11), 'g', precision));

	ui->label_m41->setText(QString::number(info.M.at(12), 'g', precision));
	ui->label_m42->setText(QString::number(info.M.at(13), 'g', precision));
	ui->label_m43->setText(QString::number(info.M.at(14), 'g', precision));
	ui->label_m44->setText(QString::number(info.M.at(15), 'g', precision));
}

void MainWindow::on_lineEdit_search_textChanged(const QString &arg1)
{
	if (!arg1.isEmpty())
	{
		ui->lineEdit_search->setFont(QFont());

		if (model != nullptr)
		{
			delete model;
		}

		QString tracks = p_proxy->GetBeamDataString(arg1);
		model = new BeamItemModel(QStringList{"Phi, Theta", "Beam number"}, tracks);
		ui->treeView_tracks->setModel(model);
		ui->treeView_tracks->expandAll();
	}
	else
	{
		SetTrackTree();
		ui->treeView_tracks->collapseAll();

		QFont stFont;
		stFont.setItalic(true);
		ui->lineEdit_search->setFont(stFont);
	}
}
