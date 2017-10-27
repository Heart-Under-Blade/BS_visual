#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	setModal(true);
	setWindowTitle("Settings");

	ResetSettings();
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::SetSettings(const Settings &setts)
{
	settings = setts;
	ResetSettings();
}

void SettingsDialog::GetSettings(Settings &setts)
{
	settings.inputPrecision = ui->spinBox_input->value();
	settings.outputPrecision = ui->spinBox_output->value();
	setts = settings;
}

void SettingsDialog::ResetSettings()
{
	ui->spinBox_input->setValue(settings.inputPrecision);
	ui->spinBox_output->setValue(settings.outputPrecision);
}

void SettingsDialog::on_buttonBox_rejected()
{
	ResetSettings();
}
