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
    if (!m_startItem || !m_endItem) return;

    // 将连接点的局部坐标转换为场景坐标
    QPointF startPos = m_startItem->mapToScene(m_startConnector.pos);
    QPointF endPos = m_endItem->mapToScene(m_endConnector.pos);
    setLine(QLineF(startPos, endPos));
}
