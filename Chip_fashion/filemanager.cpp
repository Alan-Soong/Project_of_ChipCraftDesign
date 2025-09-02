#include "filemanager.h"
#include "cellitem_new.h"
#include "canvasscene.h"
#include "connectionline.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>
#include <QSet>

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

    qDebug() << "========== 开始加载文件 ==========" << filePath;
    clearError();
    scene->clear();
    // 清空旧的命名线网缓存并标记加载中，避免交互逻辑干扰
    scene->resetNamedNets();
    scene->setLoading(true);

    QTextStream in(&file);
    // Qt6 中不再需要手动设置编码，默认使用 UTF-8
    QString line;
    QMap<QString, CellItem*> cellMap;
    QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>> macroTypes;
    QMap<QString, QString> instanceNameMapping;

    m_chipCounter = 0;
    bool parseError = false;
    int lineNumber = 0;
    int diesizeCount = 0, macroCount = 0, instCount = 0, netCount = 0;

    emit progressUpdate(10);

    while (!in.atEnd() && !parseError) {
        line = in.readLine().trimmed();
        lineNumber++;

        if (line.isEmpty()) continue;

        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
        if (parts.isEmpty()) continue;

        // qDebug() << "Line" << lineNumber << ":" << line;

        if (parts[0] == "DieSize") {
            diesizeCount++;
            qDebug() << "解析DieSize，第" << diesizeCount << "个";
            parseError = !parseDieSize(line, scene);
            if (parseError) {
                qDebug() << "DieSize解析失败！";
            } else {
                qDebug() << "DieSize解析成功，场景矩形:" << scene->sceneRect();
            }
        } else if (parts[0] == "NumMacros") {
            // qDebug() << "跳过NumMacros行:" << line;
            // 跳过数量行，直接解析宏定义
            continue;
        } else if (parts[0] == "Macro") {
            macroCount++;
            qDebug() << "解析Macro，第" << macroCount << "个:" << parts[1];
            parseError = !parseMacroDefinition(line, in, macroTypes, scene);
            if (parseError) {
                qDebug() << "Macro解析失败！";
            } else {
                qDebug() << "Macro解析成功，当前宏类型数量:" << macroTypes.size();
            }
        } else if (parts[0] == "NumInstances") {
            // qDebug() << "跳过NumInstances行:" << line;
            // 跳过数量行
            continue;
        } else if (parts[0] == "Inst") {
            instCount++;
            qDebug() << "解析Instance，第" << instCount << "个:" << parts[1];
            parseError = !parseInstance(line, scene, macroTypes, cellMap, instanceNameMapping);
            if (parseError) {
                qDebug() << "Instance解析失败！";
            } else {
                qDebug() << "Instance解析成功，当前芯片数量:" << cellMap.size();
                qDebug() << "场景中的芯片数量:" << scene->getAllCellItems().size();
            }
        } else if (parts[0] == "NumNets") {
            // qDebug() << "跳过NumNets行:" << line;
            // 跳过数量行
            continue;
        } else if (parts[0] == "Net") {
            netCount++;
            qDebug() << "解析Net，第" << netCount << "个:" << parts[1];
            parseError = !parseNet(line, in, scene, cellMap, instanceNameMapping);
            if (parseError) {
                qDebug() << "Net解析失败！";
            } else {
                qDebug() << "Net解析成功";
            }
        } else {
            // qDebug() << "跳过未知行:" << line;
        }
    }

    qDebug() << "========== 文件解析完成 ==========";
    qDebug() << "DieSize数量:" << diesizeCount;
    qDebug() << "Macro数量:" << macroCount << "，宏类型Map大小:" << macroTypes.size();
    qDebug() << "Instance数量:" << instCount << "，芯片Map大小:" << cellMap.size();
    qDebug() << "Net数量:" << netCount;
    qDebug() << "场景中实际芯片数量:" << scene->getAllCellItems().size();
    qDebug() << "最终场景矩形:" << scene->sceneRect();

    emit progressUpdate(100);

    if (parseError) {
        setError("Parse error occurred while reading file");
        scene->setLoading(false);
        return false;
    }

    scene->update();
    scene->setLoading(false);
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
    // Qt6 中不再需要手动设置编码，默认使用 UTF-8

    // 写入DieSize - 转换为左下角坐标系格式
    if (scene) {
        QRectF sceneRect = scene->sceneRect();
        QString canvasUnit = scene->get_unit();

        // Qt坐标系: (0, 0, width, height)
        // 文件格式: left bottom right top (左下角坐标系)
        double left = sceneRect.left();
        double bottom = 0;  // 左下角Y坐标
        double right = sceneRect.right();
        double top = sceneRect.height();  // 右上角Y坐标

        // 转换到文件单位
        left = convertToFileUnit(left, canvasUnit);
        bottom = convertToFileUnit(bottom, canvasUnit);
        right = convertToFileUnit(right, canvasUnit);
        top = convertToFileUnit(top, canvasUnit);

        out << "DieSize " << left << " " << bottom << " " << right << " " << top << "\n\n";
    }

    emit progressUpdate(25);

    // 生成宏定义
    if (!generateMacroDefinitions(out, cellItems, scene)) {
        return false;
    }

    emit progressUpdate(50);

    // 生成实例
    if (!generateInstances(out, cellItems, scene)) {
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
    // Qt6 中不再需要手动设置编码，默认使用 UTF-8

    // 检查文件扩展名，决定输出格式
    QString extension = QFileInfo(filePath).suffix().toLower();

    if (extension == "place" || extension == "txt") {
        // 输出标准place文件格式
        out << "DieSize 0 0 4000 4000\n\n";

        // 先输出宏定义
        generateMacroDefinitions(out, cellItems, nullptr);

        // 然后输出实例（如果有的话）
        if (!cellItems.isEmpty()) {
            out << "NumInstances " << cellItems.size() << "\n";
            for (int i = 0; i < cellItems.size(); ++i) {
                const CellItem* cellItem = cellItems[i];
                QString instanceName = cellItem->getInstanceName();
                if (instanceName.isEmpty()) {
                    instanceName = "C" + QString::number(i + 1);
                }
                out << "Inst " << instanceName << " " << cellItem->getMacroName()
                    << " 100 100\n"; // 默认位置
            }
            out << "\n";
        }

        // 网络部分（如果有连接）
        out << "NumNets 0\n";

    } else {
        // 输出自定义宏文件格式（保持原有格式）
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
    // Qt6 中不再需要手动设置编码，默认使用 UTF-8
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

    // 获取画布当前单位并转换
    QString canvasUnit = scene->get_unit();
    left = convertFromFileUnit(left, canvasUnit);
    bottom = convertFromFileUnit(bottom, canvasUnit);
    right = convertFromFileUnit(right, canvasUnit);
    top = convertFromFileUnit(top, canvasUnit);

    // 计算宽度和高度
    double width = right - left;
    double height = top - bottom;

    // 设置场景矩形：Qt坐标系中Y轴向下为正，传统CAD坐标系Y轴向上为正
    // 我们保持原始坐标系，让场景矩形从(0,0)开始，大小为width x height
    scene->setSceneRect(0, 0, width, height);

    qDebug() << "设置场景矩形: 位置(0, 0) 大小:" << width << "x" << height << canvasUnit;
    qDebug() << "原始边界(文件单位): left=" << parts[1] << ", bottom=" << parts[2] << ", right=" << parts[3] << ", top=" << parts[4];
    qDebug() << "转换后边界(" << canvasUnit << "): left=" << left << ", bottom=" << bottom << ", right=" << right << ", top=" << top;
    return true;
}

bool FileManager::parseMacroDefinition(const QString& line, QTextStream& stream,
                                       QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>>& macroTypes,
                                       CanvasScene* scene)
{
    qDebug() << "  >>> 解析Macro行:" << line;

    QStringList parts = line.split(" ", Qt::SkipEmptyParts);
    if (parts.size() < 5) {
        qDebug() << "  !!! Macro行格式错误，部分数量不足:" << parts.size() << "期望至少5个";
        return false;
    }

    QString macroName = parts[1];
    bool ok;
    double width = parts[2].toDouble(&ok);
    if (!ok || width <= 0) {
        qDebug() << "  !!! 宽度解析失败或无效:" << parts[2];
        return false;
    }
    double height = parts[3].toDouble(&ok);
    if (!ok || height <= 0) {
        qDebug() << "  !!! 高度解析失败或无效:" << parts[3];
        return false;
    }
    int pinCount = parts[4].toInt(&ok);
    if (!ok || pinCount < 0) {
        qDebug() << "  !!! 引脚数量解析失败或无效:" << parts[4];
        return false;
    }

    qDebug() << "  >>> Macro详细信息:";
    qDebug() << "      宏名:" << macroName;
    qDebug() << "      原始尺寸:" << width << "x" << height;
    qDebug() << "      引脚数量:" << pinCount;

    // 获取画布当前单位并转换宏的尺寸
    QString canvasUnit = scene->get_unit();
    width = convertFromFileUnit(width, canvasUnit);
    height = convertFromFileUnit(height, canvasUnit);

    qDebug() << "      画布单位:" << canvasUnit;
    qDebug() << "      转换后尺寸:" << width << "x" << height;

    QList<CellItem::Connector> connectors;
    for (int i = 0; i < pinCount && !stream.atEnd(); ++i) {
        QString pinLine = stream.readLine().trimmed();
        // qDebug() << "        解析引脚行" << (i+1) << ":" << pinLine;

        QStringList pinParts = pinLine.split(" ", Qt::SkipEmptyParts);
        if (pinParts.size() < 6 || pinParts[0] != "Pin") {
            qDebug() << "        !!! 引脚解析失败: 期望格式 'Pin id x y width height'，实际:" << pinLine;
            qDebug() << "        分割后的部分数量:" << pinParts.size() << "内容:" << pinParts;
            return false;
        }

        CellItem::Connector conn;
        conn.id = pinParts[1];
        qreal pinX = static_cast<qreal>(pinParts[2].toDouble(&ok));
        if (!ok) {
            qDebug() << "        !!! 无法解析引脚X坐标:" << pinParts[2];
            return false;
        }
        qreal pinY = static_cast<qreal>(pinParts[3].toDouble(&ok));
        if (!ok) {
            qDebug() << "        !!! 无法解析引脚Y坐标:" << pinParts[3];
            return false;
        }

        qDebug() << "        引脚" << conn.id << "原始坐标:" << pinX << "," << pinY;

        // 转换引脚坐标单位
        pinX = convertFromFileUnit(pinX, canvasUnit);
        pinY = convertFromFileUnit(pinY, canvasUnit);

        // qDebug() << "        单位转换后坐标:" << pinX << "," << pinY;

        // 正确的坐标转换：从左下角坐标系转换为Qt的左上角坐标系
        // 文件中的Y坐标是相对于芯片左下角的，需要转换为相对于芯片左上角的
        conn.x = pinX;
        conn.y = height - pinY; // Y坐标需要反转：从底部坐标转换为顶部坐标

        // qDebug() << "        坐标系转换后(Qt左上角):" << conn.x << "," << conn.y;

        // 对于从文件加载的引脚，始终使用custom类型以保持精确位置
        // 这样可以确保引脚位置与文件中的定义完全一致
        conn.side = "custom";
        conn.percentage = 0.0;

        connectors.append(conn);
        qDebug() << "        成功解析引脚:" << conn.id << "最终坐标:" << conn.x << "," << conn.y
                 << "类型: custom";
    }

    macroTypes[macroName] = qMakePair(QSizeF(width, height), connectors);
    qDebug() << "  >>> Macro解析完成:" << macroName << "引脚数量:" << connectors.size();
    qDebug() << "      当前宏类型Map大小:" << macroTypes.size();

    return true;
}

bool FileManager::parseInstance(const QString& line, CanvasScene* scene,
                                const QMap<QString, QPair<QSizeF, QList<CellItem::Connector>>>& macroTypes,
                                QMap<QString, CellItem*>& cellMap,
                                QMap<QString, QString>& instanceNameMapping)
{
    qDebug() << "  >>> 解析Instance行:" << line;

    QStringList parts = line.split(" ", Qt::SkipEmptyParts);
    if (parts.size() < 5) {
        qDebug() << "  !!! Instance行格式错误，部分数量不足:" << parts.size() << "期望至少5个";
        return false;
    }

    QString originalInstanceName = parts[1];
    QString macroName = parts[2];
    bool ok;
    double posX = parts[3].toDouble(&ok);
    if (!ok) {
        qDebug() << "  !!! X坐标解析失败:" << parts[3];
        return false;
    }
    double posY = parts[4].toDouble(&ok);
    if (!ok) {
        qDebug() << "  !!! Y坐标解析失败:" << parts[4];
        return false;
    }

    qDebug() << "  >>> Instance详细信息:";
    qDebug() << "      实例名:" << originalInstanceName;
    qDebug() << "      宏名:" << macroName;
    qDebug() << "      原始坐标:" << posX << "," << posY;

    // 获取画布当前单位并转换实例位置
    QString canvasUnit = scene->get_unit();
    posX = convertFromFileUnit(posX, canvasUnit);
    posY = convertFromFileUnit(posY, canvasUnit);

    qDebug() << "      画布单位:" << canvasUnit;
    qDebug() << "      转换后坐标:" << posX << "," << posY;

    if (!macroTypes.contains(macroName)) {
        qDebug() << "  !!! 找不到宏定义:" << macroName;
        qDebug() << "      可用的宏:" << macroTypes.keys();
        return false;
    }

    auto macroInfo = macroTypes[macroName];
    qDebug() << "      找到宏定义，尺寸:" << macroInfo.first << "引脚数量:" << macroInfo.second.size();

    // 需要获取场景的高度来进行Y坐标转换
    QRectF sceneRect = scene->sceneRect();
    double sceneHeight = sceneRect.height();

    // 获取芯片高度
    double chipHeight = macroInfo.first.height();

    // 正确的坐标转换：从左下角坐标系转换为Qt的左上角坐标系
    // 分析：test_example.txt中下排芯片Y=5应该显示在底部，上排芯片Y=20应该在上方
    double qtPosX = posX;
    // 关键：Y坐标转换
    // 文件中posY是芯片左下角距离场景底部的距离
    // Qt中需要芯片左上角距离场景顶部的距离
    // 转换公式：qtY = sceneHeight - fileY - chipHeight
    double qtPosY = sceneHeight - posY - chipHeight;

    qDebug() << "      坐标转换详情:";
    qDebug() << "        场景高度:" << sceneHeight;
    qDebug() << "        芯片高度:" << chipHeight;
    qDebug() << "        文件Y坐标(左下):" << posY;
    qDebug() << "        Qt Y坐标(左上):" << qtPosY;
    qDebug() << "        最终Qt坐标:" << qtPosX << "," << qtPosY;

    m_chipCounter++;
    QString instanceName = originalInstanceName;
    if (cellMap.contains(instanceName)) {
        instanceName = "C" + QString::number(m_chipCounter);
        instanceNameMapping[originalInstanceName] = instanceName;
        qDebug() << "      重复实例名，重命名为:" << instanceName;
    } else {
        instanceNameMapping[originalInstanceName] = originalInstanceName;
    }

    qDebug() << "  >>> 创建CellItem...";
    CellItem* cellItem = new CellItem();
    cellItem->setPos(qtPosX, qtPosY);
    cellItem->setSize(macroInfo.first);
    cellItem->setMacroName(macroName);
    cellItem->setInstanceName(instanceName);

    qDebug() << "      CellItem创建完成，设置属性:";
    qDebug() << "        位置:" << cellItem->pos();
    qDebug() << "        尺寸:" << cellItem->size();
    qDebug() << "        宏名:" << cellItem->getMacroName();
    qDebug() << "        实例名:" << cellItem->getInstanceName();
    cellItem->setSize(macroInfo.first);
    cellItem->setMacroName(macroName);
    cellItem->setInstanceName(instanceName);

    qDebug() << "创建芯片实例:" << instanceName << "宏:" << macroName;
    qDebug() << "文件坐标(左下角):" << posX << "," << posY;
    qDebug() << "Qt坐标(左上角):" << qtPosX << "," << qtPosY;
    qDebug() << "芯片尺寸:" << macroInfo.first << "引脚数量:" << macroInfo.second.size();
    qDebug() << "场景高度:" << sceneHeight << "芯片高度:" << chipHeight;

    // 添加连接器
    qDebug() << "  >>> 添加引脚连接器:";
    for (const auto& conn : macroInfo.second) {
        qDebug() << "        引脚:" << conn.id << "边:" << conn.side << "位置:" << conn.x << "," << conn.y;
        cellItem->addConnector(conn.side, static_cast<qreal>(conn.percentage), static_cast<qreal>(1), conn.id,
                               static_cast<qreal>(conn.x), static_cast<qreal>(conn.y));
    }

    qDebug() << "  >>> 将芯片添加到场景...";
    scene->addCellItem(cellItem);
    cellMap[instanceName] = cellItem;

    qDebug() << "  >>> Instance解析完成！";
    qDebug() << "      cellMap中现有芯片数量:" << cellMap.size();
    qDebug() << "      场景中实际芯片数量:" << scene->getAllCellItems().size();

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

        // 解析格式：支持两种格式
        // 格式1：Pin C1/pin_1 (使用斜杠)
        // 格式2：Pin C1 pin_1 (使用空格)
        QString instanceName, pinName;

        if (pinParts.size() == 2) {
            // 格式1：Pin C1/pin_1
            QString fullPin = pinParts[1];
            QStringList pinSpec = fullPin.split("/");
            if (pinSpec.size() == 2) {
                instanceName = pinSpec[0];
                pinName = pinSpec[1];
            } else {
                qDebug() << "引脚格式错误（期望 instance/pin）:" << fullPin;
                return false;
            }
        } else if (pinParts.size() == 3) {
            // 格式2：Pin C1 pin_1
            instanceName = pinParts[1];
            pinName = pinParts[2];
        } else {
            qDebug() << "引脚格式错误:" << pinLine;
            return false;
        }

        // 处理实例名映射
        if (instanceNameMapping.contains(instanceName)) {
            QString originalInstance = instanceName;
            instanceName = instanceNameMapping[instanceName];
            qDebug() << "映射实例名:" << originalInstance << "->" << instanceName;
        }
        netPins.append(qMakePair(instanceName, pinName));
        qDebug() << "网络" << netName << "包含引脚:" << instanceName << "/" << pinName;
    }

    // 检查是否有重复的引脚连接
    QSet<QString> pinSet;
    for (const auto& pin : netPins) {
        QString pinKey = pin.first + "/" + pin.second;
        if (pinSet.contains(pinKey)) {
            qWarning() << "警告：网络" << netName << "中发现重复引脚:" << pinKey;
        }
        pinSet.insert(pinKey);
    }

    // 创建连线
    if (netPins.size() < 2) {
        // qDebug() << "网络" << netName << "引脚数量不足，跳过连线创建";
        return true; // 网络至少需要2个引脚
    }

    QList<ConnectionLine*> createdLines; // 收集属于该命名线网的连线
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
            createdLines.append(line);
        }
    }

    // 构建命名线网：记录所有参与的芯片与引脚
    QList<QPair<CellItem*, QString>> pinRefs;
    for (const auto &p : netPins) {
        if (cellMap.contains(p.first)) {
            pinRefs.append(qMakePair(cellMap[p.first], p.second));
        }
    }
    if (!pinRefs.isEmpty()) {
        scene->addParsedNet(netName, pinRefs, createdLines);
    }

    return true;
}

