#include "pinitem.h"
#include "cellitem_new.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>

PinItem::PinItem(QGraphicsItem* parentRect, qreal size, QGraphicsItem* parent)
    : QGraphicsEllipseItem(0, 0, size, size, parent)
    , parentRect(parentRect)
    , m_percentage(0.0)
    , m_x(0.0)
    , m_y(0.0)
{
    // 引脚不应该可移动或可选择，只能通过对话框编辑
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    // 启用悬停事件，提供连线提示
    setAcceptHoverEvents(true);

    // 设置蓝色的默认样式
    setPen(QPen(Qt::darkBlue, 1));
    setBrush(Qt::blue);

    // 确保不被选中
    setSelected(false);
}

QVariant PinItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && parentRect) {
        // 引脚不应该通过拖动改变位置，只能通过对话框编辑
        // 返回当前位置，阻止位置变化
        return pos();
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
    // 引脚不应该可拖动，忽略鼠标移动事件
    Q_UNUSED(event);
    // 不调用基类的处理，防止拖动
}

void PinItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // 引脚不应该可拖动，但仍然可以用于连线
    Q_UNUSED(event);
    // 可以在这里处理连线相关的逻辑，但不处理拖动
}

void PinItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    // 鼠标悬停时高亮显示，表示可以用于连线
    setPen(QPen(Qt::cyan, 2));
    setBrush(Qt::cyan);
    setCursor(Qt::CrossCursor); // 设置十字光标，表示可用于连线
}

void PinItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    Q_UNUSED(event);
    // 鼠标离开时恢复默认蓝色样式
    setPen(QPen(Qt::darkBlue, 1));
    setBrush(Qt::blue);
    setCursor(Qt::ArrowCursor); // 恢复默认光标
}
