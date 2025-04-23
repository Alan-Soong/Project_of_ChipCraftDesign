// #include "mytestproject.h"
// #include "ui_mytestproject.h"

// MyTestProject::MyTestProject(QWidget *parent)
//     : QMainWindow(parent)
//     , ui(new Ui::MyTestProject)
// {
//     ui->setupUi(this);
// }

// MyTestProject::~MyTestProject()
// {
//     delete ui;
// }

#include "mytestproject.h"
#include "ui_mytestproject.h"
#include "customscene.h"
#include <QMessageBox>

MyTestProject::MyTestProject(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyTestProject)
{
    ui->setupUi(this);

    scene = new CustomScene(this);
    ui->graphicsView->setScene(scene);

    connect(scene, &CustomScene::sceneClicked,
            this, &MyTestProject::on_sceneClicked);

    connect(ui->btnAddTemplate, &QPushButton::clicked,
            this, &MyTestProject::on_btnAddTemplate_clicked);
    connect(ui->btnRemoveTemplate, &QPushButton::clicked,
            this, &MyTestProject::on_btnRemoveTemplate_clicked);
    connect(ui->btnEditTemplate, &QPushButton::clicked,
            this, &MyTestProject::on_btnEditTemplate_clicked);
    connect(ui->btnSaveTemplate, &QPushButton::clicked,
            this, &MyTestProject::on_btnSaveTemplate_clicked);
    connect(ui->twTemplates, &QTableWidget::itemSelectionChanged,
            this, &MyTestProject::on_twTemplates_itemSelectionChanged);

    connect(ui->btnAddPortGroup, &QPushButton::clicked,
            this, &MyTestProject::on_btnAddPortGroup_clicked);
    connect(ui->btnRemovePortGroup, &QPushButton::clicked,
            this, &MyTestProject::on_btnRemovePortGroup_clicked);

    refreshTemplateList();
}

MyTestProject::~MyTestProject()
{
    delete ui;
}

// mytestproject.cpp
QList<UnitTemplate> unitTemplates;  // 定义

void MyTestProject::refreshTemplateList() {
    ui->twTemplates->clearContents();
    ui->twTemplates->setRowCount(unitTemplates.size());
    for (int i = 0; i < unitTemplates.size(); ++i) {
        QTableWidgetItem *item = new QTableWidgetItem(unitTemplates[i].name());
        ui->twTemplates->setItem(i, 0, item);
    }
}

void MyTestProject::refreshForm() {
    if (currentIndex < 0 || currentIndex >= unitTemplates.size()) return;
    const UnitTemplate &ut = unitTemplates[currentIndex];

    ui->leTemplateName->setText(ut.name());
    ui->sbPortWidth->setValue(ut.width());
    ui->sbPortHeight->setValue(ut.height());

    ui->twPortGroups->clearContents();
    ui->twPortGroups->setRowCount(ut.portGroups().size());
    for (int i = 0; i < ut.portGroups().size(); ++i) {
        const PortGroup &pg = ut.portGroups()[i];
        ui->twPortGroups->setItem(i, 0, new QTableWidgetItem(pg.name()));
        ui->twPortGroups->setItem(i, 1, new QTableWidgetItem(QString::number(pg.position().x())));
        ui->twPortGroups->setItem(i, 2, new QTableWidgetItem(QString::number(pg.position().y())));
        ui->twPortGroups->setItem(i, 3, new QTableWidgetItem(QString::number(pg.size().width())));
        ui->twPortGroups->setItem(i, 4, new QTableWidgetItem(QString::number(pg.size().height())));
    }
}

void MyTestProject::refreshScene() {
    if (currentIndex < 0 || currentIndex >= unitTemplates.size()) return;
    scene->loadUnit(unitTemplates[currentIndex]);
}

void MyTestProject::on_btnAddTemplate_clicked() {
    UnitTemplate ut("NewTemplate", 100, 100);
    unitTemplates.append(ut);
    currentIndex = unitTemplates.size() - 1;
    refreshTemplateList();
    refreshForm();
    refreshScene();
}

void MyTestProject::on_btnRemoveTemplate_clicked() {
    if (currentIndex >= 0 && currentIndex < unitTemplates.size()) {
        unitTemplates.removeAt(currentIndex);
        currentIndex = -1;
        refreshTemplateList();
        refreshForm();
        scene->clear();
    }
}

void MyTestProject::on_btnEditTemplate_clicked() {
    if (currentIndex < 0 || currentIndex >= unitTemplates.size()) return;
    unitTemplates[currentIndex].setName(ui->leTemplateName->text());
    unitTemplates[currentIndex].setWidth(ui->sbPortWidth->value());
    unitTemplates[currentIndex].setHeight(ui->sbPortHeight->value());
    refreshTemplateList();
    refreshScene();
}

void MyTestProject::on_btnSaveTemplate_clicked() {
    // 可加入文件保存逻辑
    QMessageBox::information(this, "Save", "Template saved (not implemented).");
}

void MyTestProject::on_twTemplates_itemSelectionChanged() {
    currentIndex = ui->twTemplates->currentRow();
    refreshForm();
    refreshScene();
}

void MyTestProject::on_btnAddPortGroup_clicked() {
    if (currentIndex < 0 || currentIndex >= unitTemplates.size()) return;
    PortGroup pg("P", QPoint(0, 0), QSize(10, 10));
    unitTemplates[currentIndex].addPortGroup(pg);
    refreshForm();
    refreshScene();
}

void MyTestProject::on_btnRemovePortGroup_clicked() {
    if (currentIndex < 0 || currentIndex >= unitTemplates.size()) return;
    int row = ui->twPortGroups->currentRow();
    unitTemplates[currentIndex].removePortGroup(row);
    refreshForm();
    refreshScene();
}

void MyTestProject::on_sceneClicked(const QPointF &pos) {
    ui->sbPortX->setValue(static_cast<int>(pos.x()));
    ui->sbPortY->setValue(static_cast<int>(pos.y()));
}
