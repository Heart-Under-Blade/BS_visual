#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "BeamItemModel.h"
#include "ParticleView.h"

#include <QDebug>
#include <QLegendMarker>

#ifdef _DEBUG // DEB
#include <iostream>
#endif

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	drawTrack = false;
	model = nullptr;
	precision = 4;
	format = 'f';
	p_proxy = new ParticleProxy();

//	ui->mainToolBar->addAction()

	FillParticleTypes();

	RecoverState();

	SetAdditionalParamName();
	SetAngleChart();

	SetParticleView();

	ConnectWidgets();

	DrawParticle(0.0f);
}

MainWindow::~MainWindow()
{
	SaveState();
	delete ui;
	delete p_proxy;
	delete particleView;
}

void MainWindow::ConnectWidgets()
{
	QObject::connect(ui->doubleSpinBox_alpha, SIGNAL(valueChanged(double)),
					 this, SLOT(ParticleChanged(double)));
	QObject::connect(ui->doubleSpinBox_beta, SIGNAL(valueChanged(double)),
					 this, SLOT(ParticleChanged(double)));
	QObject::connect(ui->doubleSpinBox_gamma, SIGNAL(valueChanged(double)),
					 this, SLOT(ParticleChanged(double)));

	QObject::connect(ui->doubleSpinBox_phi, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));
	QObject::connect(ui->doubleSpinBox_theta, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));
	QObject::connect(ui->doubleSpinBox_psy, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));

	QObject::connect(ui->doubleSpinBox_height, SIGNAL(valueChanged(double)),
					 this, SLOT(ParticleChanged(double)));
	QObject::connect(ui->doubleSpinBox_diameter, SIGNAL(valueChanged(double)),
					 this, SLOT(ParticleChanged(double)));
	QObject::connect(ui->doubleSpinBox_additional, SIGNAL(valueChanged(double)),
					 this, SLOT(ParticleChanged(double)));

	QObject::connect(ui->comboBox_types, SIGNAL(currentIndexChanged(int)),
					 this, SLOT(ParticleChanged(int)));

	QObject::connect(ui->checkBox_axes, SIGNAL(stateChanged(int)),
					 this, SLOT(DrawParticle(int)));
	QObject::connect(ui->checkBox_numbers, SIGNAL(stateChanged(int)),
					 this, SLOT(DrawParticle(int)));
}

void MainWindow::SetParticleView()
{
	particleView = new ParticleView(this);
	ui->widget_particleView->setLayout(new QGridLayout());
	QGridLayout *lo = (QGridLayout*)ui->widget_particleView->layout();
	lo->addWidget(particleView, 6, 0, 1, 4);

	widget = new QWidget();
	widget->setLayout(new QGridLayout());
	QGridLayout *go = (QGridLayout*)widget->layout();
	go->addWidget(ui->groupBox_particlePreivew, 6, 0, 1, 4);
	widget->resize(800, 600);
	widget->setFocus();
	widget->show();
}

void MainWindow::WriteState()
{
	std::ofstream stateFile("state.dat", std::ios::out);

//	foreach (QString key, state.keys())
//	{
//		stateFile << state.value(key).toDouble();
//	}
	stateFile << state["h"].toDouble()
			<< ' ' << state["d"].toDouble()
			<< ' ' << state["ad"].toDouble()
			<< ' ' << state["ri"].toDouble()
			<< ' ' << state["b"].toDouble()
			<< ' ' << state["g"].toDouble()
			<< ' ' << state["ir"].toInt()
			<< ' ' << state["pt"].toInt();

	stateFile.close();
}

void MainWindow::SaveState()
{
	state["h"] = ui->doubleSpinBox_height->value();
	state["d"] = ui->doubleSpinBox_diameter->value();
	state["ad"] = ui->doubleSpinBox_additional->value();
	state["ri"] = ui->doubleSpinBox_refrIndex->value();
	state["b"] = ui->doubleSpinBox_beta->value();
	state["g"] = ui->doubleSpinBox_gamma->value();
	state["ir"] = ui->spinBox_inter->value();
	state["pt"] = ui->comboBox_types->currentIndex();

	WriteState();
}

void MainWindow::RecoverState()
{
	double h = 80, d = 40, ad = 0, ri = 1.31,
			b = 45, g = 30, ir = 3, pt = 0;

	if (!std::ifstream("state.dat"))
	{
		WriteState();
	}

	std::ifstream stateIn("state.dat", std::ios::in);
	stateIn >> h >> d >> ad >> ri >> b >> g >> ir >> pt;
	stateIn.close();

	ui->doubleSpinBox_height->setValue(h);
	ui->doubleSpinBox_diameter->setValue(d);
	ui->doubleSpinBox_additional->setValue(ad);
	ui->doubleSpinBox_refrIndex->setValue(ri);
	ui->doubleSpinBox_beta->setValue(b);
	ui->doubleSpinBox_gamma->setValue(g);
	ui->spinBox_inter->setValue(std::lround(ir));
	ui->comboBox_types->setCurrentIndex(std::lround(pt));
}

