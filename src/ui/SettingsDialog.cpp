#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	setModal(true);
	setWindowTitle("Settings");

	// default values
	inputPrecision = 2;
	outputPrecision = 4;

	ResetSettings();
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::GetPrecisions(int &input, int &output)
{
	input = inputPrecision = ui->spinBox_input->value();
	output = outputPrecision = ui->spinBox_output->value();
}

void SettingsDialog::ResetSettings()
{
	ui->spinBox_input->setValue(inputPrecision);
	ui->spinBox_output->setValue(outputPrecision);
}

void SettingsDialog::on_buttonBox_rejected()
{
	ResetSettings();
}
