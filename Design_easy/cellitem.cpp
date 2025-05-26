#include "cellitem.h"
#include "dialogs.h"

#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QGraphicsView>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

CellItem::CellItem(QGraphicsItem *parent)
    : QGraphicsItem(parent), m_size(100, 100), m_resizeEdge(None), m_isDragging(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    setCursor(QCursor(Qt::OpenHandCursor));
}

CellItem::~CellItem()
{
    // 清理所有引脚对象，避免内存泄漏
    for (PinItem* pin : m_pinItems) {
        if (pin && pin->scene()) {
            pin->scene()->removeItem(pin);
        }
        delete pin;
    }
    m_pinItems.clear();
    m_connectors.clear();
}

QRectF CellItem::boundingRect() const
{
    // 返回绘制区域的矩形
    return QRectF(0, 0, m_size.width(), m_size.height());
}

void CellItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    qDebug() << "Painting CellItem at pos:" << pos();
    if (!painter) {
        qDebug() << "无效的 QPainter";
        return;
    }

    QPen pen(Qt::black);
    if (isSelected()) {
        pen.setColor(Qt::red); // 被选中时高亮
        pen.setWidth(2);
    }
    painter->setPen(pen);
    painter->setBrush(Qt::lightGray);
    painter->drawRect(boundingRect());

    // 绘制重合区域
    if (!m_overlapAreas.isEmpty()) {
        QColor overlapColor = QColor(255, 0, 0, 100);  // 半透明红色
        painter->setPen(Qt::NoPen);
        painter->setBrush(overlapColor);
        for (const QPainterPath& path : m_overlapAreas) {
            painter->drawPath(path);
        }
    }

    // 现有绘制代码,加上可以修改大小的点
    if (isSelected()) {
        painter->setBrush(Qt::blue);
        painter->drawRect(QRectF(0, 0, cornerSize, cornerSize)); // 左上
        painter->drawRect(QRectF(m_size.width() - cornerSize, 0, cornerSize, cornerSize)); // 右上
        painter->drawRect(QRectF(0, m_size.height() - cornerSize, cornerSize, cornerSize)); // 左下
        painter->drawRect(QRectF(m_size.width() - cornerSize, m_size.height() - cornerSize, cornerSize, cornerSize)); // 右下
    }
    
    // 绘制实例名称和宏名称
    if (!m_instanceName.isEmpty() || !m_macroName.isEmpty()) {
        painter->setPen(Qt::black);
        QString text = m_instanceName;
        if (!m_macroName.isEmpty()) {
            if (!text.isEmpty()) text += " (";
            text += m_macroName;
            if (!m_instanceName.isEmpty()) text += ")";
        }
        painter->drawText(boundingRect().adjusted(5, 5, -5, -5), Qt::AlignCenter, text);
    }
}

void CellItem::setSize(const QSizeF& size) {
    prepareGeometryChange();
    m_size = size;
    if (m_pinItems.size() != m_connectors.size()) {
        qWarning() << "m_pinItems and m_connectors out of sync in setSize: pinItems=" << m_pinItems.size()
        << ", connectors=" << m_connectors.size();
    }
    int count = qMin(m_pinItems.size(), m_connectors.size());
    for (int i = 0; i < count; ++i) {
        if (i >= m_pinItems.size() || i >= m_connectors.size()) {
            qWarning() << "Index out of range in setSize: i=" << i << ", pinItems=" << m_pinItems.size()
            << ", connectors=" << m_connectors.size();
            break;
        }
        PinItem* pinItem = m_pinItems[i];
        Connector& conn = m_connectors[i];
        if (!pinItem) {
            qWarning() << "Null pinItem at index" << i << "in setSize";
            continue;
        }
        QPointF pos = conn.calculatePos(m_size, pinItem->boundingRect().width());
        pinItem->setPos(pos);
    }
    update();
    qDebug() << "setSize completed: size=" << m_size << ", pinItems=" << m_pinItems.size()
             << ", connectors=" << m_connectors.size();
}

