// #ifndef SETTINGDIALOG_H
// #define SETTINGDIALOG_H

// #include <QWidget>

// class settingdialog
// {
// public:
//     settingdialog();
// };

// #endif // SETTINGDIALOG_H

#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);

private slots:
    void saveSettings();

private:
    QLineEdit* nameEdit;
    QComboBox* colorCombo;
    QCheckBox* enableResize;
};

#endif // SETTINGDIALOG_H
