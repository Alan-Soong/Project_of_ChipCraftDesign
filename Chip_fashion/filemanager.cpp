#include "filemanager.h"
#include "cellitem_new.h"
#include "canvasscene.h"
#include "connectionline.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>

FileManager::FileManager(QObject *parent)
    : QObject(parent)
    , m_chipCounter(0)
{
}

FileManager::~FileManager()
{
}

bool FileManager::openDesignFile(const QString& filePath, CanvasScene* scene)
{
    if (!scene) {
        setError("Invalid scene pointer");
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError("Cannot open file: " + filePath);
        return false;
    }

    clearError();
    scene->clear();

    QTextStream in(&file);
    QString line;
    QMap<QString, CellItem*> cellMap;
    QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>> macroTypes;
    QMap<QString, QString> instanceNameMapping;

    m_chipCounter = 0;
    bool parseError = false;

    emit progressUpdate(10);

    while (!in.atEnd() && !parseError) {
        line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.isEmpty()) continue;

        if (parts[0] == "DieSize") {
            parseError = !parseDieSize(line, scene);
        } else if (parts[0] == "NumMacros") {
            // 跳过数量行，直接解析宏定义
            continue;
        } else if (parts[0] == "Macro") {
            parseError = !parseMacroDefinition(line, in, macroTypes);
        } else if (parts[0] == "NumInstances") {
            // 跳过数量行
            continue;
        } else if (parts[0] == "Inst") {
            parseError = !parseInstance(line, scene, macroTypes, cellMap, instanceNameMapping);
        } else if (parts[0] == "NumNets") {
            // 跳过数量行
            continue;
        } else if (parts[0] == "Net") {
            parseError = !parseNet(line, in, scene, cellMap, instanceNameMapping);
        }
    }

    emit progressUpdate(100);

    if (parseError) {
        setError("Parse error occurred while reading file");
        return false;
    }

    scene->update();
    emit fileOperationCompleted("open", true);
    return true;
}

bool FileManager::saveDesignFile(const QString& filePath, const QList<CellItem*>& cellItems, CanvasScene* scene)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError("Cannot write to file: " + filePath);
        return false;
    }

    QTextStream out(&file);

    // 写入DieSize
    if (scene) {
        QRectF sceneRect = scene->sceneRect();
        out << "DieSize " << sceneRect.left() << " " << sceneRect.top() << " "
            << sceneRect.right() << " " << sceneRect.bottom() << "\n\n";
    }

    emit progressUpdate(25);

    // 生成宏定义
    if (!generateMacroDefinitions(out, cellItems)) {
        return false;
    }

    emit progressUpdate(50);

    // 生成实例
    if (!generateInstances(out, cellItems)) {
        return false;
    }

    emit progressUpdate(75);

    // 生成网络连接
    if (!generateNets(out, cellItems)) {
        return false;
    }

    emit progressUpdate(100);

    file.close();
    emit fileOperationCompleted("save", true);
    return true;
}

bool FileManager::exportMacroFile(const QString& filePath, const QList<CellItem*>& cellItems)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError("Cannot write to macro file: " + filePath);
        return false;
    }

    QTextStream out(&file);

    for (const CellItem* cellItem : cellItems) {
        QString macroName = cellItem->getMacroName();
        if (macroName.isEmpty()) continue;

        QSizeF size = cellItem->size();
        QList<CellItem::Connector> connectors = cellItem->getConnectors();

        out << "Macro: " << macroName << "\n";
        out << "Size: " << size.width() << " x " << size.height() << "\n";
        out << "Pins: " << connectors.size() << "\n";

        for (const auto& conn : connectors) {
            out << "  Pin " << conn.id << " " << conn.x << " " << conn.y
                << " " << conn.side << " " << conn.percentage << "\n";
        }
        out << "\n";
    }

    file.close();
    emit fileOperationCompleted("export", true);
    return true;
}

