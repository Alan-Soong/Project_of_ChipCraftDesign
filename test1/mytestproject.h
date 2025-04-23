// #ifndef MYTESTPROJECT_H
// #define MYTESTPROJECT_H

// #include <QMainWindow>

// QT_BEGIN_NAMESPACE
// namespace Ui {
// class MyTestProject;
// }
// QT_END_NAMESPACE

// class MyTestProject : public QMainWindow
// {
//     Q_OBJECT

// public:
//     MyTestProject(QWidget *parent = nullptr);
//     ~MyTestProject();

// private:
//     Ui::MyTestProject *ui;
// };
// #endif // MYTESTPROJECT_H

// mytestproject.h
#ifndef MYTESTPROJECT_H
#define MYTESTPROJECT_H

#include <QMainWindow>
#include "portgroup.h"
#include "unittemplate.h"
#include "customscene.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MyTestProject; }
QT_END_NAMESPACE

class MyTestProject : public QMainWindow {
    Q_OBJECT

public:
    explicit MyTestProject(QWidget *parent = nullptr);
    ~MyTestProject();

private:
    Ui::MyTestProject *ui;
    CustomScene    *scene;
    QList<UnitTemplate> templates;
    int currentIndex{-1};

    // Helper methods
    void refreshTemplateList();
    void refreshForm();
    void refreshScene();

private slots:
    // Template actions
    void on_btnAddTemplate_clicked();
    void on_btnRemoveTemplate_clicked();
    void on_btnEditTemplate_clicked();
    void on_btnSaveTemplate_clicked();
    void on_twTemplates_itemSelectionChanged();

    // PortGroup actions
    void on_btnAddPortGroup_clicked();
    void on_btnRemovePortGroup_clicked();

    // Scene interaction
    void on_sceneClicked(const QPointF &pos);
};

extern QList<UnitTemplate> unitTemplates;

#endif // MYTESTPROJECT_H