bool FileManager::generateMacroDefinitions(QTextStream& stream, const QList<CellItem*>& cellItems, CanvasScene* scene)
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

        // 获取画布单位并转换宏的尺寸到文件单位
        QString canvasUnit = scene ? scene->get_unit() : "cm"; // 默认cm
        qreal fileWidth = convertToFileUnit(size.width(), canvasUnit);
        qreal fileHeight = convertToFileUnit(size.height(), canvasUnit);

        stream << "Macro " << macroName << " " << fileWidth << " " << fileHeight
               << " " << connectors.size() << "\n";

        // 写入引脚信息 - 转换回左下角坐标系并转换单位
        for (const auto& conn : connectors) {
            // 将Qt坐标系(左上角)转换回文件格式的左下角坐标系
            qreal qtY = conn.y;  // Qt坐标系中的Y坐标
            qreal fileY = size.height() - qtY;  // 转换为左下角坐标系的Y坐标

            // 转换坐标单位到文件单位
            qreal fileX = convertToFileUnit(conn.x, canvasUnit);
            fileY = convertToFileUnit(fileY, canvasUnit);

            // 使用标准的引脚宽度和高度（10 10），与原始格式保持一致
            stream << "Pin " << conn.id << " " << fileX << " " << fileY
                   << " 10 10" << "\n";
        }
    }
    stream << "\n";

    return true;
}

