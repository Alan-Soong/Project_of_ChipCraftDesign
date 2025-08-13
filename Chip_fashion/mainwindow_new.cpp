#include "mainwindow_new.h"
#include "ui_mainwindow.h"
#include "canvasscene.h"
#include "canvasview.h"
#include "connectionline.h"
#include "chipmanager.h"
#include "filemanager.h"
#include "pineditordialog.h"
#include "command.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QTimer>
#include <QStatusBar>
#include <QTimer>

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

    // 注释掉自动创建初始芯片，让用户手动添加或通过文件加载
    // on_addRectangleButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 设置场景和视图，默认显示-1000,0到1000,1000的区域
    m_scene = new CanvasScene(this);
    m_scene->setSceneRect(-500, 0, 500, 1000);

    // 设置撤销栈
    m_scene->setUndoStack(m_undoStack);

    m_view = new CanvasView(m_scene);
    m_view->setParent(this);
    setCentralWidget(m_view);

    // 初始视图适配场景矩形
    QTimer::singleShot(100, [this]() {
        if (m_view && m_scene) {
            m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
        }
    });
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

    // 创建初始芯片 - 只在用户明确选择新建文件时创建
    on_addRectangleButton_clicked();
}

void MainWindow::newProject()
{
    // 清理当前项目，准备新建项目或打开文件
    m_scene->clear();
    m_currentFilePath.clear();
    m_chipManager->resetChipCounter();
    m_isModified = false;

    updateWindowTitle("新项目");

    // 注意：newProject 不自动创建芯片，这样打开文件时场景是干净的
}

