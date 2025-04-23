#include "cellitem.h"
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

CellItem::CellItem(QGraphicsItem *parent)
    : QGraphicsItem(parent), m_size(100, 100)  // 默认大小为 100x100
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

        // for (const Connector& connector : getConnectors()) {
        //     painter->drawEllipse(connector.pos, connectorSize, connectorSize);
        // }
    }
}

void CellItem::setSize(const QSizeF& size)
{
    prepareGeometryChange(); // 通知场景几何变化
    m_size = size;
    update();  // 更新图形
}

QSizeF CellItem::size() const
{
    return m_size;
}

QList<CellItem::Connector> CellItem::getConnectors() const
{
    QList<Connector> connectors;
    QRectF rect = boundingRect();
    // 四条边的中点
    connectors << Connector(QPointF(rect.width() / 2, 0), "top");
    connectors << Connector(QPointF(rect.width() / 2, rect.height()), "bottom");
    connectors << Connector(QPointF(0, rect.height() / 2), "left");
    connectors << Connector(QPointF(rect.width(), rect.height() / 2), "right");
    return connectors;
}

bool CellItem::isOnConnector(const QPointF& pos, Connector& connector) const
{
    for (const Connector& c : getConnectors()) {
        if (QLineF(pos, c.pos).length() <= connectorSize) {
            connector = c;
            return true;
        }
    }
    return false;
}

/*
bool CellItem::isOnEdge(const QPointF &pos, ResizeEdge &edge) const
{
    QRectF rect = boundingRect();
    qreal x = pos.x();
    qreal y = pos.y();
    edge = None;

    // 检测是否在边框区域（5 像素宽）
    if (x >= -edgeWidth && x <= edgeWidth) {
        edge = Left;
    } else if (x >= rect.width() - edgeWidth && x <= rect.width() + edgeWidth) {
        edge = Right;
    } else if (y >= -edgeWidth && y <= edgeWidth) {
        edge = Top;
    } else if (y >= rect.height() - edgeWidth && y <= rect.height() + edgeWidth) {
        edge = Bottom;
    }

    return edge != None;
}*/
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
    /*if (event->button() == Qt::LeftButton && isSelected()) {
        // 检测是否在边框上
        ResizeEdge edge;
        if (isOnEdge(event->pos(), edge)) {
            m_resizeEdge = edge;
            setCursor(edge == Left || edge == Right ? QCursor(Qt::SizeHorCursor) :
                          QCursor(Qt::SizeVerCursor));
        } else {
            m_resizeEdge = None;
            m_dragOffset = event->scenePos() - pos();
            setCursor(QCursor(Qt::ClosedHandCursor));
        }
        update();
        event->accept();
    }
    if (event->button() == Qt::LeftButton && isSelected()) {
        // 检测是否在边框或角点上
        ResizeEdge edge;
        if (isOnEdgeOrCorner(event->pos(), edge)) {
            m_resizeEdge = edge;
            // 设置光标
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
        update();
        event->accept();
    }
    */
    if (event->button() == Qt::LeftButton && isSelected()) {
        // 检测是否在连接点上
        Connector connector(QPointF(), "");
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

/*
void CellItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event || !scene()) {
        qDebug() << "无效的事件或场景";
        return;
    }

    if (event->buttons() & Qt::LeftButton && isSelected()) {
        if (m_resizeEdge != None) {
            // 调整大小
            QRectF rect = boundingRect();
            QPointF scenePos = event->scenePos();
            QPointF localPos = mapFromScene(scenePos);
            QSizeF newSize = m_size;
            QPointF newPos = pos();

            switch (m_resizeEdge) {
            // 边框调整
            case Left:
                newSize.setWidth(rect.width() - localPos.x());
                if (newSize.width() > 20) {
                    newPos.setX(scenePos.x());
                }
                break;
            case Right:
                newSize.setWidth(localPos.x());
                break;
            case Top:
                newSize.setHeight(rect.height() - localPos.y());
                if (newSize.height() > 20) {
                    newPos.setY(scenePos.y());
                }
                break;
            case Bottom:
                newSize.setHeight(localPos.y());
                break;
            // 角点调整
            case TopLeft:
                newSize.setWidth(rect.width() - localPos.x());
                newSize.setHeight(rect.height() - localPos.y());
                if (newSize.width() > 20 && newSize.height() > 20) {
                    newPos.setX(scenePos.x());
                    newPos.setY(scenePos.y());
                }
                break;
            case TopRight:
                newSize.setWidth(localPos.x());
                newSize.setHeight(rect.height() - localPos.y());
                if (newSize.height() > 20) {
                    newPos.setY(scenePos.y());
                }
                break;
            case BottomLeft:
                newSize.setWidth(rect.width() - localPos.x());
                newSize.setHeight(localPos.y());
                if (newSize.width() > 20) {
                    newPos.setX(scenePos.x());
                }
                break;
            case BottomRight:
                newSize.setWidth(localPos.x());
                newSize.setHeight(localPos.y());
                break;
            default:
                break;
            }

            // 确保尺寸不小于最小值
            newSize.setWidth(qMax(20.0, newSize.width()));
            newSize.setHeight(qMax(20.0, newSize.height()));
            setSize(newSize);

            // 限制位置在场景边界内
            QRectF sceneRect = scene()->sceneRect();
            newPos.setX(qBound(sceneRect.left(), newPos.x(), sceneRect.right() - newSize.width()));
            newPos.setY(qBound(sceneRect.top(), newPos.y(), sceneRect.bottom() - newSize.height()));
            setPos(newPos);
        }
        else {
            // 拖动
            QPointF newPos = event->scenePos() - m_dragOffset;
            QRectF sceneRect = scene()->sceneRect();
            newPos.setX(qBound(sceneRect.left(), newPos.x(), sceneRect.right() - m_size.width()));
            newPos.setY(qBound(sceneRect.top(), newPos.y(), sceneRect.bottom() - m_size.height()));
            setPos(newPos);
        }
        event->accept();
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void CellItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event || !scene()) {
        qDebug() << "无效的事件或场景";
        return;
    }
    qDebug() << "图元：鼠标释放位置:" << event->scenePos();
    m_resizeEdge = None;
    setCursor(QCursor(Qt::OpenHandCursor));
    m_dragOffset = QPointF(0, 0);
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}*/

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
