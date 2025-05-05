// #ifndef CONNECTIONLINE_H
// #define CONNECTIONLINE_H

// class ConnectionLine
// {
// public:
//     ConnectionLine();
// };

// #endif // CONNECTIONLINE_H

#ifndef CONNECTIONLINE_H
#define CONNECTIONLINE_H

#include <QGraphicsLineItem>
#include "cellitem.h"

class ConnectionLine : public QGraphicsLineItem
{
public:
    ConnectionLine(CellItem* startItem, const CellItem::Connector& startConnector,
                   CellItem* endItem, const CellItem::Connector& endConnector,
                   QGraphicsItem* parent = nullptr);

    void updatePosition(); // 更新连线位置

private:
    CellItem* m_startItem;
    CellItem::Connector m_startConnector;
    CellItem* m_endItem;
    CellItem::Connector m_endConnector;
};

#endif
