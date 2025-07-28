#include "pinmanager.h"
#include "cellitem_new.h"
#include <QDebug>
#include <QGraphicsRectItem>

PinManager::PinManager(QObject *parent)
    : QObject(parent)
    , m_pinCounter(0)
{
}

PinManager::~PinManager()
{
    // 清理所有引脚
    for (auto it = m_cellPins.begin(); it != m_cellPins.end(); ++it) {
        qDeleteAll(it.value());
    }
    m_cellPins.clear();
}

void PinManager::addPin(CellItem* cellItem, const QString& side, qreal percentage,
                        qreal size, const QString& id, qreal x, qreal y)
{
    if (!cellItem) {
        qWarning() << "Cannot add pin to null cellItem";
        return;
    }

    QString pinId = id.isEmpty() ? generatePinId(m_pinCounter++) : id;

    // 如果坐标为0，计算边缘位置
    if (x == 0 && y == 0 && side != "custom") {
        QSizeF cellSize = cellItem->size();
        calculatePinPosition(side, percentage, cellSize.width(),
                             cellSize.height(), size, x, y);
    }

    // 创建引脚图形项
    PinItem* pinItem = createPinItem(cellItem, size, pinId, x, y);
    if (!pinItem) {
        qWarning() << "Failed to create pin item for id:" << pinId;
        return;
    }

    // 设置引脚属性（使用重载的updateConnector方法）
    pinItem->updateConnector(pinId, x, y, side, percentage);
    pinItem->setBrush(Qt::darkBlue);
    pinItem->setPen(Qt::NoPen);

    // 添加到管理列表
    m_cellPins[cellItem].append(pinItem);

    // 通过CellItem添加连接器
    cellItem->addConnector(side, static_cast<qreal>(percentage), static_cast<qreal>(size), pinId, static_cast<qreal>(x), static_cast<qreal>(y));

    emit pinAdded(pinId);
    qDebug() << "Added pin:" << pinId << " at side=" << side
             << ", percentage=" << percentage << ", x=" << x << ", y=" << y;
}

bool PinManager::removePin(CellItem* cellItem, const QString& id)
{
    if (!cellItem || !m_cellPins.contains(cellItem)) return false;

    QList<PinItem*>& pins = m_cellPins[cellItem];
    for (int i = 0; i < pins.size(); ++i) {
        if (pins[i]->getId() == id) {
            PinItem* pinItem = pins.takeAt(i);
            if (pinItem->scene()) {
                pinItem->scene()->removeItem(pinItem);
            }
            delete pinItem;

            cellItem->removeConnector(id);
            emit pinRemoved(id);
            return true;
        }
    }
    return false;
}

bool PinManager::removePin(CellItem* cellItem, int index)
{
    if (!cellItem || !m_cellPins.contains(cellItem)) return false;

    QList<PinItem*>& pins = m_cellPins[cellItem];
    if (index < 0 || index >= pins.size()) return false;

    PinItem* pinItem = pins.takeAt(index);
    QString id = pinItem->getId();

    if (pinItem->scene()) {
        pinItem->scene()->removeItem(pinItem);
    }
    delete pinItem;

    cellItem->removeConnector(index);
    emit pinRemoved(id);
    return true;
}

QList<PinManager::Connector> PinManager::getPins(const CellItem* cellItem) const
{
    QList<Connector> connectors;
    if (!cellItem || !m_cellPins.contains(cellItem)) return connectors;

    const QList<PinItem*>& pins = m_cellPins[cellItem];
    for (const PinItem* pin : pins) {
        Connector conn;
        conn.side = pin->getSide();
        conn.percentage = pin->getPercentage();
        conn.id = pin->getId();
        conn.x = pin->getX();
        conn.y = pin->getY();
        connectors.append(conn);
    }

    return connectors;
}

PinItem* PinManager::findPinById(const CellItem* cellItem, const QString& id) const
{
    if (!cellItem || !m_cellPins.contains(cellItem)) return nullptr;

    const QList<PinItem*>& pins = m_cellPins[cellItem];
    for (PinItem* pin : pins) {
        if (pin->getId() == id) {
            return pin;
        }
    }
    return nullptr;
}

void PinManager::updatePinScene(QGraphicsScene* scene, const CellItem* cellItem,
                                int chipWidth, int chipHeight)
{
    if (!scene || !cellItem) return;

    // 清除旧的引脚显示
    clearPinScene(scene);

    // 创建芯片矩形
    QGraphicsRectItem* chipRect = new QGraphicsRectItem(0, 0, chipWidth, chipHeight);
    chipRect->setPen(QPen(Qt::black));
    chipRect->setBrush(Qt::lightGray);
    scene->addItem(chipRect);

    // 添加引脚显示
    if (m_cellPins.contains(cellItem)) {
        const QList<PinItem*>& pins = m_cellPins[cellItem];
        for (PinItem* pin : pins) {
            PinItem* scenePin = new PinItem(chipRect, 10, nullptr);
            scenePin->updateConnector(pin->getId(), static_cast<qreal>(pin->getX()), static_cast<qreal>(pin->getY()));
            scene->addItem(scenePin);
        }
    }

    scene->setSceneRect(0, 0, chipWidth, chipHeight);
}

void PinManager::clearPinScene(QGraphicsScene* scene)
{
    if (!scene) return;

    scene->clear();
}

QString PinManager::generatePinId(int index) const
{
    return QString("pin_%1").arg(index);
}

PinItem* PinManager::createPinItem(QGraphicsItem* parentRect, qreal size,
                                   const QString& id, qreal x, qreal y)
{
    PinItem* pinItem = new PinItem(parentRect, size);
    pinItem->updateConnector(id, static_cast<qreal>(x), static_cast<qreal>(y));
    return pinItem;
}

void PinManager::calculatePinPosition(const QString& side, qreal percentage,
                                      qreal chipWidth, qreal chipHeight,
                                      qreal pinSize, qreal& x, qreal& y)
{
    if (side == "top") {
        x = chipWidth * percentage / 100.0 - pinSize / 2;
        y = 0;
    } else if (side == "bottom") {
        x = chipWidth * percentage / 100.0 - pinSize / 2;
        y = chipHeight - pinSize;
    } else if (side == "left") {
        x = 0;
        y = chipHeight * percentage / 100.0 - pinSize / 2;
    } else if (side == "right") {
        x = chipWidth - pinSize;
        y = chipHeight * percentage / 100.0 - pinSize / 2;
    } else if (side == "custom") {
        // 对于自定义位置，如果未提供坐标，则使用中心位置
        if (x == 0 && y == 0) {
            x = chipWidth / 2 - pinSize / 2;
            y = chipHeight / 2 - pinSize / 2;
        }
    }
}
