#include "pinitem.h"
#include "cellitem_new.h"
#include <QDebug>
#include <QGraphicsScene>

PinItem::PinItem(QGraphicsItem* parentRect, qreal size, QGraphicsItem* parent)
    : QGraphicsEllipseItem(0, 0, size, size, parent)
    , parentRect(parentRect)
    , m_percentage(0.0)
    , m_x(0.0)
    , m_y(0.0)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setPen(QPen(Qt::blue, 2));
    setBrush(QColorConstants::Svg::lightblue);
}

QVariant PinItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && parentRect) {
        QPointF newPos = value.toPointF();
        QRectF parentBounds = parentRect->boundingRect();

        QString side;
        qreal percentage;
        QPointF restrictedPos = restrictToEdge(newPos, parentBounds.width(),
                                               parentBounds.height(), side, percentage);

        m_side = side;
        m_percentage = percentage;
        m_x = restrictedPos.x();
        m_y = restrictedPos.y();

        return restrictedPos;
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}

void PinItem::updateConnector(const QString& id, qreal x, qreal y)
{
    m_id = id;
    m_x = x;
    m_y = y;
    setPos(x, y);
}

void PinItem::updateConnector(const QString& id, qreal x, qreal y, const QString& side, qreal percentage)
{
    m_id = id;
    m_x = x;
    m_y = y;
    m_side = side;
    m_percentage = percentage;
    setPos(x, y);
}


QPointF PinItem::restrictToEdge(const QPointF& pos, qreal width, qreal height,
                                QString& side, qreal& percentage) const
{
    qreal x = pos.x();
    qreal y = pos.y();
    qreal edgeThreshold = 10.0; // 边缘吸附阈值

    // 计算到各边的距离
    qreal distToTop = qAbs(y);
    qreal distToBottom = qAbs(y - height);
    qreal distToLeft = qAbs(x);
    qreal distToRight = qAbs(x - width);

    // 找到最近的边
    qreal minDist = qMin(qMin(distToTop, distToBottom), qMin(distToLeft, distToRight));

    if (minDist <= edgeThreshold) {
        if (minDist == distToTop) {
            side = "top";
            percentage = (x / width) * 100.0;
            return QPointF(x, 0);
        } else if (minDist == distToBottom) {
            side = "bottom";
            percentage = (x / width) * 100.0;
            return QPointF(x, height);
        } else if (minDist == distToLeft) {
            side = "left";
            percentage = (y / height) * 100.0;
            return QPointF(0, y);
        } else {
            side = "right";
            percentage = (y / height) * 100.0;
            return QPointF(width, y);
        }
    }

    // 如果不在边缘附近，允许自由位置
    side = "custom";
    percentage = 0.0;
    return pos;
}

void PinItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void PinItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsEllipseItem::mouseReleaseEvent(event);

    // 通知父对象引脚位置已更改
    if (CellItem* cellItem = qgraphicsitem_cast<CellItem*>(parentRect)) {
        // 这里可以添加通知逻辑
        qDebug() << "Pin" << m_id << "moved to" << pos() << "side:" << m_side << "percentage:" << m_percentage;
    }
}