bool FileManager::saveComponentToFile(const QString& filePath, const CellItem* cellItem)
{
    if (!cellItem) {
        setError("Invalid cell item");
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError("Cannot write to component file: " + filePath);
        return false;
    }

    QTextStream out(&file);
    QSizeF size = cellItem->size();
    QList<CellItem::Connector> connectors = cellItem->getConnectors();

    out << "name: " << cellItem->getMacroName() << "\n";
    out << "width: " << size.width() << "\n";
    out << "height: " << size.height() << "\n";
    out << "pins:\n";

    for (const auto& conn : connectors) {
        out << "  " << conn.id << " " << conn.x << " " << conn.y
            << " " << conn.side << " " << conn.percentage << "\n";
    }

    file.close();
    return true;
}

bool FileManager::loadComponentFromFile(const QString& filePath, CellItem* cellItem)
{
    if (!cellItem) {
        setError("Invalid cell item");
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError("Cannot read component file: " + filePath);
        return false;
    }

    QTextStream in(&file);
    QString line;
    QString name;
    int width = 0, height = 0;
    QList<CellItem::Connector> connectors;

    while (!in.atEnd()) {
        line = in.readLine().trimmed();
        if (line.startsWith("name: ")) {
            name = line.mid(6);
        } else if (line.startsWith("width: ")) {
            width = line.mid(7).toInt();
        } else if (line.startsWith("height: ")) {
            height = line.mid(8).toInt();
        } else if (line.startsWith("pins:")) {
            // 读取引脚信息
            while (!in.atEnd()) {
                line = in.readLine().trimmed();
                if (line.isEmpty()) break;

                QStringList parts = line.split(" ", Qt::SkipEmptyParts);
                if (parts.size() >= 5) {
                    CellItem::Connector conn;
                    conn.id = parts[0];
                    conn.x = static_cast<qreal>(parts[1].toDouble());
                    conn.y = static_cast<qreal>(parts[2].toDouble());
                    conn.side = parts[3];
                    conn.percentage = static_cast<qreal>(parts[4].toDouble());
                    connectors.append(conn);
                }
            }
        }
    }

    // 应用到CellItem
    cellItem->setMacroName(name);
    cellItem->setSize(QSizeF(width, height));

    // 清除现有连接器并添加新的
    // 这里需要CellItem提供清除连接器的方法
    for (const auto& conn : connectors) {
        cellItem->addConnector(conn.side, static_cast<qreal>(conn.percentage), static_cast<qreal>(1), conn.id, static_cast<qreal>(conn.x), static_cast<qreal>(conn.y));
    }

    file.close();
    return true;
}

bool FileManager::parseDieSize(const QString& line, CanvasScene* scene)
{
    QStringList parts = line.split(" ", Qt::SkipEmptyParts);
    if (parts.size() < 5) return false;

    bool ok;
    double left = parts[1].toDouble(&ok);
    if (!ok) return false;
    double bottom = parts[2].toDouble(&ok);  // 左下角Y坐标
    if (!ok) return false;
    double right = parts[3].toDouble(&ok);
    if (!ok) return false;
    double top = parts[4].toDouble(&ok);     // 右上角Y坐标
    if (!ok) return false;

    // 计算宽度和高度
    double width = right - left;
    double height = top - bottom;

    // 设置场景矩形：Qt坐标系中Y轴向下为正，传统CAD坐标系Y轴向上为正
    // 我们保持原始坐标系，让场景矩形从(0,0)开始，大小为width x height
    scene->setSceneRect(0, 0, width, height);

    qDebug() << "设置场景矩形: 位置(0, 0) 大小:" << width << "x" << height;
    qDebug() << "原始边界: left=" << left << ", bottom=" << bottom << ", right=" << right << ", top=" << top;
    return true;
}

