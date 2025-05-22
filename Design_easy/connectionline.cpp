#include "connectionline.h"
#include <QPen>
#include <QGraphicsScene>
#include <QDebug>

ConnectionLine::ConnectionLine(CellItem* startItem, const CellItem::Connector& startConnector,
                               CellItem* endItem, const CellItem::Connector& endConnector,
                               QGraphicsItem* parent)
    : QGraphicsLineItem(parent),
      m_startItem(startItem),
      m_startConnector(startConnector),
      m_endItem(endItem),
      m_endConnector(endConnector)
{
    setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    updatePosition();
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
    
    qDebug() << "ConnectionLine updated: from" << startPos << "to" << endPos;
}