QSizeF CellItem::size() const
{
    return m_size;
}

QPointF CellItem::Connector::calculatePos(const QSizeF& cellSize, qreal pinSize) const {
    qreal posX = x, posY = y;
    
    // 如果是自定义位置，直接使用保存的x,y坐标
    if (side == "custom") {
        return QPointF(posX, posY);
    }
    
    // 否则根据边缘和百分比计算位置
    if (side == "top") {
        posX = cellSize.width() * percentage / 100.0 - pinSize / 2;
        posY = 0;
    } else if (side == "bottom") {
        posX = cellSize.width() * percentage / 100.0 - pinSize / 2;
        posY = cellSize.height() - pinSize;
    } else if (side == "left") {
        posX = 0;
        posY = cellSize.height() * percentage / 100.0 - pinSize / 2;
    } else if (side == "right") {
        posX = cellSize.width() - pinSize;
        posY = cellSize.height() * percentage / 100.0 - pinSize / 2;
    }
    return QPointF(posX, posY);
}

void CellItem::addConnector(const QString& side, qreal percentage, qreal size, const QString& id, qreal x, qreal y) {
    if (!scene()) {
        qWarning() << "No scene available, cannot add connector:" << id;
        return;
    }
    percentage = qBound(0.0, percentage, 100.0);
    Connector connector(side, percentage, id, x, y);
    m_connectors.append(connector);
    PinItem* pinItem = new PinItem(this, size, this);
    pinItem->setBrush(Qt::darkBlue);
    pinItem->setPen(Qt::NoPen);
    pinItem->setPos(x, y);
    pinItem->updateConnector(id, x, y);
    m_pinItems.append(pinItem);
    update();
    qDebug() << "Added PinItem to scene for connector:" << id << "at side=" << side << ", percentage=" << percentage;
    qDebug() << "After addConnector: pinItems=" << m_pinItems.size() << ", connectors=" << m_connectors.size();
}

