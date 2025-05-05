#ifndef DIALOGS_H
#define DIALOGS_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QGraphicsItem>

// 信息结构体，可扩展为大小、接口数量等
struct ComponentInfo {
    QString name;
    int width = 0;
    int height = 0;
    int portCount = 0;
    QString color;
};

namespace Ui {
class Dialogs;
}

class Dialogs : public QDialog
{
    Q_OBJECT

public:
    explicit Dialogs(QGraphicsItem* item, QWidget *parent = nullptr);
    ComponentInfo currentInfo;
    ~Dialogs();

private slots:
    void saveSetting();

    void on_nameEdit_textEdited(const QString &arg1);

    void on_saveButton_clicked();

    void on_colorCombo_activated(int index);

private:
    QGraphicsItem* targetItem = nullptr;
    Ui::Dialogs *ui;
    QLineEdit* nameEdit;
    QComboBox* colorCombo;
    QCheckBox* enableResize;
    QGraphicsRectItem* RectItem = nullptr;
};

#endif // DIALOGS_H