void MainWindow::openFile()
{
    qDebug() << "========== 开始打开文件 ==========";

    // 检查是否已经打开了文件
    if (!m_currentFilePath.isEmpty()) {
        qDebug() << "当前已有文件:" << m_currentFilePath;

        // 创建确认对话框
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("打开文件确认");
        msgBox.setText("当前已经打开了一个设计文件。");
        msgBox.setInformativeText("请选择您希望如何处理当前工作：\n\n"
                                  "• 新建项目：清空当前内容，创建空白项目后再打开新文件\n"
                                  "• 重新打开：直接用新文件替换当前内容\n"
                                  "• 取消：保持当前状态，不打开新文件");
        msgBox.setIcon(QMessageBox::Question);

        // 添加自定义按钮
        QPushButton *newButton = msgBox.addButton("新建项目", QMessageBox::ActionRole);
        QPushButton *reopenButton = msgBox.addButton("重新打开", QMessageBox::ActionRole);
        QPushButton *cancelButton = msgBox.addButton("取消", QMessageBox::RejectRole);

        msgBox.setDefaultButton(reopenButton);

        msgBox.exec();

        if (msgBox.clickedButton() == cancelButton) {
            qDebug() << "用户取消打开文件";
            return; // 用户选择取消
        } else if (msgBox.clickedButton() == newButton) {
            // 用户选择新建项目，先清理当前场景
            qDebug() << "用户选择新建项目，先清理场景";
            newProject();
        }
        // 如果选择重新打开，继续执行打开文件的逻辑
    }

    QString filePath = QFileDialog::getOpenFileName(this, "打开文件", "", "设计文件 (*.place *.txt);;Place文件 (*.place);;文本文件 (*.txt);;所有文件 (*)");
    if (filePath.isEmpty()) {
        qDebug() << "用户未选择文件";
        return;
    }

    qDebug() << "选择的文件:" << filePath;
    qDebug() << "当前画布单位:" << m_scene->get_unit();
    qDebug() << "打开前场景中芯片数量:" << m_scene->getAllCellItems().size();

    if (m_fileManager->openDesignFile(filePath, m_scene)) {
        qDebug() << "文件加载成功！";
        m_currentFilePath = filePath;
        m_isModified = false;
        updateWindowTitle(QFileInfo(filePath).fileName());

        qDebug() << "打开后场景中芯片数量:" << m_scene->getAllCellItems().size();
        qDebug() << "场景矩形:" << m_scene->sceneRect();

        // 使用QTimer延迟调整视图，确保所有芯片都已渲染完成
        QTimer::singleShot(100, this, &MainWindow::fitViewToContent);
    } else {
        qDebug() << "文件加载失败:" << m_fileManager->getLastError();
        QMessageBox::warning(this, "错误", "无法打开文件：" + m_fileManager->getLastError());
    }

    qDebug() << "========== 打开文件完成 ==========";
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
    QString filePath = QFileDialog::getSaveFileName(this, "另存为", "", "设计文件 (*.place *.txt);;Place文件 (*.place);;文本文件 (*.txt);;所有文件 (*)");
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
    QString filePath = QFileDialog::getSaveFileName(this, "导出宏文件", "", "宏文件 (*.macro);;Place文件 (*.place);;文本文件 (*.txt);;所有文件 (*)");
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
    // 获取视图中心在场景坐标系中的位置
    QPointF viewCenter = m_view->rect().center();
    QPointF scenePos = m_view->mapToScene(viewCenter.toPoint());

    qDebug() << "视图中心:" << viewCenter << "对应场景坐标:" << scenePos;

    // 使用命令模式支持撤销/重做，传入ChipManager而不是计数器
    m_undoStack->push(new AddRectangleCommand(m_scene, scenePos, m_chipManager, m_undoStack));

    m_isModified = true;
    updateWindowTitle();

    qDebug() << "添加芯片到视图中心位置" << scenePos;
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

    int cellCount = 0;
    int lineCount = 0;

    // 统计选中的项目类型
    for (QGraphicsItem* item : selectedItems) {
        if (dynamic_cast<CellItem*>(item)) {
            cellCount++;
        } else if (dynamic_cast<ConnectionLine*>(item)) {
            lineCount++;
        }
    }

    // 显示确认对话框
    QString message;
    if (cellCount > 0 && lineCount > 0) {
        message = QString("确定要删除 %1 个芯片和 %2 条连线吗？").arg(cellCount).arg(lineCount);
    } else if (cellCount > 0) {
        message = QString("确定要删除 %1 个芯片吗？").arg(cellCount);
    } else if (lineCount > 0) {
        message = QString("确定要删除 %1 条连线吗？").arg(lineCount);
    } else {
        QMessageBox::information(this, "提示", "没有可删除的项目");
        return;
    }

    int ret = QMessageBox::question(this, "确认删除", message,
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return;
    }

    // 执行删除操作
    for (QGraphicsItem* item : selectedItems) {
        if (CellItem* cellItem = dynamic_cast<CellItem*>(item)) {
            // 删除芯片（使用撤销命令）
            m_undoStack->push(new DeleteRectangleCommand(m_scene, cellItem, m_undoStack));
        } else if (ConnectionLine* line = dynamic_cast<ConnectionLine*>(item)) {
            // 删除连线（直接删除，也可以考虑添加撤销命令）
            m_scene->removeConnectionLine(line);
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

void MainWindow::fitViewToContent()
{
    if (!m_scene || !m_view) return;

    // 优先使用场景矩形（DieSize定义的区域）
    QRectF sceneRect = m_scene->sceneRect();
    if (!sceneRect.isNull() && sceneRect.width() > 0 && sceneRect.height() > 0) {
        qDebug() << "使用场景矩形适配视图:" << sceneRect;

        // 计算合适的缩放，让芯片有足够的显示空间
        // 获取所有芯片，分析它们的平均大小
        QList<CellItem*> cellItems = m_scene->getAllCellItems();
        qreal averageChipSize = 10.0; // 默认芯片大小

        if (!cellItems.isEmpty()) {
            qreal totalSize = 0;
            for (CellItem* item : cellItems) {
                QSizeF chipSize = item->size();
                totalSize += (chipSize.width() + chipSize.height()) / 2.0;
            }
            averageChipSize = totalSize / cellItems.size();
        }

        // 根据芯片平均大小计算合适的缩放
        // 目标：让平均芯片在屏幕上显示为50-100像素
        qreal targetChipPixelSize = 75.0; // 目标像素大小
        qreal optimalScale = targetChipPixelSize / averageChipSize;

        // 限制缩放范围
        const qreal minScale = 2.0;   // 最小缩放
        const qreal maxScale = 20.0;  // 最大缩放
        optimalScale = qBound(minScale, optimalScale, maxScale);

        // 为场景矩形添加适量边距
        qreal marginX = sceneRect.width() * 0.1;
        qreal marginY = sceneRect.height() * 0.1;
        QRectF viewRect = sceneRect.adjusted(-marginX, -marginY, marginX, marginY);

        // 先适应场景
        m_view->fitInView(viewRect, Qt::KeepAspectRatio);

        // 然后应用优化的缩放
        qreal currentScale = m_view->transform().m11();
        qreal adjustFactor = optimalScale / currentScale;
        m_view->scale(adjustFactor, adjustFactor);

        qDebug() << "场景矩形:" << sceneRect;
        qDebug() << "视图矩形:" << viewRect;
        qDebug() << "平均芯片大小:" << averageChipSize;
        qDebug() << "优化缩放因子:" << optimalScale;
        qDebug() << "最终缩放因子:" << m_view->transform().m11();

        // 缩放后更新所有连线的线宽
        if (CanvasView* canvasView = qobject_cast<CanvasView*>(m_view)) {
            QTimer::singleShot(10, [canvasView]() {
                QList<QGraphicsItem*> items = canvasView->scene()->items();
                for (QGraphicsItem* item : items) {
                    ConnectionLine* connectionLine = dynamic_cast<ConnectionLine*>(item);
                    if (connectionLine) {
                        connectionLine->updateLineWidth();
                    }
                }
            });
        }
        return;
    }

    // 如果没有有效的场景矩形，则根据芯片内容调整
    QList<CellItem*> cellItems = m_scene->getAllCellItems();
    if (cellItems.isEmpty()) {
        qDebug() << "没有内容可以适配";
        return;
    }

    // 计算所有芯片的包围盒
    QRectF contentRect;
    for (CellItem* item : cellItems) {
        if (item) {
            QRectF itemRect = QRectF(item->pos(), item->size());
            if (contentRect.isNull()) {
                contentRect = itemRect;
            } else {
                contentRect = contentRect.united(itemRect);
            }
        }
    }

    if (!contentRect.isNull()) {
        // 添加足够的边距以便芯片移动
        qreal marginX = qMax(contentRect.width() * 0.3, 50.0); // 至少50像素边距
        qreal marginY = qMax(contentRect.height() * 0.3, 50.0);
        contentRect.adjust(-marginX, -marginY, marginX, marginY);

        // 让视图适应内容
        m_view->fitInView(contentRect, Qt::KeepAspectRatio);

        qDebug() << "已根据芯片内容调整视图";
        qDebug() << "内容矩形:" << contentRect;
        qDebug() << "最终缩放因子:" << m_view->transform().m11();

        // 缩放后更新所有连线的线宽
        if (CanvasView* canvasView = qobject_cast<CanvasView*>(m_view)) {
            QTimer::singleShot(10, [canvasView]() {
                QList<QGraphicsItem*> items = canvasView->scene()->items();
                for (QGraphicsItem* item : items) {
                    ConnectionLine* connectionLine = dynamic_cast<ConnectionLine*>(item);
                    if (connectionLine) {
                        connectionLine->updateLineWidth();
                    }
                }
            });
        }
    }
}

//将背景画布网格进行调整
void MainWindow::on_actionmm_triggered()
{
    m_view->setGridSize(10);
    m_scene->set_unit("mm");
}


void MainWindow::on_actioncm_triggered()
{
    m_view->setGridSize(100);
    m_scene->set_unit("cm");
}


void MainWindow::on_actiondm_triggered()
{
    m_view->setGridSize(1000);
    m_scene->set_unit("dm");
}