bool FileManager::parseMacroDefinition(const QString& line, QTextStream& stream,
                                       QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>>& macroTypes)
{
    QStringList parts = line.split(" ", Qt::SkipEmptyParts);
    if (parts.size() < 5) return false;

    QString macroName = parts[1];
    bool ok;
    double width = parts[2].toDouble(&ok);
    if (!ok || width <= 0) return false;
    double height = parts[3].toDouble(&ok);
    if (!ok || height <= 0) return false;
    int pinCount = parts[4].toInt(&ok);
    if (!ok || pinCount < 0) return false;

    QList<CellItem::Connector> connectors;
    for (int i = 0; i < pinCount && !stream.atEnd(); ++i) {
        QString pinLine = stream.readLine().trimmed();
        QStringList pinParts = pinLine.split(" ", Qt::SkipEmptyParts);
        if (pinParts.size() < 6 || pinParts[0] != "Pin") {
            qDebug() << "引脚解析失败: 期望格式 'Pin id x y width height'，实际:" << pinLine;
            qDebug() << "分割后的部分数量:" << pinParts.size() << "内容:" << pinParts;
            return false;
        }

        CellItem::Connector conn;
        conn.id = pinParts[1];
        qreal pinX = static_cast<qreal>(pinParts[2].toDouble(&ok));
        if (!ok) {
            qDebug() << "无法解析引脚X坐标:" << pinParts[2];
            return false;
        }
        qreal pinY = static_cast<qreal>(pinParts[3].toDouble(&ok));
        if (!ok) {
            qDebug() << "无法解析引脚Y坐标:" << pinParts[3];
            return false;
        }

        // 坐标转换：从左下角坐标系转换为Qt的左上角坐标系
        // 文件中的Y坐标是相对于左下角的，需要转换为相对于左上角的
        conn.x = pinX;
        conn.y = height - pinY; // Y坐标需要反转

        // 根据引脚位置智能推断边缘和百分比
        QString side;
        qreal percentage;

        // 容错值，用于判断引脚是否在边缘
        qreal tolerance = 0.5;

        if (qAbs(conn.x) <= tolerance) {
            // 左边缘
            side = "left";
            percentage = (conn.y / height) * 100.0;
        } else if (qAbs(conn.x - width) <= tolerance) {
            // 右边缘
            side = "right";
            percentage = (conn.y / height) * 100.0;
        } else if (qAbs(conn.y) <= tolerance) {
            // 顶边缘（注意：文件中Y=0可能是底部，需要根据坐标系调整）
            side = "top";
            percentage = (conn.x / width) * 100.0;
        } else if (qAbs(conn.y - height) <= tolerance) {
            // 底边缘
            side = "bottom";
            percentage = (conn.x / width) * 100.0;
        } else {
            // 不在边缘，使用自定义位置
            side = "custom";
            percentage = 0.0;
        }

        conn.side = side;
        conn.percentage = percentage;

        connectors.append(conn);
        qDebug() << "成功解析引脚:" << conn.id << "原始位置(左下角):" << pinX << "," << pinY
                 << "转换后位置(左上角):" << conn.x << "," << conn.y
                 << "推断边缘:" << side << "百分比:" << percentage;
    }

    macroTypes[macroName] = qMakePair(QSizeF(width, height), connectors);
    return true;
}

