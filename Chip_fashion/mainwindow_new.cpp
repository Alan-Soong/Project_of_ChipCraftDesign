#include "mainwindow_new.h"
#include "ui_mainwindow.h"
#include "canvasscene.h"
#include "canvasview.h"
#include "chipmanager.h"
#include "filemanager.h"
#include "componentsettingsdialog.h"
#include "pineditordialog.h"
#include "command.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_scene(nullptr)
    , m_view(nullptr)
    , m_undoStack(new QUndoStack(this))
    , m_chipManager(nullptr)
    , m_fileManager(nullptr)
    , m_isModified(false)
{
    ui->setupUi(this);
    
    // 设置应用程序图标
    setWindowIcon(QIcon(":/app_icon.svg"));
    
    loadStyleSheet();
    setupUI();
    setupManagers();
    connectSignals();
    
    updateWindowTitle();
    
    // 设置状态栏
    statusBar()->showMessage("就绪", 2000);
    
    // 创建初始芯片
    on_addRectangleButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 设置场景和视图
    m_scene = new CanvasScene(this);
    m_scene->setSceneRect(0, 0, 4000, 4000);
    
    m_view = new CanvasView(m_scene);
    m_view->setParent(this);
    setCentralWidget(m_view);
}

void MainWindow::loadStyleSheet()
{
    // 尝试加载样式表文件
    QFile styleFile(":/styles.qss");
    if (!styleFile.exists()) {
        // 如果资源文件不存在，尝试从当前目录加载
        styleFile.setFileName("styles.qss");
    }
    
    if (styleFile.open(QFile::ReadOnly)) {
        QTextStream stream(&styleFile);
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
        qDebug() << "样式表加载成功";
    } else {
        qDebug() << "样式表加载失败:" << styleFile.errorString();
        // 如果无法加载文件，使用内嵌的基本样式
        setDefaultStyle();
    }
}

void MainWindow::setDefaultStyle()
{
    // 设置基本的现代化样式
    QString basicStyle = R"(
        QMainWindow {
            background-color: #f3f3f3;
        }
        QPushButton {
            background-color: #ffffff;
            border: 2px solid #e1dfdd;
            border-radius: 6px;
            padding: 8px 16px;
            font-size: 14px;
            color: #323130;
        }
        QPushButton:hover {
            background-color: #f3f2f1;
            border-color: #c8c6c4;
        }
        QPushButton:pressed {
            background-color: #edebe9;
        }
    )";
    qApp->setStyleSheet(basicStyle);
}

void MainWindow::setupManagers()
{
    // 初始化管理器
    m_chipManager = new ChipManager(this);
    m_fileManager = new FileManager(this);
}

void MainWindow::connectSignals()
{
    // 连接菜单信号
    connect(ui->actionnew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionopen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionsave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionsaveOther, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(ui->actionexit, &QAction::triggered, this, &MainWindow::exportFiles);

    // 连接文件管理器信号
    connect(m_fileManager, &FileManager::fileOperationCompleted,
            this, &MainWindow::onFileOperationCompleted);
}

void MainWindow::newFile()
{
    m_scene->clear();
    m_currentFilePath.clear();
    m_chipManager->resetChipCounter();
    m_isModified = false;
    
    updateWindowTitle("新文件");
    
    // 创建初始芯片
    on_addRectangleButton_clicked();
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "打开文件", "", "设计文件 (*.txt);;所有文件 (*)");
    if (filePath.isEmpty()) return;

    if (m_fileManager->openDesignFile(filePath, m_scene)) {
        m_currentFilePath = filePath;
        m_isModified = false;
        updateWindowTitle(QFileInfo(filePath).fileName());
    } else {
        QMessageBox::warning(this, "错误", "无法打开文件：" + m_fileManager->getLastError());
    }
}

void MainWindow::saveFile()
{
    if (m_currentFilePath.isEmpty()) {
        saveFileAs();
        return;
    }

    QList<CellItem*> cellItems = m_scene->getAllCellItems();
    if (m_fileManager->saveDesignFile(m_currentFilePath, cellItems, m_scene)) {
        m_isModified = false;
        updateWindowTitle(QFileInfo(m_currentFilePath).fileName());
    } else {
        QMessageBox::warning(this, "错误", "无法保存文件：" + m_fileManager->getLastError());
    }
}

