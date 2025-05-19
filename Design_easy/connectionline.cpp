// #include "connectionline.h"

// ConnectionLine::ConnectionLine() {}

#include "connectionline.h"
#include <QPen>

ConnectionLine::ConnectionLine(CellItem* startItem, const CellItem::Connector& startConnector,
                               CellItem* endItem, const CellItem::Connector& endConnector,
                               QGraphicsItem* parent)
    : QGraphicsLineItem(parent),
    m_startItem(startItem), m_startConnector(startConnector),
    m_endItem(endItem), m_endConnector(endConnector)
{
    setPen(QPen(Qt::black, 2));
    updatePosition();
}

void ConnectionLine::updatePosition()
{
    // if (!m_startItem || !m_endItem) return;

    // // 将连接点的局部坐标转换为场景坐标
    // QPointF startPos = m_startItem->mapToScene(m_startConnector.pos);
    // QPointF endPos = m_endItem->mapToScene(m_endConnector.pos);
    // setLine(QLineF(startPos, endPos));
    if (!m_startItem || !m_endItem) return;

    // 获取引脚的局部坐标（相对于 CellItem）
    const qreal pinSize = 10; // 与 CellItem 的引脚大小一致
    QPointF startLocalPos = m_startConnector.calculatePos(m_startItem->size(), pinSize);
    QPointF endLocalPos = m_endConnector.calculatePos(m_endItem->size(), pinSize);

    // 将局部坐标转换为场景坐标
    QPointF startPos = m_startItem->mapToScene(startLocalPos);
    QPointF endPos = m_endItem->mapToScene(endLocalPos);

    // 调整连线起点和终点，考虑引脚大小
    startPos += QPointF(pinSize / 2, pinSize / 2); // 移动到引脚中心
    endPos += QPointF(pinSize / 2, pinSize / 2);   // 移动到引脚中心

    setLine(QLineF(startPos, endPos));
}
