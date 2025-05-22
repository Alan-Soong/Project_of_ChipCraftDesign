#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvasscene.h"
#include "canvasview.h"
#include "cellitem.h"
#include "command.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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

    
    // 添加一个默认的芯片
    CellItem* defaultCell = new CellItem();
    defaultCell->setPos(100, 100);
    defaultCell->setSize(QSizeF(150, 100));
    defaultCell->setMacroName("MC1");
    defaultCell->setInstanceName("C1");
    scene->addCellItem(defaultCell);
    
    setWindowTitle("芯片设计 - 新文件");
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
    
    // 添加一个默认的芯片
    CellItem* defaultCell = new CellItem();
    defaultCell->setPos(100, 100);
    defaultCell->setSize(QSizeF(150, 100));
    defaultCell->setMacroName("MC1");
    defaultCell->setInstanceName("C1");
    scene->addCellItem(defaultCell);
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
    QByteArray jsonData = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(this, "错误", "文件格式无效");
        return;
    }
    
    QJsonObject root = doc.object();
    
    // 加载芯片
    QJsonArray cellsArray = root["cells"].toArray();
    QMap<QString, CellItem*> cellMap; // 用于记录实例名称到CellItem的映射
    
    for (const QJsonValue& cellValue : cellsArray) {
        QJsonObject cellObj = cellValue.toObject();
        CellItem* cell = new CellItem();
        cell->fromJson(cellObj);
        scene->addCellItem(cell);
        
        // 记录实例名称到CellItem的映射
        cellMap[cell->getInstanceName()] = cell;
    }
    
    // 加载连线
    QJsonArray connectionsArray = root["connections"].toArray();
    for (const QJsonValue& connValue : connectionsArray) {
        QJsonObject connObj = connValue.toObject();
        QString sourceCell = connObj["sourceCell"].toString();
        QString targetCell = connObj["targetCell"].toString();
        QString sourcePin = connObj["sourcePin"].toString();
        QString targetPin = connObj["targetPin"].toString();
        
        if (cellMap.contains(sourceCell) && cellMap.contains(targetCell)) {
            cellMap[sourceCell]->addConnection(cellMap[targetCell], sourcePin, targetPin);
            
            // 在场景中创建连线
            CellItem::Connector sourcePinConnector;
            CellItem::Connector targetPinConnector;
            
            // 查找源引脚
            bool sourceFound = false;
            auto sourceConnectors = cellMap[sourceCell]->getConnectors();
            for (const auto& conn : sourceConnectors) {
                if (conn.id == sourcePin) {
                    sourcePinConnector = conn;
                    sourceFound = true;
                    break;
                }
            }
            
            // 查找目标引脚
            bool targetFound = false;
            auto targetConnectors = cellMap[targetCell]->getConnectors();
            for (const auto& conn : targetConnectors) {
                if (conn.id == targetPin) {
                    targetPinConnector = conn;
                    targetFound = true;
                    break;
                }
            }
            
            if (sourceFound && targetFound) {
                ConnectionLine* line = new ConnectionLine(cellMap[sourceCell], sourcePinConnector, 
                                                         cellMap[targetCell], targetPinConnector);
                scene->addItem(line);
            }
        }
    }

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
    QJsonObject root;
    
    // 保存芯片
    QJsonArray cellsArray;
    QList<CellItem*> cellItems = scene->getAllCellItems();
    for (CellItem* cell : cellItems) {
        cellsArray.append(cell->toJson());
    }
    root["cells"] = cellsArray;
    
    // 保存连线
    QJsonArray connectionsArray;
    for (CellItem* cell : cellItems) {
        auto connections = cell->getConnections();
        for (const auto& conn : connections) {
            QJsonObject connObj;
            connObj["sourceCell"] = cell->getInstanceName();
            connObj["targetCell"] = conn.first->getInstanceName();
            connObj["sourcePin"] = conn.second.first;
            connObj["targetPin"] = conn.second.second;
            connectionsArray.append(connObj);
        }
    }
    root["connections"] = connectionsArray;
    
    QJsonDocument doc(root);
    file.write(doc.toJson());
    file.close();

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
    QList<CellItem*> cellItems = scene->getAllCellItems();

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
    for (CellItem* cellItem : cellItems) {
        QString macroName = cellItem->getMacroName();
        if (macroName.isEmpty()) {
            macroName = "MC" + QString::number(macroTypes.size() + 1);
            cellItem->setMacroName(macroName);
        }
        
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
    for (CellItem* cellItem : cellItems) {
        QString macroName = cellItem->getMacroName();
        if (macroName.isEmpty()) {
            macroName = "MC" + QString::number(macroTypes.size() + 1);
            cellItem->setMacroName(macroName);
        }
        
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
        QString instanceName = cellItem->getInstanceName();
        if (instanceName.isEmpty()) {
            instanceName = "C" + QString::number(i + 1);
            cellItem->setInstanceName(instanceName);
        }
        QString macroName = cellItem->getMacroName();
        QPointF pos = cellItem->pos();
        
        out << "Inst " << instanceName << " " << macroName << " " << pos.x() << " " << pos.y() << "\n";
    }
    
    // 写入线网信息
    QList<QPair<QString, QPair<QString, QString>>> allConnections;
    QMap<QString, QList<QPair<QString, QString>>> netMap; // 网络名称 -> (实例名, 引脚名)
    
    // 收集所有连线
    int netCounter = 1;
    for (CellItem* cellItem : cellItems) {
        QString sourceInstance = cellItem->getInstanceName();
        auto connections = cellItem->getConnections();
        
        for (const auto& conn : connections) {
            QString targetInstance = conn.first->getInstanceName();
            QString sourcePin = conn.second.first;
            QString targetPin = conn.second.second;
            
            // 为每个连线创建一个网络
            QString netName = "N" + QString::number(netCounter++);
            
            // 查找源引脚和目标引脚的索引
            int sourcePinIndex = -1;
            int targetPinIndex = -1;
            
            auto sourceConnectors = cellItem->getConnectors();
            for (int i = 0; i < sourceConnectors.size(); ++i) {
                if (sourceConnectors[i].id == sourcePin) {
                    sourcePinIndex = i + 1; // 引脚索引从1开始
                    break;
                }
            }
            
            auto targetConnectors = conn.first->getConnectors();
            for (int i = 0; i < targetConnectors.size(); ++i) {
                if (targetConnectors[i].id == targetPin) {
                    targetPinIndex = i + 1; // 引脚索引从1开始
                    break;
                }
            }
            
            if (sourcePinIndex > 0 && targetPinIndex > 0) {
                // 添加到网络映射
                netMap[netName].append(qMakePair(sourceInstance, "P" + QString::number(sourcePinIndex)));
                netMap[netName].append(qMakePair(targetInstance, "P" + QString::number(targetPinIndex)));
            }
        }
    }
    
    // 写入线网信息
    out << "\nNumNets " << netMap.size() << "\n";
    for (auto it = netMap.begin(); it != netMap.end(); ++it) {
        QString netName = it.key();
        QList<QPair<QString, QString>> pins = it.value();
        
        out << "Net " << netName << " " << pins.size() << "\n";
        for (const auto& pin : pins) {
            out << "Pin " << pin.first << " " << pin.second << "\n";
        }
    }
    
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