void MainWindow::DrawParticle(int)
{
	DrawParticle();
}

void MainWindow::DrawParticle(double)
{
	DrawParticle();
}

void MainWindow::ParticleChanged(int)
{
	ParticleChanged(0.f);
}

void MainWindow::ParticleChanged(double)
{
	drawTrack = false;
	DeleteModel();
	DrawParticle();
}

void MainWindow::DrawParticle()
{
	bool drawNumbers = ui->checkBox_numbers->isChecked();
	bool drawAxes = ui->checkBox_axes->isChecked();

	SetParticle();

	Angle rotAngle = GetRotateAngle();
	Angle viewAngle = GetViewAngle();
	p_proxy->RotateParticle(rotAngle, viewAngle);

	VisualParticle particle;
	p_proxy->GetVisibleFacets(particle.visibleFacets, particle.invisibleFacets);
	particle.refrIndex = ui->doubleSpinBox_refrIndex->value();
	particle.localAxes = p_proxy->RotateLocalAxes(rotAngle, viewAngle);
	particle.globalAxes = p_proxy->RotateGlobalAxes(viewAngle);
	particle.track.clear();

	if (drawTrack)
	{
		p_proxy->GetTrack(beamNumber, viewAngle, particle.track);
	}

	particleView->DrawParticle(particle, drawNumbers, drawAxes);
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
		ui->label_additional->show();
		ui->doubleSpinBox_additional->show();
		hasAdditional = true;
	}
	else
	{
		ui->label_additional->hide();
		ui->doubleSpinBox_additional->hide();
		hasAdditional = false;
	}
}

void MainWindow::on_comboBox_types_currentIndexChanged(int)
{
	SetAdditionalParamName();
}

void MainWindow::DrawBeamAnglePoints()
{
	QVector<QPair<QPointF, int>> angles;
	int max = 0, min = INT_MAX;

	TrackMap trackMap = p_proxy->GetTrackMap();

	foreach (QString tKey, trackMap.keys())
	{
		BeamData data = trackMap.value(tKey);
		BeamInfo info = data.value(data.keys().at(0));

		QPair<QPointF, int> angleData;
		angleData.first = QPointF(info.phiDeg, info.thetaDeg);
		angleData.second = data.size();
		angles.append(angleData);

		if (angleData.second < min)
		{
			min = angleData.second;
		}

		if (angleData.second > max)
		{
			max = angleData.second;
		}
	}

	float coef = (float)(max - min)/colors.size();

	QVector<QPair<int, int>> ranges;

	QPair<int, int> range;
	range.first = min;
	range.second = min + lround(coef);
	ranges.append(range);

	for (int i = 1; i < colors.size(); ++i)
	{
		range.first = ranges.last().second + 1;
		range.second = min + lround(coef*(i+1));
		ranges.append(range);
	}

	for (int i = 0; i < angles.size(); ++i)
	{
		int beamCount = angles[i].second;
		int id = 0;

		for (int j = 0; j < ranges.size(); ++j)
		{
			if (beamCount >= ranges[j].first
					&& beamCount <= ranges[j].second)
			{
				id = j;
			}
		}

		QScatterSeries *series = (QScatterSeries*)chart->series().at(id);

		QPair<int, int> r = ranges[id];
		series->setName(QString("%1-%2").arg(r.first).arg(r.second));

		QPointF a = angles[i].first;
		series->append(a.x(), a.y());
	}
}

void MainWindow::DeleteModel()
{
	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
}

void MainWindow::SetTrackTree()
{
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

	if (hasAdditional)
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
	a.alpha = DegToRad(ui->doubleSpinBox_alpha->value());
	a.beta = DegToRad(ui->doubleSpinBox_beta->value());
	a.gamma = DegToRad(ui->doubleSpinBox_gamma->value());
	return a;
}

Angle MainWindow::GetViewAngle()
{
	Angle a;
	a.alpha = -DegToRad(ui->doubleSpinBox_phi->value() - coordinateOffset);
	a.beta = DegToRad(ui->doubleSpinBox_theta->value());
	a.gamma = DegToRad(ui->doubleSpinBox_psy->value());
	return a;
}

void MainWindow::on_pushButton_clicked()
{
	ParticleChanged(0.f);
	SetParticle();

	Angle angle = GetRotateAngle();
	Angle viewAngle = GetViewAngle();
	int reflNum = ui->spinBox_inter->value();

	p_proxy->Trace(angle, viewAngle, reflNum);

	SetTrackTree();
	DrawBeamAnglePoints();
}

