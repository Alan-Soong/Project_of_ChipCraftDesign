#include "connectionline.h"
#include "pinitem.h"
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDebug>
#include <QVariant>

ConnectionLine::ConnectionLine(CellItem* startItem, const CellItem::Connector& startConnector,
                               CellItem* endItem, const CellItem::Connector& endConnector,
                               QGraphicsItem* parent)
    : QGraphicsLineItem(parent),
    m_startItem(startItem),
    m_startConnector(startConnector),
    m_endItem(endItem),
    m_endConnector(endConnector)
{
    // 使用较细的初始线宽，后续会根据缩放调整
    setPen(QPen(Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    updatePosition();
    updateLineWidth(); // 初始化线宽
}

void ConnectionLine::updatePosition()
{
    if (!m_startItem || !m_endItem) {
        qWarning() << "ConnectionLine::updatePosition: Invalid start or end item";
        return;
    }

    // 获取起始引脚的位置
    QPointF startPos;
    QPointF endPos;

    // 查找起始引脚
    auto startPins = m_startItem->getPinItems();
    auto startConnectors = m_startItem->getConnectors();
    for (int i = 0; i < startConnectors.size() && i < startPins.size(); ++i) {
        if (startConnectors[i].id == m_startConnector.id) {
            QPointF pinCenter = startPins[i]->boundingRect().center() + startPins[i]->pos();
            startPos = m_startItem->pos() + pinCenter;
            break;
        }
    }

    // 查找结束引脚
    auto endPins = m_endItem->getPinItems();
    auto endConnectors = m_endItem->getConnectors();
    for (int i = 0; i < endConnectors.size() && i < endPins.size(); ++i) {
        if (endConnectors[i].id == m_endConnector.id) {
            QPointF pinCenter = endPins[i]->boundingRect().center() + endPins[i]->pos();
            endPos = m_endItem->pos() + pinCenter;
            break;
        }
    }

    // 更新连线位置
    setLine(QLineF(startPos, endPos));

    // qDebug() << "ConnectionLine updated: from" << startPos << "to" << endPos;
}

void ConnectionLine::updateLineWidth()
{
    // 获取当前视图的缩放因子
    QGraphicsScene* scene = this->scene();
    if (!scene) return;

    // 从场景的视图列表中获取第一个视图（通常只有一个）
    QList<QGraphicsView*> views = scene->views();
    if (views.isEmpty()) return;

    QGraphicsView* view = views.first();
    QTransform t = view->transform();
    qreal currentScale = qSqrt(t.m11() * t.m11() + t.m12() * t.m12());

    // 计算自适应线宽：基础线宽除以缩放因子，但设置最小和最大值
    qreal baseLineWidth = 0.8; // 基础线宽
    qreal adaptiveLineWidth = baseLineWidth / currentScale;

    // 限制线宽范围：最小0.2，最大3.0
    adaptiveLineWidth = qMax(0.2, qMin(adaptiveLineWidth, 3.0));

    // 更新画笔
    QPen currentPen = pen();
    currentPen.setWidthF(adaptiveLineWidth);
    setPen(currentPen);

    // qDebug() << "ConnectionLine line width updated to:" << adaptiveLineWidth << "for scale:" << currentScale;
}

QVariant ConnectionLine::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSelectedChange) {
        bool selected = value.toBool();
        QPen currentPen = pen();

        if (selected) {
            // 选中时使用红色高亮
            currentPen.setColor(Qt::red);
            currentPen.setWidthF(currentPen.widthF() + 1.0); // 稍微加粗
        } else {
            // 未选中时使用默认黑色
            currentPen.setColor(Qt::black);
            updateLineWidth(); // 恢复正常线宽
        }

        setPen(currentPen);
    }

    return QGraphicsLineItem::itemChange(change, value);
}