bool CellItem::removeConnector(const QString& id) {
    if (id.isEmpty()) {
        qWarning() << "Cannot remove connector with empty id";
        return false;
    }
    
    int index = -1;
    for (int i = 0; i < m_connectors.size(); ++i) {
        if (m_connectors[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index >= 0 && index < m_connectors.size() && index < m_pinItems.size()) {
        return removeConnector(index);
    }
    
    qWarning() << "Connector id=" << id << " not found or index out of range";
    return false;
}

bool CellItem::removeConnector(int index) {
    if (index < 0 || index >= m_connectors.size() || index >= m_pinItems.size()) {
        qWarning() << "Invalid index for removeConnector: index=" << index
                   << ", connectors=" << m_connectors.size()
                   << ", pinItems=" << m_pinItems.size();
        return false;
    }
    QString removedId = m_connectors[index].id;
    // 从场景中移除引脚
    PinItem* pinItem = m_pinItems[index];
    if (pinItem) {
        if (pinItem->scene()) {
            pinItem->scene()->removeItem(pinItem);
        }
        delete pinItem;
    }
    
    // 从列表中移除引脚和连接器
    m_pinItems.removeAt(index);
    m_connectors.removeAt(index);
    
    // 移除与此引脚相关的所有连线
    // QString removedId = m_connectors[index].id;
    for (int i = m_connections.size() - 1; i >= 0; --i) {
        if (m_connections[i].second.first == removedId || m_connections[i].second.second == removedId) {
            m_connections.removeAt(i);
        }
    }
    
    update();
    qDebug() << "Removed connector at index=" << index << ", remaining connectors=" << m_connectors.size()
             << ", remaining pinItems=" << m_pinItems.size();
    return true;
}

QList<CellItem::Connector> CellItem::getConnectors() const
{
    return m_connectors;
}

QList<PinItem*> CellItem::getPinItems() const {
    return m_pinItems;
}

bool CellItem::isOnConnector(const QPointF& pos, Connector& connector) const
{
    for (const Connector& conn : m_connectors) {
        QPointF connPos = conn.calculatePos(m_size, connectorSize);
        QRectF connRect(connPos.x(), connPos.y(), connectorSize, connectorSize);
        if (connRect.contains(pos)) {
            connector = conn;
            return true;
        }
    }
    return false;
}

void CellItem::addConnection(CellItem* targetCell, const QString& sourcePin, const QString& targetPin) {
    if (!targetCell) {
        qWarning() << "Cannot add connection to null target cell";
        return;
    }
    
    // 检查源引脚是否存在
    bool sourceExists = false;
    for (const Connector& conn : m_connectors) {
        if (conn.id == sourcePin) {
            sourceExists = true;
            break;
        }
    }
    
    // 检查目标引脚是否存在
    bool targetExists = false;
    for (const Connector& conn : targetCell->getConnectors()) {
        if (conn.id == targetPin) {
            targetExists = true;
            break;
        }
    }
    
    if (!sourceExists || !targetExists) {
        qWarning() << "Cannot add connection: source or target pin does not exist";
        return;
    }
    
    // 添加连接
    m_connections.append(qMakePair(targetCell, qMakePair(sourcePin, targetPin)));
    qDebug() << "Added connection from" << sourcePin << "to" << targetPin;
    update();
}

QList<QPair<CellItem*, QPair<QString, QString>>> CellItem::getConnections() const {
    return m_connections;
}

QJsonObject CellItem::toJson() const {
    QJsonObject json;
    
    // 基本属性
    json["macroName"] = m_macroName;
    json["instanceName"] = m_instanceName;
    json["width"] = m_size.width();
    json["height"] = m_size.height();
    json["posX"] = pos().x();
    json["posY"] = pos().y();
    
    // 引脚信息
    QJsonArray pinsArray;
    for (const Connector& conn : m_connectors) {
        QJsonObject pinJson;
        pinJson["id"] = conn.id;
        pinJson["side"] = conn.side;
        pinJson["percentage"] = conn.percentage;
        pinJson["x"] = conn.x;
        pinJson["y"] = conn.y;
        pinsArray.append(pinJson);
    }
    json["pins"] = pinsArray;
    
    // 连线信息
    QJsonArray connectionsArray;
    for (const auto& conn : m_connections) {
        QJsonObject connJson;
        connJson["targetCell"] = conn.first ? conn.first->getInstanceName() : "";
        connJson["sourcePin"] = conn.second.first;
        connJson["targetPin"] = conn.second.second;
        connectionsArray.append(connJson);
    }
    json["connections"] = connectionsArray;
    
    return json;
}

void CellItem::fromJson(const QJsonObject& json) {
    // 清理现有数据
    for (PinItem* pin : m_pinItems) {
        if (pin && pin->scene()) {
            pin->scene()->removeItem(pin);
        }
        delete pin;
    }
    m_pinItems.clear();
    m_connectors.clear();
    m_connections.clear();
    
    // 加载基本属性
    m_macroName = json["macroName"].toString();
    m_instanceName = json["instanceName"].toString();
    setSize(QSizeF(json["width"].toDouble(), json["height"].toDouble()));
    setPos(json["posX"].toDouble(), json["posY"].toDouble());
    
    // 加载引脚信息
    QJsonArray pinsArray = json["pins"].toArray();
    for (const QJsonValue& pinValue : pinsArray) {
        QJsonObject pinJson = pinValue.toObject();
        QString id = pinJson["id"].toString();
        QString side = pinJson["side"].toString();
        qreal percentage = pinJson["percentage"].toDouble();
        qreal x = pinJson["x"].toDouble();
        qreal y = pinJson["y"].toDouble();
        
        // 添加引脚
        addConnector(side, percentage, connectorSize, id, x, y);
    }
    
    // 连线信息需要在所有CellItem都加载完成后处理
    // 这里只记录连线信息，实际连线在外部完成
    
    update();
}

bool CellItem::isOnEdgeOrCorner(const QPointF &pos, ResizeEdge &edge) const
{
    QRectF rect = boundingRect();
    qreal x = pos.x();
    qreal y = pos.y();
    edge = None;

    // 检测角点（优先级高于边框）
    if (qAbs(x) <= cornerSize && qAbs(y) <= cornerSize) {
        edge = TopLeft;
    } else if (qAbs(x - rect.width()) <= cornerSize && qAbs(y) <= cornerSize) {
        edge = TopRight;
    } else if (qAbs(x) <= cornerSize && qAbs(y - rect.height()) <= cornerSize) {
        edge = BottomLeft;
    } else if (qAbs(x - rect.width()) <= cornerSize && qAbs(y - rect.height()) <= cornerSize) {
        edge = BottomRight;
    }
    // 检测边框
    else if (x >= -edgeWidth && x <= edgeWidth) {
        edge = Left;
    } else if (x >= rect.width() - edgeWidth && x <= rect.width() + edgeWidth) {
        edge = Right;
    } else if (y >= -edgeWidth && y <= edgeWidth) {
        edge = Top;
    } else if (y >= rect.height() - edgeWidth && y <= rect.height() + edgeWidth) {
        edge = Bottom;
    }

    return edge != None;
}

// 辅助函数：限制尺寸（仅用于调整大小）
void CellItem::restrictSize(QSizeF& size) {
    // 确保尺寸不小于最小值（20x20 像素）
    size.setWidth(qMax(20.0, size.width()));
    size.setHeight(qMax(20.0, size.height()));
    prepareGeometryChange();
    m_size = size;
    update();
}

// 辅助函数：限制位置（仅用于拖动）
void CellItem::restrictPosition(QPointF& pos) {
    if (!scene()) {
        qDebug() << "场景无效，无法限制位置";
        return;
    }
    QRectF sceneRect = scene()->sceneRect();
    if (sceneRect.isEmpty()) {
        qDebug() << "场景矩形无效";
        return;
    }
    pos.setX(qBound(sceneRect.left(), pos.x(), sceneRect.right() - m_size.width()));
    pos.setY(qBound(sceneRect.top(), pos.y(), sceneRect.bottom() - m_size.height()));
    setPos(pos);
    update();
}

void CellItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event || !scene()) {
        qDebug() << "无效的事件或场景";
        return;
    }
    qDebug() << "图元：鼠标按下位置:" << event->scenePos();

    if (event->button() == Qt::LeftButton && isSelected()) {
        // 检测是否在连接点上
        Connector connector;
        if (isOnConnector(event->pos(), connector)) {
            // 通知场景开始连线（通过自定义事件或信号）
            scene()->setProperty("startItem", QVariant::fromValue(this));
            scene()->setProperty("startConnector", QVariant::fromValue(connector));
            setCursor(QCursor(Qt::CrossCursor));
        }
        // 检测是否在边框或角点上
        else {
            ResizeEdge edge;
            if (isOnEdgeOrCorner(event->pos(), edge)) {
                m_resizeEdge = edge;
                switch (edge) {
                case TopLeft:
                case BottomRight:
                    setCursor(QCursor(Qt::SizeFDiagCursor));
                    break;
                case TopRight:
                case BottomLeft:
                    setCursor(QCursor(Qt::SizeBDiagCursor));
                    break;
                case Left:
                case Right:
                    setCursor(QCursor(Qt::SizeHorCursor));
                    break;
                case Top:
                case Bottom:
                    setCursor(QCursor(Qt::SizeVerCursor));
                    break;
                default:
                    break;
                }
            } else {
                m_resizeEdge = None;
                m_dragOffset = event->scenePos() - pos();
                setCursor(QCursor(Qt::ClosedHandCursor));
            }
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

// 辅助函数：限制尺寸和位置
void CellItem::restrictSizeAndPosition(QSizeF& size, QPointF& pos)
{
    // 确保尺寸不小于最小值（20x20 像素）
    size.setWidth(qMax(20.0, size.width()));
    size.setHeight(qMax(20.0, size.height()));

    // 限制位置在场景边界内
    QRectF sceneRect = scene()->sceneRect();
    if (sceneRect.isEmpty()) {
        qDebug() << "场景矩形无效";
        return;
    }
    pos.setX(qBound(sceneRect.left(), pos.x(), sceneRect.right() - size.width()));
    pos.setY(qBound(sceneRect.top(), pos.y(), sceneRect.bottom() - size.height()));

    // 合并几何更新
    prepareGeometryChange();
    m_size = size;
    setPos(pos);
    update();
}

void CellItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (!event || !scene()) {
        qDebug() << "无效的事件或场景";
        return;
    }

    if (event->buttons() & Qt::LeftButton && isSelected()) {
        if (!scene()->property("startItem").isNull()) {
            // 连线模式：由 CanvasScene 处理临时连线
            event->accept();
        } else if (m_resizeEdge != None) {
            // 调整矩形大小（边框或角点）
            QRectF rect = boundingRect();
            QPointF localPos = mapFromScene(event->scenePos());
            QSizeF newSize = m_size;
            QPointF newPos = pos();

            switch (m_resizeEdge) {
            case Left: // 调整左边，保持右边固定
                newSize.setWidth(rect.width() - localPos.x());
                newPos.setX(event->scenePos().x());
                break;
            case Right: // 调整右边，保持左边固定
                newSize.setWidth(localPos.x());
                break;
            case Top: // 调整上边，保持下边固定
                newSize.setHeight(rect.height() - localPos.y());
                newPos.setY(event->scenePos().y());
                break;
            case Bottom: // 调整下边，保持上边固定
                newSize.setHeight(localPos.y());
                break;
            case TopLeft: // 调整左上角，保持右下角固定
                newSize.setWidth(rect.width() - localPos.x());
                newSize.setHeight(rect.height() - localPos.y());
                newPos.setX(event->scenePos().x());
                newPos.setY(event->scenePos().y());
                break;
            case TopRight: // 调整右上角，保持左下角固定
                newSize.setWidth(localPos.x());
                newSize.setHeight(rect.height() - localPos.y());
                newPos.setY(event->scenePos().y());
                break;
            case BottomLeft: // 调整左下角，保持右上角固定
                newSize.setWidth(rect.width() - localPos.x());
                newSize.setHeight(localPos.y());
                newPos.setX(event->scenePos().x());
                break;
            case BottomRight: // 调整右下角，保持左上角固定
                newSize.setWidth(localPos.x());
                newSize.setHeight(localPos.y());
                break;
            default:
                break;
            }

            // 限制大小
            restrictSize(newSize);
            // 更新引脚位置
            setSize(newSize); // setSize 会更新引脚位置
            // 调整位置（仅对需要移动的边缘/角点）
            if (m_resizeEdge == Left || m_resizeEdge == Top || m_resizeEdge == TopLeft ||
                m_resizeEdge == TopRight || m_resizeEdge == BottomLeft) {
                restrictPosition(newPos);
            }
            
            // 更新重合状态
            updateOverlapState();
            // 通知其他重合的矩形也更新状态
            for (CellItem* other : m_overlappingItems) {
                if (other) {
                    other->updateOverlapState();
                }
            }
            
            qDebug() << "调整大小，边：" << m_resizeEdge << ", 新大小：" << newSize << ", 新位置：" << pos();
            event->accept();
        } else {
            // 拖动整个矩形
            QPointF newPos = event->scenePos() - m_dragOffset;
            restrictPosition(newPos);
            
            // 更新重合状态
            updateOverlapState();
            // 通知其他重合的矩形也更新状态
            for (CellItem* other : m_overlappingItems) {
                if (other) {
                    other->updateOverlapState();
                }
            }
            
            qDebug() << "拖动，新位置：" << newPos;
            event->accept();
        }
    }
    // 避免调用基类的默认拖动行为
    // QGraphicsItem::mouseMoveEvent(event);
}

// 处理鼠标释放事件，重置状态
void CellItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event || !scene()) {
        qDebug() << "无效的事件或场景";
        return;
    }
    qDebug() << "图元：鼠标释放位置:" << event->scenePos();
    m_resizeEdge = None;
    m_dragOffset = QPointF(0, 0);
    setCursor(QCursor(Qt::OpenHandCursor));
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void CellItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // 检查双击是否发生在单元格内部
    if (boundingRect().contains(event->pos())) {
        onDoubleClick();
    }

    // 确保将事件传递给基类处理
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void CellItem::onDoubleClick()
{
    Dialogs dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        const ComponentInfo& info = dialog.currentInfo;
        setSize(QSizeF(info.width, info.height));
    }
}