void MainWindow::SetAngleChart()
{
	colors.append(QColor(0, 0, 255));
	colors.append(QColor(109, 0, 255));
	colors.append(QColor(164, 0, 255));
	colors.append(QColor(255, 0, 255));
	colors.append(QColor(255, 0, 144));
	colors.append(QColor(255, 0, 0));

	chart = new QPolarChart();

//	chart->setTitle("Use arrow keys to scroll, +/- to zoom, and space to switch chart type.");

	phiAxis = new QValueAxis();
	phiAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
	phiAxis->setLabelFormat("%.1f");
	phiAxis->setShadesVisible(true);
	phiAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
	phiAxis->setReverse(true);
	chart->addAxis(phiAxis, QPolarChart::PolarOrientationAngular);

	thetaAxis = new QValueAxis();
	thetaAxis->setTickCount(7);
	thetaAxis->setLabelFormat("%d");
	chart->addAxis(thetaAxis, QPolarChart::PolarOrientationRadial);

	thetaAxis->setRange(0, 180);
	phiAxis->setRange(0, 360);

	foreach (const QColor &color, colors)
	{
		QScatterSeries *angleSeries = new QScatterSeries();
		angleSeries->setMarkerShape(QScatterSeries::MarkerShapeCircle);
		angleSeries->setMarkerSize(10);
		angleSeries->setColor(color);

		chart->addSeries(angleSeries);
		angleSeries->attachAxis(phiAxis);
		angleSeries->attachAxis(thetaAxis);
	}

	chartView = new QChartView();
	chartView->setChart(chart);
//	chart->legend()->setVisible(false);
//	chartView->setRenderHint(QPainter::Antialiasing);

	ui->widget_chart->setLayout(new QGridLayout());
	ui->widget_chart->layout()->addWidget(chartView);
}

void MainWindow::FillResultBeamData(const BeamInfo &info)
{
	ui->label_track->setText(info.track);
	ui->label_area->setText(QString::number(info.area, format, precision));
	ui->label_optPath->setText(QString::number(info.beam.opticalPath, format, precision));

	auto M = info.M;

	ui->label_m11->setText(QString::number(M.at(0), format, precision));
	ui->label_m12->setText(QString::number(M.at(1), format, precision));
	ui->label_m13->setText(QString::number(M.at(2), format, precision));
	ui->label_m14->setText(QString::number(M.at(3), format, precision));

	ui->label_m21->setText(QString::number(M.at(4), format, precision));
	ui->label_m22->setText(QString::number(M.at(5), format, precision));
	ui->label_m23->setText(QString::number(M.at(6), format, precision));
	ui->label_m24->setText(QString::number(M.at(7), format, precision));

	ui->label_m31->setText(QString::number(M.at(8), format, precision));
	ui->label_m32->setText(QString::number(M.at(9), format, precision));
	ui->label_m33->setText(QString::number(M.at(10), format, precision));
	ui->label_m34->setText(QString::number(M.at(11), format, precision));

	ui->label_m41->setText(QString::number(M.at(12), format, precision));
	ui->label_m42->setText(QString::number(M.at(13), format, precision));
	ui->label_m43->setText(QString::number(M.at(14), format, precision));
	ui->label_m44->setText(QString::number(M.at(15), format, precision));
}

void MainWindow::on_treeView_tracks_clicked(const QModelIndex &index)
{
	if (model->hasChildren(index)) // is not root element
	{
		drawTrack = false;
		return;
	}

	QModelIndex secondColIndex = model->index(index.row(), 1, index.parent());
	QVariant itemData = model->data(secondColIndex, Qt::DisplayRole);

	beamNumber = itemData.toString().toInt();
	drawTrack = true;

	BeamInfo info;
	p_proxy->GetBeamByNumber(beamNumber, info);
	FillResultBeamData(info);

	GlobalAngle outAngle;
	outAngle.phi = info.phiDeg - 180;
	outAngle.theta = info.thetaDeg - 180;
	outAngle.psy = 0;
	SetViewAngle(outAngle);

	DrawParticle();
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

void MainWindow::on_toolButton_resetRot_clicked()
{
	ui->doubleSpinBox_alpha->setValue(0.0f);
	ui->doubleSpinBox_beta->setValue(0.0f);
	ui->doubleSpinBox_gamma->setValue(0.0f);
	DrawParticle(0.0f);
}

void MainWindow::SetViewAngle(const GlobalAngle &value)
{
	ui->doubleSpinBox_phi->setValue(value.phi);
	ui->doubleSpinBox_theta->setValue(value.theta);
	ui->doubleSpinBox_psy->setValue(value.psy);
}

void MainWindow::on_toolButton_resetView_clicked()
{
	SetViewAngle(GlobalAngle());
	DrawParticle(0.0f);
}
