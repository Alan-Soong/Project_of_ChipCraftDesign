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
        cellItem->addConnector(conn.side, static_cast<qreal>(conn.percentage), static_cast<qreal>(10), conn.id, static_cast<qreal>(conn.x), static_cast<qreal>(conn.y));
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
    double top = parts[2].toDouble(&ok);
    if (!ok) return false;
    double right = parts[3].toDouble(&ok);
    if (!ok) return false;
    double bottom = parts[4].toDouble(&ok);
    if (!ok) return false;

    scene->setSceneRect(left, top, right - left, bottom - top);
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
            return false;
        }

        CellItem::Connector conn;
        conn.id = pinParts[1];
        conn.x = static_cast<qreal>(pinParts[2].toDouble(&ok));
        if (!ok) return false;
        conn.y = static_cast<qreal>(pinParts[3].toDouble(&ok));
        if (!ok) return false;

        // 如果有side和percentage信息，读取它们
        if (pinParts.size() >= 6) {
            conn.side = pinParts[4];
            conn.percentage = static_cast<qreal>(pinParts[5].toDouble(&ok));
            if (!ok) conn.percentage = 0.0;
        } else {
            // 默认值
            conn.side = "custom";
            conn.percentage = 0.0;
        }

        connectors.append(conn);
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

    m_chipCounter++;
    QString instanceName = originalInstanceName;
    if (cellMap.contains(instanceName)) {
        instanceName = "C" + QString::number(m_chipCounter);
        instanceNameMapping[originalInstanceName] = instanceName;
    } else {
        instanceNameMapping[originalInstanceName] = originalInstanceName;
    }

    auto macroInfo = macroTypes[macroName];
    CellItem* cellItem = new CellItem();
    cellItem->setPos(posX, posY);
    cellItem->setSize(macroInfo.first);
    cellItem->setMacroName(macroName);
    cellItem->setInstanceName(instanceName);

    // 添加连接器
    for (const auto& conn : macroInfo.second) {
        cellItem->addConnector(conn.side, static_cast<qreal>(conn.percentage), static_cast<qreal>(10), conn.id, static_cast<qreal>(conn.x), static_cast<qreal>(conn.y));
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

    for (int i = 0; i < pinCount && !stream.atEnd(); ++i) {
        QString pinLine = stream.readLine().trimmed();
        QStringList pinParts = pinLine.split(" ", Qt::SkipEmptyParts);
        if (pinParts.size() < 3 || pinParts[0] != "Pin") {
            return false;
        }

        QString instanceName = pinParts[1];
        if (instanceNameMapping.contains(instanceName)) {
            instanceName = instanceNameMapping[instanceName];
        }
        QString pinName = pinParts[2];
        netPins.append(qMakePair(instanceName, pinName));
    }

    // 创建连线
    if (netPins.size() < 2) return true; // 网络至少需要2个引脚

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
            scene->addItem(line);
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

        // 写入引脚信息 - 与原始格式保持一致：Pin id x y side percentage
        for (const auto& conn : connectors) {
            stream << "Pin " << conn.id << " " << conn.x << " " << conn.y
                   << " " << conn.side << " " << conn.percentage << "\n";
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
            stream << "Pin " << pin.first << " " << pin.second << "\n";
        }
    }

    return true;
}

void FileManager::setError(const QString& error)
{
    m_lastError = error;
    qWarning() << "FileManager error:" << error;
}
