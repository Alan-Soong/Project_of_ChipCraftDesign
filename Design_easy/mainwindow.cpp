#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvasscene.h"
#include "canvasview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_canvasView(new CanvasView(this))
    , m_canvasScene(new CanvasScene(this))
{
    ui->setupUi(this);
    qDebug() << "MainWindow initialized";

    // // 设置画布
    // m_canvasView->setScene(m_canvasScene);
    // setCentralWidget(m_canvasView);
    // 使用 UI 中的 canvasView 作为画布
    ui->canvasView->setScene(m_canvasScene);

    // 设置适当的场景大小
    m_canvasScene->setSceneRect(0, 0, 1800, 1600);

    // 可以设置抗锯齿或边框等效果
    ui->canvasView->setRenderHint(QPainter::Antialiasing);

    // setupConnections();
    qDebug() << "Connections set up";
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
void MainWindow::setupConnections() {
    connect(ui->addRectangleButton, &QPushButton::clicked, this, &MainWindow::on_addRectangleButton_clicked);
    connect(ui->selectButton, &QPushButton::clicked, this, &MainWindow::on_selectButton_clicked);
    connect(ui->undoButton, &QPushButton::clicked, this, &MainWindow::on_undoButton_clicked);
    connect(ui->redoButton, &QPushButton::clicked, this, &MainWindow::on_redoButton_clicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::on_deleteButton_clicked);
}
*/


void MainWindow::on_addRectangleButton_clicked() {
    m_canvasScene->saveSnapshot(); // 保存当前状态
    // 在画布上添加矩形
    CellItem *newItem = new CellItem();
    m_canvasScene->addCellItem(newItem);
}

void MainWindow::on_selectButton_clicked() {
    // 设置选择工具
    m_canvasScene->setSelectionMode(true);
}

void MainWindow::on_undoButton_clicked() {
    // 撤销操作
    m_canvasScene->undoAction();
}

void MainWindow::on_redoButton_clicked() {
    // 重做操作
    m_canvasScene->redoAction();
}

void MainWindow::on_deleteButton_clicked() {
    // 删除选中的项
    m_canvasScene->deleteSelectedItems();
}
