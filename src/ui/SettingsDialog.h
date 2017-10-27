#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

struct Settings
{
	int inputPrecision;
	int outputPrecision;
};

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();

	void SetSettings(const Settings &setts);
	void GetSettings(Settings &setts);

private slots:
	void on_buttonBox_rejected();

private:
	Ui::SettingsDialog *ui;
	Settings settings;

private:
	void ResetSettings();
};

#endif // SETTINGSDIALOG_H
