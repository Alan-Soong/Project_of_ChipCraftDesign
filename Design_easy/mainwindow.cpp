#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvasscene.h"
#include "canvasview.h"
#include "dialogs.h"

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

void MainWindow::on_actionhideLeftDock_triggered()
{
    ui->dockWidget->hide();
}


void MainWindow::on_actionsetLeftDock_triggered()
{
    ui->dockWidget->show();
}


void MainWindow::on_actionsave_triggered()
{
    //保存为TXT文件
    bool exist;
    QString fileName;

    QDir *folder = new QDir;
    QString str = "/home/alansoong";
    exist = folder->exists(str);//查看目录是否存在（例子是保存到桌面）

    if(!exist){//不存在就创建
        bool ok = folder->mkdir(str);
        if(ok){
            QMessageBox::warning(this,tr("创建目录"),tr("创建成功!"));//添加提示方便查看是否成功创建
        }else{
            QMessageBox::warning(this,tr("创建目录"),tr("创建失败"));
        }
    }

    QString fina = "/1.txt";
    fileName = QString("%1").arg(str + fina);
    // fileName = tr(str+fina).arg("数据");

    Dialogs dl(nullptr, this);
    dl.exec();

    QFile f(fileName);
    if(!f.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){// 追加写入 添加结束符\r\n
        QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
        return ;
    }else{
        // 获取 Dialogs 对象中的 currentInfo 并写入文件
        f.write(dl.currentInfo.name.toUtf8());
        f.write("\n");
        f.write(QString("Width: %1").arg(dl.currentInfo.width).toUtf8());
        f.write("\n");
        f.write(QString("Height: %1").arg(dl.currentInfo.height).toUtf8());
        f.write("\n");
        f.write(QString("Port Count: %1").arg(dl.currentInfo.portCount).toUtf8());
    }
    f.close();
}