QVariant CellItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();
        QRectF sceneRect = scene()->sceneRect();
        // 限制 CellItem 在场景边界内
        qreal x = qMax(sceneRect.left(), qMin(newPos.x(), sceneRect.right() - m_size.width()));
        qreal y = qMax(sceneRect.top(), qMin(newPos.y(), sceneRect.bottom() - m_size.height()));
        return QPointF(x, y);
    }
    return QGraphicsItem::itemChange(change, value);
}

void CellItem::updateConnector(int index, const QString& side, qreal percentage, qreal x, qreal y) {
    if (index >= 0 && index < m_connectors.size()) {
        m_connectors[index].side = side;
        m_connectors[index].percentage = percentage;
        m_connectors[index].x = x;
        m_connectors[index].y = y;
        if (index < m_pinItems.size() && m_pinItems[index]) {
            m_pinItems[index]->updateConnector(m_connectors[index].id, x, y);
            m_pinItems[index]->setPos(x, y);
        }
        update();
        qDebug() << "Updated connector: index=" << index << ", id=" << m_connectors[index].id << ", x=" << x << ", y=" << y;
    } else {
        qWarning() << "Invalid connector index:" << index;
    }
}

QPainterPath CellItem::getShape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

bool CellItem::isOverlapping(const CellItem* other) const
{
    if (!other) return false;
    
    // 获取两个矩形的场景坐标
    QRectF thisRect = mapRectToScene(boundingRect());
    QRectF otherRect = other->mapRectToScene(other->boundingRect());
    
    return thisRect.intersects(otherRect);
}

QPainterPath CellItem::getOverlapArea(const CellItem* other) const
{
    if (!other) return QPainterPath();
    
    // 获取两个矩形的场景坐标
    QRectF thisRect = mapRectToScene(boundingRect());
    QRectF otherRect = other->mapRectToScene(other->boundingRect());
    
    // 计算交集
    QRectF intersection = thisRect.intersected(otherRect);
    if (!intersection.isEmpty()) {
        // 将交集区域转换回本地坐标
        QRectF localIntersection = mapRectFromScene(intersection);
        QPainterPath overlap;
        overlap.addRect(localIntersection);
        return overlap;
    }
    return QPainterPath();
}

void CellItem::updateOverlapState()
{
    m_overlapAreas.clear();
    m_overlappingItems.clear();
    
    if (!scene()) return;
    
    // 获取场景中所有的CellItem
    QList<QGraphicsItem*> items = scene()->items();
    for (QGraphicsItem* item : items) {
        CellItem* otherCell = dynamic_cast<CellItem*>(item);
        if (otherCell && otherCell != this && isOverlapping(otherCell)) {
            m_overlappingItems.append(otherCell);
            m_overlapAreas.append(getOverlapArea(otherCell));
        }
    }
    
    update();  // 触发重绘
}
