#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvasscene.h"
#include "canvasview.h"
#include "cellitem.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTextStream>
#include "command.h" // 新增包含

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new CanvasScene(this);
    scene->setSceneRect(0, 0, 800, 600);
    view = new CanvasView(scene);
    view->setParent(this);
    setCentralWidget(view);
    undoStack = new QUndoStack(this); // 初始化撤销栈

    // 连接菜单动作
    connect(ui->actionnew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionopen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionsave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionsave, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(ui->actionexit, &QAction::triggered, this, &MainWindow::exportFiles);
    connect(ui->actionexit, &QAction::triggered, this, &MainWindow::close);

    // 连接五个按钮的信号
    connect(ui->addRectangleButton, &QPushButton::clicked, this, &MainWindow::on_addRectangleButton_clicked);
    connect(ui->selectButton, &QPushButton::clicked, this, &MainWindow::on_selectButton_clicked);
    connect(ui->undoButton, &QPushButton::clicked, this, &MainWindow::on_undoButton_clicked);
    connect(ui->redoButton, &QPushButton::clicked, this, &MainWindow::on_redoButton_clicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::on_deleteButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newFile()
{
    // 清空场景
    scene->clear();
    currentFilePath.clear();
    setWindowTitle("芯片设计 - 新文件");
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "打开文件", "", "设计文件 (*.json);;所有文件 (*)");
    if (filePath.isEmpty()) return;

    // 实现文件加载逻辑
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件：" + filePath);
        return;
    }

    // 清空当前场景
    scene->clear();

    // 读取文件内容并解析
    // 这里需要根据实际文件格式实现解析逻辑
    // ...

    currentFilePath = filePath;
    setWindowTitle("芯片设计 - " + QFileInfo(filePath).fileName());
}

void MainWindow::saveFile()
{
    if (currentFilePath.isEmpty()) {
        saveFileAs();
        return;
    }

    // 实现保存逻辑
    QFile file(currentFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法保存文件：" + currentFilePath);
        return;
    }

    // 将场景内容保存到文件
    // 这里需要根据实际需求实现保存逻辑
    // ...

    QMessageBox::information(this, "提示", "文件已保存");
}

void MainWindow::saveFileAs()
{
    QString filePath = QFileDialog::getSaveFileName(this, "保存文件", "", "设计文件 (*.json);;所有文件 (*)");
    if (filePath.isEmpty()) return;

    currentFilePath = filePath;
    saveFile();
}

void MainWindow::exportFiles()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "选择导出目录", "");
    if (dirPath.isEmpty()) return;

    // 生成两个文本文件
    QString macroFilePath = dirPath + "/macro_file.txt";
    QString designFilePath = dirPath + "/design_file.txt";

    // 收集场景中的所有芯片和引脚信息
    QList<CellItem*> cellItems;
    for (QGraphicsItem* item : scene->items()) {
        if (CellItem* cellItem = dynamic_cast<CellItem*>(item)) {
            cellItems.append(cellItem);
        }
    }

    // 生成宏文件
    if (!generateMacroFile(macroFilePath, cellItems)) {
        QMessageBox::warning(this, "错误", "无法生成宏文件");
        return;
    }

    // 生成设计文件
    if (!generateDesignFile(designFilePath, cellItems)) {
        QMessageBox::warning(this, "错误", "无法生成设计文件");
        return;
    }

    QMessageBox::information(this, "提示", "文件已导出到：\n" + macroFilePath + "\n" + designFilePath);
}

bool MainWindow::generateMacroFile(const QString& filePath, const QList<CellItem*>& cellItems)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // 写入布局区域信息
    QRectF sceneRect = scene->sceneRect();
    out << "DieSize " << sceneRect.left() << " " << sceneRect.top() << " "
        << sceneRect.right() << " " << sceneRect.bottom() << "\n\n";

    // 写入芯粒库芯粒信息
    QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>> macroTypes;

    // 收集所有不同类型的芯粒
    int macroCounter = 1;
    QMap<CellItem*, QString> cellToMacroMap;

    for (CellItem* cellItem : cellItems) {
        // 为每个CellItem分配一个唯一的宏名称
        QString macroName = "MC" + QString::number(macroCounter++);
        cellToMacroMap[cellItem] = macroName;

        QSizeF size = cellItem->size();
        QList<CellItem::Connector> connectors = cellItem->getConnectors();

        // 添加到宏类型映射
        macroTypes.insert(macroName, qMakePair(size, connectors));
    }

    // 写入芯粒库信息
    out << "NumMacros " << macroTypes.size() << "\n";
    for (auto it = macroTypes.begin(); it != macroTypes.end(); ++it) {
        QString macroName = it.key();
        QSizeF size = it.value().first;
        QList<CellItem::Connector> connectors = it.value().second;

        out << "Macro " << macroName << " " << size.width() << " " << size.height() << " " << connectors.size() << "\n";

        // 写入引脚信息
        for (int i = 0; i < connectors.size(); ++i) {
            const CellItem::Connector& conn = connectors[i];
            QString pinName = "P" + QString::number(i + 1);
            out << "Pin " << pinName << " " << conn.x << " " << conn.y << " " << 1 << " " << 1 << "\n";
        }
    }

    file.close();
    return true;
}