bool FileManager::parseInstance(const QString& line, CanvasScene* scene,
                                const QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>>& macroTypes,
                                QMap<QString, CellItem*>& cellMap,
                                QMap<QString, QString>& instanceNameMapping)
{
    QStringList parts = line.split(" ", Qt::SkipEmptyParts);
    if (parts.size() < 5) return false;

    QString originalInstanceName = parts[1];
    QString macroName = parts[2];
    bool ok;
    double posX = parts[3].toDouble(&ok);
    if (!ok) return false;
    double posY = parts[4].toDouble(&ok);
    if (!ok) return false;

    if (!macroTypes.contains(macroName)) {
        qWarning() << "Macro" << macroName << "not found for instance" << originalInstanceName;
        return false;
    }

    auto macroInfo = macroTypes[macroName];

    // 需要获取场景的高度来进行Y坐标转换
    QRectF sceneRect = scene->sceneRect();
    double sceneHeight = sceneRect.height();

    // 获取芯片高度
    double chipHeight = macroInfo.first.height();

    // 坐标转换：从左下角坐标系转换为Qt的左上角坐标系
    // 文件中的位置是芯片左下角的位置，Qt中需要芯片左上角的位置
    double qtPosX = posX;
    double qtPosY = sceneHeight - posY - chipHeight; // Y坐标需要反转，并考虑芯片高度

    m_chipCounter++;
    QString instanceName = originalInstanceName;
    if (cellMap.contains(instanceName)) {
        instanceName = "C" + QString::number(m_chipCounter);
        instanceNameMapping[originalInstanceName] = instanceName;
    } else {
        instanceNameMapping[originalInstanceName] = originalInstanceName;
    }

    CellItem* cellItem = new CellItem();
    cellItem->setPos(qtPosX, qtPosY);
    cellItem->setSize(macroInfo.first);
    cellItem->setMacroName(macroName);
    cellItem->setInstanceName(instanceName);

    qDebug() << "创建芯片实例:" << instanceName << "宏:" << macroName
             << "原始位置(左下角):" << posX << "," << posY
             << "转换后位置(左上角):" << qtPosX << "," << qtPosY;
    qDebug() << "芯片尺寸:" << macroInfo.first << "引脚数量:" << macroInfo.second.size();

    // 添加连接器
    for (const auto& conn : macroInfo.second) {
        qDebug() << "为芯片" << instanceName << "添加引脚:" << conn.id << "位置:" << conn.x << "," << conn.y;
        cellItem->addConnector(conn.side, static_cast<qreal>(conn.percentage), static_cast<qreal>(1), conn.id,
                               static_cast<qreal>(conn.x), static_cast<qreal>(conn.y));
    }

    scene->addCellItem(cellItem);
    cellMap[instanceName] = cellItem;

    return true;
}

bool FileManager::parseNet(const QString& line, QTextStream& stream, CanvasScene* scene,
                           const QMap<QString, CellItem*>& cellMap,
                           const QMap<QString, QString>& instanceNameMapping)
{
    QStringList parts = line.split(" ", Qt::SkipEmptyParts);
    if (parts.size() < 3) return false;

    QString netName = parts[1];
    int pinCount = parts[2].toInt();
    QList<QPair<QString, QString>> netPins;

    qDebug() << "解析网络:" << netName << "引脚数量:" << pinCount;

    for (int i = 0; i < pinCount && !stream.atEnd(); ++i) {
        QString pinLine = stream.readLine().trimmed();
        QStringList pinParts = pinLine.split(" ", Qt::SkipEmptyParts);
        if (pinParts.size() < 2 || pinParts[0] != "Pin") {
            qDebug() << "网络引脚解析失败:" << pinLine;
            return false;
        }

        // 解析格式：Pin C1/P1
        QString fullPin = pinParts[1];
        QStringList pinSpec = fullPin.split("/");
        if (pinSpec.size() != 2) {
            qDebug() << "引脚格式错误:" << fullPin;
            return false;
        }

        QString instanceName = pinSpec[0];
        if (instanceNameMapping.contains(instanceName)) {
            instanceName = instanceNameMapping[instanceName];
        }
        QString pinName = pinSpec[1];
        netPins.append(qMakePair(instanceName, pinName));
        qDebug() << "网络" << netName << "包含引脚:" << instanceName << "/" << pinName;
    }

    // 创建连线
    if (netPins.size() < 2) {
        qDebug() << "网络" << netName << "引脚数量不足，跳过连线创建";
        return true; // 网络至少需要2个引脚
    }

    for (int i = 0; i < netPins.size() - 1; ++i) {
        QString sourceInstance = netPins[i].first;
        QString sourcePin = netPins[i].second;
        QString targetInstance = netPins[i + 1].first;
        QString targetPin = netPins[i + 1].second;

        if (!cellMap.contains(sourceInstance) || !cellMap.contains(targetInstance)) {
            continue;
        }

        CellItem* sourceCell = cellMap[sourceInstance];
        CellItem* targetCell = cellMap[targetInstance];

        // 查找连接器
        CellItem::Connector sourceConn, targetConn;
        bool sourceFound = false, targetFound = false;

        for (const auto& conn : sourceCell->getConnectors()) {
            if (conn.id == sourcePin) {
                sourceConn = conn;
                sourceFound = true;
                break;
            }
        }

        for (const auto& conn : targetCell->getConnectors()) {
            if (conn.id == targetPin) {
                targetConn = conn;
                targetFound = true;
                break;
            }
        }

        if (sourceFound && targetFound) {
            sourceCell->addConnection(targetCell, sourcePin, targetPin);
            ConnectionLine* line = new ConnectionLine(sourceCell, sourceConn, targetCell, targetConn);
            scene->addConnectionLine(line); // 使用专门的方法添加连线
        }
    }

    return true;
}