bool FileManager::generateInstances(QTextStream& stream, const QList<CellItem*>& cellItems, CanvasScene* scene)
{
    stream << "NumInstances " << cellItems.size() << "\n";

    // 获取场景高度用于坐标转换
    double sceneHeight = 0;
    QString canvasUnit = "cm"; // 默认单位
    if (scene) {
        sceneHeight = scene->sceneRect().height();
        canvasUnit = scene->get_unit();
    }

    for (CellItem* cellItem : cellItems) {
        QString instanceName = cellItem->getInstanceName();
        if (instanceName.isEmpty()) {
            instanceName = "C" + QString::number(cellItems.indexOf(cellItem) + 1);
            cellItem->setInstanceName(instanceName);
        }

        // 正确的坐标转换：从Qt左上角坐标系转换回文件格式的左下角坐标系
        QPointF qtPos = cellItem->pos();  // Qt坐标系位置(左上角)
        QSizeF chipSize = cellItem->size();

        // Qt中的位置是芯片左上角，文件中需要芯片左下角
        // 转换公式：fileY = sceneHeight - qtY - chipHeight
        double fileX = qtPos.x();
        double fileY = sceneHeight - qtPos.y() - chipSize.height();

        // 转换坐标单位到文件单位
        fileX = convertToFileUnit(fileX, canvasUnit);
        fileY = convertToFileUnit(fileY, canvasUnit);

        stream << "Inst " << instanceName << " " << cellItem->getMacroName() << " "
               << fileX << " " << fileY << "\n";
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
            // 使用空格分隔格式，与解析器保持一致
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

qreal FileManager::convertFromFileUnit(qreal value, const QString& canvasUnit) const
{
    // 文件中的数值直接使用画布当前单位，不进行单位转换
    // 例如：画布是厘米，文件中的数字就当作厘米处理
    // qDebug() << "不进行单位转换: 文件值" << value << "直接作为" << canvasUnit << "单位";
    return value;
}

qreal FileManager::convertToFileUnit(qreal value, const QString& canvasUnit) const
{
    // 保存时直接使用画布的当前单位，不进行单位转换
    // 例如：画布是厘米，保存的数字就是厘米值
    // qDebug() << "不进行单位转换: 画布值" << value << canvasUnit << "直接保存为文件值";
    return value;
}
