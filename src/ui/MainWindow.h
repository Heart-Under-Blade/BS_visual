#pragma once

#include <QMainWindow>

#include "ParticleProxy.h"

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

private:
	Ui::MainWindow *ui;
	ParticleProxy p_proxy;
	BeamItemModel *model;

private:
	void FillParticleTypes();
	void SetAdditionalParamName();
};
