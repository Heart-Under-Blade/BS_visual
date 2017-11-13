#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "BeamItemModel.h"
#include "ParticleView.h"

#include <QDebug>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>

#ifdef _DEBUG // DEB
#include <iostream>
#endif

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	isTreeExpanded = false;
	ui->toolButton_expandTree->setArrowType(Qt::RightArrow);

	drawTrack = false;
	model = nullptr;
	format = 'f';
	p_proxy = new ParticleProxy();

	ui->mainToolBar->addAction("Settings");
	settingsDialog = new SettingsDialog();

	FillParticleTypes();

	LoadSession();
	AcceptSettings();

	SetAdditionalParam();
	SetDirectionChart();

	SetParticleView();

	ConnectWidgets();
}

MainWindow::~MainWindow()
{
	SaveSession();
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
	QObject::connect(ui->doubleSpinBox_cavityAngle, SIGNAL(valueChanged(double)),
					 this, SLOT(ParticleChanged(double)));
	QObject::connect(ui->doubleSpinBox_sizeIndex, SIGNAL(valueChanged(double)),
					 this, SLOT(ParticleChanged(double)));

	QObject::connect(ui->comboBox_types, SIGNAL(currentIndexChanged(int)),
					 this, SLOT(ParticleChanged(int)));

	QObject::connect(ui->checkBox_globalAxes, SIGNAL(stateChanged(int)),
					 this, SLOT(DrawParticle(int)));
	QObject::connect(ui->checkBox_numbers, SIGNAL(stateChanged(int)),
					 this, SLOT(DrawParticle(int)));
	QObject::connect(ui->checkBox_localAxes, SIGNAL(stateChanged(int)),
					 this, SLOT(DrawParticle(int)));

	QObject::connect(ui->mainToolBar->actions().last(), SIGNAL(triggered(bool)),
					 this, SLOT(OpenSettingsDialog(bool)));

	QObject::connect(settingsDialog, SIGNAL(accepted()),
					 this, SLOT(AcceptSettings()));
}

void MainWindow::SetInputPrecisions()
{
	int &precision = settings.inputPrecision;

	ui->doubleSpinBox_alpha->setDecimals(precision);
	ui->doubleSpinBox_beta->setDecimals(precision);
	ui->doubleSpinBox_gamma->setDecimals(precision);

	ui->doubleSpinBox_phi->setDecimals(precision);
	ui->doubleSpinBox_theta->setDecimals(precision);
	ui->doubleSpinBox_psy->setDecimals(precision);

	ui->doubleSpinBox_height->setDecimals(precision);
	ui->doubleSpinBox_diameter->setDecimals(precision);
	ui->doubleSpinBox_sizeIndex->setDecimals(precision);
	ui->doubleSpinBox_cavityAngle->setDecimals(precision);

	ui->doubleSpinBox_refrIndex->setDecimals(precision);
}

void MainWindow::SetParticleView()
{
	particleView = new ParticleView(this);
	ui->widget_particleView->setLayout(new QGridLayout());
	QGridLayout *lo = (QGridLayout*)ui->widget_particleView->layout();
	lo->addWidget(particleView, 6, 0, 1, 4);

	DrawParticle();
}

void MainWindow::SaveSession()
{
	QJsonObject patObj;
	{
		double height = ui->doubleSpinBox_height->value();
		patObj.insert("height", QJsonValue::fromVariant(height));

		double diameter = ui->doubleSpinBox_diameter->value();
		patObj.insert("diameter", QJsonValue::fromVariant(diameter));

		double cavityAngle = ui->doubleSpinBox_cavityAngle->value();
		patObj.insert("cavity angle", QJsonValue::fromVariant(cavityAngle));

		double sizeIndex = ui->doubleSpinBox_sizeIndex->value();
		patObj.insert("size index", QJsonValue::fromVariant(sizeIndex));

		double refrIndex = ui->doubleSpinBox_refrIndex->value();
		patObj.insert("refraction index", QJsonValue::fromVariant(refrIndex));

		double beta = ui->doubleSpinBox_beta->value();
		patObj.insert("beta", QJsonValue::fromVariant(beta));

		double gamma = ui->doubleSpinBox_gamma->value();
		patObj.insert("gamma", QJsonValue::fromVariant(gamma));

		double alpha = ui->doubleSpinBox_alpha->value();
		patObj.insert("alpha", QJsonValue::fromVariant(alpha));

		int inter = ui->spinBox_inter->value();
		patObj.insert("internal reflections", QJsonValue::fromVariant(inter));

		int type = ui->comboBox_types->currentIndex();
		patObj.insert("type", QJsonValue::fromVariant(type));
	}

	QJsonObject showObj;
	{
		bool globalAxes = ui->checkBox_globalAxes->isChecked();
		showObj.insert("global axes", QJsonValue::fromVariant(globalAxes));

		bool localAxes = ui->checkBox_localAxes->isChecked();
		showObj.insert("local axes", QJsonValue::fromVariant(localAxes));

		bool numbers = ui->checkBox_numbers->isChecked();
		showObj.insert("numbers", QJsonValue::fromVariant(numbers));
	}

	settingsDialog->GetSettings(settings);

	QJsonObject settingsObj;
	{
		settingsObj.insert("input precision", QJsonValue::fromVariant(settings.inputPrecision));
		settingsObj.insert("output precision", QJsonValue::fromVariant(settings.outputPrecision));
	}

	QJsonObject obj;
	obj.insert("particle", patObj);
	obj.insert("show elements", showObj);
	obj.insert("settings", settingsObj);

	QJsonDocument doc(obj);

	QFile file;
	file.setFileName("session.json");
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	file.write(doc.toJson());
	file.close();
}

