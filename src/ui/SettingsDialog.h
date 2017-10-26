#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = 0);
	~SettingsDialog();

	void GetPrecisions(int &input, int &output);

private slots:
	void on_buttonBox_rejected();

private:
	Ui::SettingsDialog *ui;

	int inputPrecision;
	int outputPrecision;
	void ResetSettings();
};

#endif // SETTINGSDIALOG_H
