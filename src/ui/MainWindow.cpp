#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "BeamItemModel.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

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
}

void MainWindow::FillParticleTypes()
{
	QStringList types = p_proxy.GetParticleTypes();

	foreach (const QString &type, types)
	{
		ui->comboBox_types->addItem(type);
	}
}

void MainWindow::SetAdditionalParamName()
{
	QString type = ui->comboBox_types->currentText();
	QString paramName = p_proxy.GetAdditionalParticleParam(type);

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

void MainWindow::on_pushButton_clicked()
{
	QString type = ui->comboBox_types->currentText();
	double ri = ui->doubleSpinBox_refrIndex->value();
	double h = ui->doubleSpinBox_height->value();
	double d = ui->doubleSpinBox_diameter->value();

	if (ui->doubleSpinBox_additional->isVisible())
	{
		double add = ui->doubleSpinBox_additional->value();
		p_proxy.SetParticle(type, ri, h, d, add);
	}
	else
	{
		p_proxy.SetParticle(type, ri, h, d);
	}

	double b = ui->doubleSpinBox_beta->value();
	double g = ui->doubleSpinBox_gamma->value();
	double a = ui->doubleSpinBox_alpha->value();

	int reflNum = ui->spinBox_inter->value();
qDebug() << "----";
	p_proxy.Trace(b, g, a, reflNum);

	QString tracks = p_proxy.GetTracks();
	model = new BeamItemModel(QStringList{"Phi/Theta", "Beam number"}, tracks);
	ui->treeView_tracks->setModel(model);
}

void MainWindow::on_treeView_tracks_clicked(const QModelIndex &index)
{
	QVariant itemData = model->data(index, Qt::DisplayRole);
	QVariant parentData = model->data(index.parent(), Qt::DisplayRole);

	QString strData = itemData.toString();
//	qDebug() << strData << parentData.toString();

	if (strData.contains(':'))
	{
		BeamInfo info = p_proxy.GetBeamInfo(parentData.toString(), strData);
		ui->label_track->setText(info.track);
		ui->label_area->setText(QString::number(info.area));
		ui->label_optPath->setText(QString::number(info.beam.opticalPath));

		ui->label_m11->setText(QString::number(info.M.at(0)));
		ui->label_m12->setText(QString::number(info.M.at(1)));
		ui->label_m13->setText(QString::number(info.M.at(2)));
		ui->label_m14->setText(QString::number(info.M.at(3)));

		ui->label_m21->setText(QString::number(info.M.at(4)));
		ui->label_m22->setText(QString::number(info.M.at(5)));
		ui->label_m23->setText(QString::number(info.M.at(6)));
		ui->label_m24->setText(QString::number(info.M.at(7)));

		ui->label_m31->setText(QString::number(info.M.at(8)));
		ui->label_m32->setText(QString::number(info.M.at(9)));
		ui->label_m33->setText(QString::number(info.M.at(10)));
		ui->label_m34->setText(QString::number(info.M.at(11)));

		ui->label_m41->setText(QString::number(info.M.at(12)));
		ui->label_m42->setText(QString::number(info.M.at(13)));
		ui->label_m43->setText(QString::number(info.M.at(14)));
		ui->label_m44->setText(QString::number(info.M.at(15)));
	}
}