void MainWindow::LoadSession()
{
	double h = 80, d = 40, ca = 0, si = 1, ri = 1.31,
			b = 0, g = 0, al = 0;
	int ir = 0, pt = 0;

	QFile file;
	file.setFileName("session.json");
	file.open(QIODevice::ReadOnly);

	QJsonParseError *error;
	QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), error);

	if (doc.isNull() || doc.isEmpty())
	{
		QString msg = error->errorString() + "Offset: " + QString::number(error->offset);
		QMessageBox::information(this, "Load session error", msg);
	}

	file.close();
	QJsonObject obj = doc.object();

	QJsonObject patObj = obj.value("particle").toObject();
	{
		double height = patObj.value("height").toDouble(h);
		ui->doubleSpinBox_height->setValue(height);

		double diameter = patObj.value("diameter").toDouble(d);
		ui->doubleSpinBox_diameter->setValue(diameter);

		double cavityAngle = patObj.value("cavity angle").toDouble(ca);
		ui->doubleSpinBox_cavityAngle->setValue(cavityAngle);

		double sizeIndex = patObj.value("size index").toDouble(si);
		ui->doubleSpinBox_sizeIndex->setValue(sizeIndex);

		double refrIndex = patObj.value("refraction index").toDouble(ri);
		ui->doubleSpinBox_refrIndex->setValue(refrIndex);

		double beta = patObj.value("beta").toDouble(b);
		ui->doubleSpinBox_beta->setValue(beta);

		double gamma = patObj.value("gamma").toDouble(g);
		ui->doubleSpinBox_gamma->setValue(gamma);

		double alpha = patObj.value("alpha").toDouble(al);
		ui->doubleSpinBox_alpha->setValue(alpha);

		int inter = patObj.value("internal reflections").toInt(ir);
		ui->spinBox_inter->setValue(inter);

		int type = patObj.value("type").toInt(pt);
		ui->comboBox_types->setCurrentIndex(type);
	}

	QJsonObject showObj = obj.value("show elements").toObject();
	{
		bool globalAxes = showObj.value("global axes").toBool(false);
		ui->checkBox_globalAxes->setChecked(globalAxes);

		bool localAxes = showObj.value("local axes").toBool(false);
		ui->checkBox_localAxes->setChecked(localAxes);

		bool numbers = showObj.value("numbers").toBool(true);
		ui->checkBox_numbers->setChecked(numbers);
	}

	QJsonObject settingsObj = obj.value("settings").toObject();
	{
		settings.inputPrecision = settingsObj.value("input precision").toInt(2);
		settings.outputPrecision = settingsObj.value("output precision").toInt(4);
	}

	settingsDialog->SetSettings(settings);
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

void MainWindow::OpenSettingsDialog(bool)
{
	settingsDialog->show();
}

void MainWindow::AcceptSettings()
{
	settingsDialog->GetSettings(settings);
	SetInputPrecisions();
}