void MainWindow::saveFileAs()
{
    QString filePath = QFileDialog::getSaveFileName(this, "另存为", "", "设计文件 (*.txt);;所有文件 (*)");
    if (filePath.isEmpty()) return;

    QList<CellItem*> cellItems = m_scene->getAllCellItems();
    if (m_fileManager->saveDesignFile(filePath, cellItems, m_scene)) {
        m_currentFilePath = filePath;
        m_isModified = false;
        updateWindowTitle(QFileInfo(filePath).fileName());
    } else {
        QMessageBox::warning(this, "错误", "无法保存文件：" + m_fileManager->getLastError());
    }
}

void MainWindow::exportFiles()
{
    QString filePath = QFileDialog::getSaveFileName(this, "导出宏文件", "", "宏文件 (*.macro);;所有文件 (*)");
    if (filePath.isEmpty()) return;

    QList<CellItem*> cellItems = m_scene->getAllCellItems();
    if (m_fileManager->exportMacroFile(filePath, cellItems)) {
        QMessageBox::information(this, "成功", "宏文件导出成功");
    } else {
        QMessageBox::warning(this, "错误", "无法导出宏文件：" + m_fileManager->getLastError());
    }
}

void MainWindow::on_addRectangleButton_clicked()
{
    QPointF pos(2000, 2000);
    
    // 使用命令模式支持撤销/重做
    int chipCounter = m_chipManager->getChipCounter();
    m_undoStack->push(new AddRectangleCommand(m_scene, pos, chipCounter, m_undoStack));
    
    m_isModified = true;
    updateWindowTitle();
    
    qDebug() << "添加芯片到位置" << pos;
}

void MainWindow::on_selectButton_clicked()
{
    // 选择第一个未选中的芯片
    QList<CellItem*> cellItems = m_scene->getAllCellItems();
    for (CellItem* cellItem : cellItems) {
        if (!cellItem->isSelected()) {
            m_scene->clearSelection();
            cellItem->setSelected(true);
            qDebug() << "选中芯片" << cellItem->getInstanceName() << "位置:" << cellItem->pos();
            return;
        }
    }
    QMessageBox::information(this, "提示", "没有可选择的芯片");
}

void MainWindow::on_undoButton_clicked()
{
    if (m_undoStack->canUndo()) {
        m_undoStack->undo();
        m_isModified = true;
        updateWindowTitle();
    }
}

void MainWindow::on_redoButton_clicked()
{
    if (m_undoStack->canRedo()) {
        m_undoStack->redo();
        m_isModified = true;
        updateWindowTitle();
    }
}

void MainWindow::on_deleteButton_clicked()
{
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择要删除的项目");
        return;
    }

    for (QGraphicsItem* item : selectedItems) {
        if (CellItem* cellItem = dynamic_cast<CellItem*>(item)) {
            m_undoStack->push(new DeleteRectangleCommand(m_scene, cellItem, m_undoStack));
        }
    }
    
    m_isModified = true;
    updateWindowTitle();
}

void MainWindow::onFileOperationCompleted(const QString& operation, bool success)
{
    if (success) {
        qDebug() << "文件操作完成:" << operation;
    } else {
        QMessageBox::warning(this, "错误", "文件操作失败: " + operation);
    }
}

void MainWindow::onComponentSettingsSaved()
{
    m_isModified = true;
    updateWindowTitle();
}

void MainWindow::updateWindowTitle(const QString& fileName)
{
    QString title = "芯片设计工具";
    
    if (!fileName.isEmpty()) {
        title += " - " + fileName;
    } else if (!m_currentFilePath.isEmpty()) {
        title += " - " + QFileInfo(m_currentFilePath).fileName();
    } else {
        title += " - 新文件";
    }
    
    if (m_isModified) {
        title += " *";
    }
    
    setWindowTitle(title);
}
