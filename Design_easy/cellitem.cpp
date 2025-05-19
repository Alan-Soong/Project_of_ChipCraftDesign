#include "cellitem.h"
// #include "settingdialog.h"
#include "dialogs.h"

#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QGraphicsView> // 包含此头文件以完成 QGraphicsView 类型的定义

CellItem::CellItem(QGraphicsItem *parent)
    : QGraphicsItem(parent), m_size(100, 100), m_resizeEdge(None), m_isDragging(false)  // 默认大小为 100x100
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    setCursor(QCursor(Qt::OpenHandCursor));
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

    // 现有绘制代码,加上可以修改大小的点
    if (isSelected()) {
        painter->setBrush(Qt::blue);
        painter->drawRect(QRectF(0, 0, cornerSize, cornerSize)); // 左上
        painter->drawRect(QRectF(m_size.width() - cornerSize, 0, cornerSize, cornerSize)); // 右上
        painter->drawRect(QRectF(0, m_size.height() - cornerSize, cornerSize, cornerSize)); // 左下
        painter->drawRect(QRectF(m_size.width() - cornerSize, m_size.height() - cornerSize, cornerSize, cornerSize)); // 右下
    }
}

// void CellItem::setSize(const QSizeF& size)
// {
//     prepareGeometryChange();
//     m_size = size;
//     if (m_pinItems.size() != m_connectors.size()) {
//         qWarning() << "m_pinItems and m_connectors out of sync in setSize: pinItems=" << m_pinItems.size()
//         << ", connectors=" << m_connectors.size();
//     }
//     int count = qMin(m_pinItems.size(), m_connectors.size());
//     for (int i = 0; i < count; ++i) {
//         if (i >= m_pinItems.size() || i >= m_connectors.size()) {
//             qWarning() << "Index out of range in setSize: i=" << i << ", pinItems=" << m_pinItems.size()
//             << ", connectors=" << m_connectors.size();
//             break;
//         }
//         PinItem* pinItem = m_pinItems[i];
//         Connector& conn = m_connectors[i];
//         if (!pinItem) {
//             qWarning() << "Null pinItem at index" << i << "in setSize";
//             continue;
//         }
//         qreal x = qMax(0.0, qMin(conn.pos.x(), m_size.width() - pinItem->boundingRect().width()));
//         qreal y = qMax(0.0, qMin(conn.pos.y(), m_size.height() - pinItem->boundingRect().height()));
//         conn.pos = QPointF(x, y);
//         pinItem->setPos(x, y);
//     }
//     update();
//     qDebug() << "setSize completed: size=" << m_size << ", pinItems=" << m_pinItems.size()
//              << ", connectors=" << m_connectors.size();
// }

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
    qreal x = 0, y = 0;
    if (side == "top") {
        x = cellSize.width() * percentage / 100.0 - pinSize / 2;
        y = 0;
    } else if (side == "bottom") {
        x = cellSize.width() * percentage / 100.0 - pinSize / 2;
        y = cellSize.height() - pinSize;
    } else if (side == "left") {
        x = 0;
        y = cellSize.height() * percentage / 100.0 - pinSize / 2;
    } else if (side == "right") {
        x = cellSize.width() - pinSize;
        y = cellSize.height() * percentage / 100.0 - pinSize / 2;
    }
    return QPointF(x, y);
}

void CellItem::addConnector(const QString& side, qreal percentage, qreal size, const QString& id, qreal x, qreal y) {
    if (!scene()) {
        qWarning() << "No scene available, cannot add connector:" << id;
        return;
    }
    // percentage = qBound(0.0, percentage, 100.0);
    // Connector connector(side, percentage, id, x, y);
    // m_connectors.append(connector);
    // PinItem* pinItem = new PinItem(this, size, this);
    // pinItem->setBrush(Qt::darkBlue);
    // pinItem->setPen(Qt::NoPen);
    // QPointF pos = connector.calculatePos(m_size, size);
    // pinItem->setPos(x, y);
    // m_pinItems.append(pinItem);
    // scene()->addItem(pinItem);
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
        // QRectF connRect(conn.pos.x() - connectorSize / 2, conn.pos.y() - connectorSize / 2,
                        // connectorSize, connectorSize);
        QRectF connRect(connPos.x(), connPos.y(), connectorSize, connectorSize);
        if (connRect.contains(pos)) {
            connector = conn;
            return true;
        }
    }
    return false;
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

void CellItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event || !scene()) {
        qDebug() << "无效的事件或场景";
        return;
    }
    qDebug() << "图元：鼠标按下位置:" << event->scenePos();

    if (event->button() == Qt::LeftButton && isSelected()) {
        // 检测是否在连接点上
        // Connector connector(QPointF(), "");
        Connector connector; // 使用默认构造函数
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
    // prepareGeometryChange();
    // m_size = size;

    // 限制位置在场景边界内
    QRectF sceneRect = scene()->sceneRect();
    if (sceneRect.isEmpty()) {
        qDebug() << "场景矩形无效";
        return;
    }
    pos.setX(qBound(sceneRect.left(), pos.x(), sceneRect.right() - size.width()));
    pos.setY(qBound(sceneRect.top(), pos.y(), sceneRect.bottom() - size.height()));
    // setPos(pos);
    // update();

    // 合并几何更新
    prepareGeometryChange();
    m_size = size;
    setPos(pos);
    update();
}

// 处理鼠标移动事件，支持拖动、调整大小和连线
void CellItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
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
            QPointF scenePos = event->scenePos();
            QPointF localPos = mapFromScene(scenePos);
            QSizeF newSize = m_size;
            QPointF newPos = pos();

            switch (m_resizeEdge) {
            case Left: // 调整左边，保持右边固定
                newSize.setWidth(rect.width() - localPos.x());
                if (newSize.width() > 20) {
                    newPos.setX(scenePos.x());
                }
                break;
            case Right: // 调整右边，保持左边固定
                newSize.setWidth(localPos.x());
                break;
            case Top: // 调整上边，保持下边固定
                newSize.setHeight(rect.height() - localPos.y());
                if (newSize.height() > 20) {
                    newPos.setY(scenePos.y());
                }
                break;
            case Bottom: // 调整下边，保持上边固定
                newSize.setHeight(localPos.y());
                break;
            case TopLeft: // 调整左上角，保持右下角固定
                newSize.setWidth(rect.width() - localPos.x());
                newSize.setHeight(rect.height() - localPos.y());
                if (newSize.width() > 20 && newSize.height() > 20) {
                    newPos.setX(scenePos.x());
                    newPos.setY(scenePos.y());
                }
                break;
            case TopRight: // 调整右上角，保持左下角固定
                newSize.setWidth(localPos.x());
                newSize.setHeight(rect.height() - localPos.y());
                if (newSize.height() > 20) {
                    newPos.setY(scenePos.y());
                }
                break;
            case BottomLeft: // 调整左下角，保持右上角固定
                newSize.setWidth(rect.width() - localPos.x());
                newSize.setHeight(localPos.y());
                if (newSize.width() > 20) {
                    newPos.setX(scenePos.x());
                }
                break;
            case BottomRight: // 调整右下角，保持左上角固定
                newSize.setWidth(localPos.x());
                newSize.setHeight(localPos.y());
                break;
            default:
                break;
            }

            restrictSizeAndPosition(newSize, newPos);
        } else {
            // 拖动整个矩形
            QPointF newPos = event->scenePos() - m_dragOffset;
            restrictSizeAndPosition(m_size, newPos);
        }
        event->accept();
    }
    QGraphicsItem::mouseMoveEvent(event);
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
    // // 尝试从场景的视图获取 QWidget*
    // QWidget* parentWidget = nullptr;
    // if (scene()) {
    //     QList<QGraphicsView*> views = scene()->views();
    //     if (!views.isEmpty()) {
    //         parentWidget = static_cast<QWidget*>(views.first());
    //     }
    // }

    // // 如果仍然没有有效的 QWidget*，使用 nullptr
    // if (parentWidget) {
    //     // 使用获取到的父窗口创建并显示设置对话框
    //     Dialogs dialog(this, parentWidget);
    //     dialog.exec();
    // } else {
    //     // 如果没有有效的父窗口，也可以直接显示对话框
    //     Dialogs dialog(nullptr);
    //     dialog.exec();
    // }
    Dialogs dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        const ComponentInfo& info = dialog.currentInfo;
        setSize(QSizeF(info.width, info.height));
        // 不再调用 loadFromFile 或 saveToFile
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
    // if (index < 0 || index >= m_connectors.size() || index >= m_pinItems.size()) {
    //     qWarning() << "Invalid index for updateConnector: index=" << index
    //                << ", connectors=" << m_connectors.size()
    //                << ", pinItems=" << m_pinItems.size();
    //     return;
    // }

    // m_connectors[index].side = side;
    // m_connectors[index].percentage = percentage;

    // PinItem* pinItem = m_pinItems[index];
    // if (!pinItem) {
    //     qWarning() << "Null pinItem at index" << index;
    //     return;
    // }

    // // 更新 PinItem 位置
    // QPointF newPos = m_connectors[index].calculatePos(m_size, connectorSize);
    // pinItem->setPos(newPos);

    // qDebug() << "Updated connector: index=" << index << ", side=" << side
    //          << ", percentage=" << percentage << ", pos=" << newPos;
    // update();
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