void MainWindow::DrawParticle()
{
	bool drawNumbers = ui->checkBox_numbers->isChecked();
	bool drawAxes = ui->checkBox_globalAxes->isChecked();
	particleView->setDrawLocalAxes(ui->checkBox_localAxes->isChecked());
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

void MainWindow::SetAdditionalParam()
{
	QString type = p_proxy->GetParticleType();

	if (type == "Hollow column")
	{
		ui->doubleSpinBox_cavityAngle->show();
		ui->label_cavAngle->show();

		ui->doubleSpinBox_sizeIndex->hide();
		ui->label_sizeIndex->hide();
	}
	else if (type == "Aggregate")
	{
		ui->doubleSpinBox_sizeIndex->show();
		ui->label_sizeIndex->show();

		ui->doubleSpinBox_cavityAngle->hide();
		ui->label_cavAngle->hide();
	}
	else
	{
		ui->doubleSpinBox_cavityAngle->hide();
		ui->label_cavAngle->hide();

		ui->doubleSpinBox_sizeIndex->hide();
		ui->label_sizeIndex->hide();
	}
}

void MainWindow::on_comboBox_types_currentIndexChanged(int)
{
	SetParticle();
	SetAdditionalParam();
}

void MainWindow::DeleteModel()
{
	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
}

void MainWindow::SetBeamTree()
{
	QString tracks = p_proxy->GetBeamDataString();
	int total = p_proxy->GetTotalBeamCount();
	QString beamNum = QString("Number of beams (total: %1)").arg(total);
	model = new BeamItemModel(QStringList{"Phi, Theta", beamNum}, tracks);
	ui->treeView_tracks->setModel(model);
}

void MainWindow::SetParticle()
{
	QString t = ui->comboBox_types->currentText();
	double ri = ui->doubleSpinBox_refrIndex->value();
	double h = ui->doubleSpinBox_height->value();
	double d = ui->doubleSpinBox_diameter->value();

	if (t == "Hollow column")
	{
		double add = ui->doubleSpinBox_cavityAngle->value();
		p_proxy->SetParticle(t, ri, h, d, add);
	}
	else if (t == "Aggregate")
	{
		double add = ui->doubleSpinBox_sizeIndex->value();
		p_proxy->SetParticle(t, ri, h, d, add);
	}
	else
	{
		p_proxy->SetParticle(t, ri, h, d);
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

void MainWindow::GetDirections(QMap<int, QPointF> &directions)
{
	BeamMap map = p_proxy->GetBeamMap();

	foreach (QString tKey, map.keys())
	{
		BeamData data = map.value(tKey);
		BeamInfo info = data.value(data.keys().at(0));

		QPointF dir = QPointF(info.phiDeg, info.thetaDeg);
		int count = data.size();
		directions.insertMulti(count, dir);
	}
}

void MainWindow::on_pushButton_clicked()
{
	ParticleChanged(0.f);
	SetParticle();

	Angle angle = GetRotateAngle();
	Angle viewAngle = GetViewAngle();
	int reflNum = ui->spinBox_inter->value();

	p_proxy->Trace(angle, viewAngle, reflNum);

	SetBeamTree();

	QMap<int, QPointF> directions;
	GetDirections(directions);

	dirChart->Draw(directions);
}

void MainWindow::SetDirectionChart()
{
	dirChart = new BeamDirectionChart();

	chartView = new QChartView();
	chartView->setChart(dirChart);
	chartView->setRenderHint(QPainter::Antialiasing);

	ui->widget_chart->setLayout(new QGridLayout());
	ui->widget_chart->layout()->addWidget(chartView);
}

void MainWindow::FillResultBeamData(const BeamInfo &info)
{
	int &precision = settings.outputPrecision;

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

	beamNumber = itemData.toString().mid(2).toUInt(NULL, 16);

	BeamInfo info;
	p_proxy->GetBeamByNumber(beamNumber, info);
	FillResultBeamData(info);
	SetViewAngle(GlobalAngle(info.phiDeg-180, info.thetaDeg-180, 0));
	drawTrack = true;

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
		SetBeamTree();
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
	ui->doubleSpinBox_psy->setValue(value.psy);
	ui->doubleSpinBox_phi->setValue(value.phi);
	ui->doubleSpinBox_theta->setValue(value.theta);
}

void MainWindow::on_toolButton_resetView_clicked()
{
	SetViewAngle(GlobalAngle());
	DrawParticle(0.0f);
}

void MainWindow::on_toolButton_expandTree_clicked()
{
	if (isTreeExpanded)
	{
		ui->treeView_tracks->collapseAll();
		ui->toolButton_expandTree->setArrowType(Qt::RightArrow);
		isTreeExpanded = false;
	}
	else
	{
		ui->treeView_tracks->expandAll();
		ui->toolButton_expandTree->setArrowType(Qt::DownArrow);
		isTreeExpanded = true;
	}
}