bool FileManager::generateMacroDefinitions(QTextStream& stream, const QList<CellItem*>& cellItems)
{
    QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>> macroTypes;

    // 收集所有不同的宏类型
    for (CellItem* cellItem : cellItems) {
        QString macroName = cellItem->getMacroName();
        if (macroName.isEmpty()) {
            macroName = "MC" + QString::number(macroTypes.size() + 1);
            cellItem->setMacroName(macroName);
        }
        macroTypes[macroName] = qMakePair(cellItem->size(), cellItem->getConnectors());
    }

    stream << "NumMacros " << macroTypes.size() << "\n";
    for (auto it = macroTypes.begin(); it != macroTypes.end(); ++it) {
        QString macroName = it.key();
        QSizeF size = it.value().first;
        QList<CellItem::Connector> connectors = it.value().second;

        stream << "Macro " << macroName << " " << size.width() << " " << size.height()
               << " " << connectors.size() << "\n";

        // 写入引脚信息 - 修改为标准格式：Pin id x y width height
        for (const auto& conn : connectors) {
            // 使用固定的引脚宽度和高度（1 1）
            stream << "Pin " << conn.id << " " << conn.x << " " << conn.y
                   << " 1 1" << "\n";
        }
    }
    stream << "\n";

    return true;
}

bool FileManager::generateInstances(QTextStream& stream, const QList<CellItem*>& cellItems)
{
    stream << "NumInstances " << cellItems.size() << "\n";
    for (CellItem* cellItem : cellItems) {
        QString instanceName = cellItem->getInstanceName();
        if (instanceName.isEmpty()) {
            instanceName = "C" + QString::number(cellItems.indexOf(cellItem) + 1);
            cellItem->setInstanceName(instanceName);
        }
        stream << "Inst " << instanceName << " " << cellItem->getMacroName() << " "
               << cellItem->pos().x() << " " << cellItem->pos().y() << "\n";
    }
    stream << "\n";

    return true;
}

bool FileManager::generateNets(QTextStream& stream, const QList<CellItem*>& cellItems)
{
    QMap<QString, QList<QPair<QString, QString>>> netMap;
    int netCounter = 1;

    for (CellItem* cellItem : cellItems) {
        QString sourceInstance = cellItem->getInstanceName();
        auto connections = cellItem->getConnections();
        for (const auto& conn : connections) {
            QString targetInstance = conn.first->getInstanceName();
            QString sourcePin = conn.second.first;
            QString targetPin = conn.second.second;

            QString netName = "N" + QString::number(netCounter++);
            netMap[netName].append(qMakePair(sourceInstance, sourcePin));
            netMap[netName].append(qMakePair(targetInstance, targetPin));
        }
    }

    stream << "NumNets " << netMap.size() << "\n";
    for (auto it = netMap.begin(); it != netMap.end(); ++it) {
        QString netName = it.key();
        const auto& pins = it.value();
        stream << "Net " << netName << " " << pins.size() << "\n";
        for (const auto& pin : pins) {
            stream << "Pin " << pin.first << "/" << pin.second << "\n";
        }
    }

    return true;
}

void FileManager::setError(const QString& error)
{
    m_lastError = error;
    qWarning() << "FileManager error:" << error;
}
