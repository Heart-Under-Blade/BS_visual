#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "BeamItemModel.h"
#include "ParticleProxy.h"
#include "ParticleView.h"

#include <QDebug>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>

#ifdef _DEBUG // DEB
#include <iostream>
#endif

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	model = nullptr;
	precision = 4;
	p_proxy = new ParticleProxy();

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
					 this, SLOT(DrawParticle(double)));
	QObject::connect(ui->doubleSpinBox_beta, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));
	QObject::connect(ui->doubleSpinBox_gamma, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));

	QObject::connect(ui->doubleSpinBox_view_alpha, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));
	QObject::connect(ui->doubleSpinBox_view_beta, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));
	QObject::connect(ui->doubleSpinBox_view_gamma, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));

	QObject::connect(ui->doubleSpinBox_height, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));
	QObject::connect(ui->doubleSpinBox_diameter, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));
	QObject::connect(ui->doubleSpinBox_additional, SIGNAL(valueChanged(double)),
					 this, SLOT(DrawParticle(double)));

	QObject::connect(ui->comboBox_types, SIGNAL(currentIndexChanged(int)),
					 this, SLOT(DrawParticle(int)));
}

void MainWindow::SetParticleView()
{
	scene = new QGraphicsScene(this);
	particleView = new ParticleView(this);
	particleView->setScene(scene);
	particleView->setRenderHints(QPainter::Antialiasing
								 | QPainter::SmoothPixmapTransform);

	QGridLayout *lo = (QGridLayout*)ui->groupBox_input->layout();
	lo->addWidget(particleView, 5, 0, 1, 4);
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
	DrawParticle(0.0f);
}

void MainWindow::DrawFacetNumber(QPointF pos, int num)
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

void MainWindow::DrawParticle(double)
{
	scene->clear();
	SetParticle();

	Angle rotAngle = GetRotateAngle();
	Angle viewAngle = GetViewAngle();
	QPolygonF axes = p_proxy->Rotate(rotAngle, viewAngle);

	QVector<NumberedFacet> facets;
	p_proxy->GetVisibleFacets(facets);

//	QPen dashPen;
//	dashPen.setStyle(Qt::DashLine);
//	dashPen.setColor(Qt::gray);

//	for (int i = facets.size()-1; i >= 0; --i)
	for (int i = 0; i < facets.size(); ++i)
	{
		scene->addPolygon(facets[i].pol, /*dashPen*/ QPen(Qt::blue), QBrush(QColor(Qt::white)));
		QPointF center = facets[i].pol.boundingRect().center();
		DrawFacetNumber(center, facets[i].num);
	}

	QPen redPen;
	redPen.setColor(Qt::red);

	DrawAxes(axes);
}

void MainWindow::DrawAxis(const QPointF &axis, const QString &letter)
{
	double x = axis.x();
	double y = axis.y();

	QPen redPen(Qt::red);
	scene->addLine(0, 0, x, y, redPen);

	QGraphicsItem *text = scene->addText(letter);
	text->moveBy(x, y);

	double len = sqrt(x*x + y*y);
	double arrowSize = len/15;

	QPolygonF arrow;
	arrow << QPointF(x, y) << QPointF(x-arrowSize, y-arrowSize);
}

void MainWindow::DrawAxes(const QVector<QPointF> &axes)
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
	a.alpha = DegToRad(ui->doubleSpinBox_view_alpha->value() + coordinateOffset);
	a.beta = DegToRad(ui->doubleSpinBox_view_beta->value());
	a.gamma = DegToRad(ui->doubleSpinBox_view_gamma->value());
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

void MainWindow::SetAngleChart()
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

	BeamInfo info;
	p_proxy->GetBeamByNumber(beamNumber, info);

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