bool MainWindow::generateDesignFile(const QString& filePath, const QList<CellItem*>& cellItems)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);

    // 写入布局区域信息
    QRectF sceneRect = scene->sceneRect();
    out << "DieSize " << sceneRect.left() << " " << sceneRect.top() << " "
        << sceneRect.right() << " " << sceneRect.bottom() << "\n\n";

    // 写入芯粒库芯粒信息 (与宏文件相同)
    QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>> macroTypes;

    // 收集所有不同类型的芯粒
    int macroCounter = 1;
    QMap<CellItem*, QString> cellToMacroMap;

    for (CellItem* cellItem : cellItems) {
        // 为每个CellItem分配一个唯一的宏名称
        QString macroName = "MC" + QString::number(macroCounter++);
        cellToMacroMap[cellItem] = macroName;

        QSizeF size = cellItem->size();
        QList<CellItem::Connector> connectors = cellItem->getConnectors();

        // 添加到宏类型映射
        macroTypes.insert(macroName, qMakePair(size, connectors));
    }

    // 写入芯粒库信息
    out << "NumMacros " << macroTypes.size() << "\n";
    for (auto it = macroTypes.begin(); it != macroTypes.end(); ++it) {
        QString macroName = it.key();
        QSizeF size = it.value().first;
        QList<CellItem::Connector> connectors = it.value().second;

        out << "Macro " << macroName << " " << size.width() << " " << size.height() << " " << connectors.size() << "\n";

        // 写入引脚信息
        for (int i = 0; i < connectors.size(); ++i) {
            const CellItem::Connector& conn = connectors[i];
            QString pinName = "P" + QString::number(i + 1);
            out << "Pin " << pinName << " " << conn.x << " " << conn.y << " " << 1 << " " << 1 << "\n";
        }
    }

    // 写入芯粒实例信息
    out << "\nNumInstances " << cellItems.size() << "\n";
    for (int i = 0; i < cellItems.size(); ++i) {
        CellItem* cellItem = cellItems[i];
        QString instanceName = "C" + QString::number(i + 1);
        QString macroName = cellToMacroMap[cellItem];
        QPointF pos = cellItem->pos();

        out << "Inst " << instanceName << " " << macroName << " " << pos.x() << " " << pos.y() << "\n";
    }

    // 写入线网信息
    // 这部分需要根据实际的连线信息来实现
    // 由于当前代码中没有完整的连线信息收集，这里只是一个示例框架
    out << "\nNumNets 0\n";
    // 实际应该遍历所有连线，并按照格式输出

    file.close();
    return true;
}

void MainWindow::on_addRectangleButton_clicked()
{
    // 在场景中心添加矩形
    QPointF pos(400, 300); // 默认位置（场景中心）
    undoStack->push(new AddRectangleCommand(scene, pos, undoStack));
    qDebug() << "Added rectangle at" << pos;
}

void MainWindow::on_selectButton_clicked()
{
    // 遍历场景中的项，选择第一个未选中的 CellItem
    for (QGraphicsItem* item : scene->items()) {
        if (CellItem* cellItem = dynamic_cast<CellItem*>(item)) {
            if (!cellItem->isSelected()) {
                scene->clearSelection();
                cellItem->setSelected(true);
                qDebug() << "Selected CellItem at" << cellItem->pos();
                return;
            }
        }
    }
    QMessageBox::information(this, "提示", "没有可选择的矩形");
}

void MainWindow::on_undoButton_clicked()
{
    if (undoStack->canUndo()) {
        undoStack->undo();
        qDebug() << "Undo performed";
    } else {
        QMessageBox::information(this, "提示", "没有可撤销的操作");
    }
}

void MainWindow::on_redoButton_clicked()
{
    if (undoStack->canRedo()) {
        undoStack->redo();
        qDebug() << "Redo performed";
    } else {
        QMessageBox::information(this, "提示", "没有可重做的操作");
    }
}

void MainWindow::on_deleteButton_clicked()
{
    // 删除所有选中的 CellItem
    QList<QGraphicsItem*> selectedItems = scene->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, "错误", "请先选择一个矩形");
        return;
    }

    for (QGraphicsItem* item : selectedItems) {
        if (CellItem* cellItem = dynamic_cast<CellItem*>(item)) {
            undoStack->push(new DeleteRectangleCommand(scene, cellItem, undoStack));
            qDebug() << "Deleted CellItem at" << cellItem->pos();
        }
    }
}
