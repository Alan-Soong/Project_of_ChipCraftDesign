#ifndef CELLITEM_H
#define CELLITEM_H

#pragma once
#include "dialogs.h"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QMessageBox>

// class PinItem : public QGraphicsRectItem {
// public:
//     PinItem(QGraphicsItem* parentRect, qreal size, QGraphicsItem* parent = nullptr);
// private:
//     QGraphicsItem* parentRect;
// };
class Dialogs;
class PinItem;

class CellItem : public QGraphicsItem
{
public:
    explicit CellItem(QGraphicsItem *parent = nullptr);
    enum ResizeEdge { None, TopLeft, TopRight, BottomLeft, BottomRight, Top, Bottom, Left, Right };
    // 重写 boundingRect 来定义矩形的范围
    QRectF boundingRect() const override;

    // 重写 paint 方法来绘制矩形
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // 设置单元格的尺寸
    void setSize(const QSizeF& size);

    // 获取单元格的尺寸
    QSizeF size() const;

    // 鼠标事件（用于交互）
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;  // 添加双击事件

    // 连接点接口
    struct Connector {
        QString side; // "top", "bottom", "left", "right"
        qreal percentage; // 0.0 to 100.0
        QString id;
        qreal x;
        qreal y;
        Connector() = default;
        Connector(const QString& s, qreal p, const QString& i, qreal xPos = 0, qreal yPos = 0) : side(s), percentage(p), id(i), x(xPos), y(yPos) {}
        QPointF calculatePos(const QSizeF& cellSize, qreal pinSize) const;
    };
    void updateConnector(int index, const QString& side, qreal percentage, qreal x, qreal y); // 新方法
    void addConnector(const QString& side, qreal percentage, qreal size, const QString& id, qreal x, qreal y);
    QList<Connector> getConnectors() const; // 获取所有连接点
    QList<PinItem*> getPinItems() const;    // 新增：获取 PinItem 列表
    bool isOnConnector(const QPointF& pos, Connector& connector) const; // 检测是否点击连接点

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override; // 新增：处理调整大小


private:
    // enum ResizeEdge { None, Top, Bottom, Left, Right,
    //                   TopLeft, TopRight, BottomLeft, BottomRight // 角点
    // };
    QSizeF m_size;
    QPointF m_dragOffset;
    ResizeEdge m_resizeEdge; // 当前调整的边
    bool m_isDragging;
    // bool isOnEdge(const QPointF &pos, ResizeEdge &edge) const; // 检测是否在边框上
    bool isOnEdgeOrCorner(const QPointF &pos, ResizeEdge &edge) const; // 检测边框或角点
    void restrictSizeAndPosition(QSizeF& size, QPointF& pos);
    void onDoubleClick();  // 双击事件处理
    static constexpr qreal edgeWidth = 5.0; // 边框检测宽度
    static constexpr qreal cornerSize = 5.0; // 角点检测区域大小
    static constexpr qreal connectorSize = 5.0; // 连接点检测区域大小
    // bool isOnConnector(const QPointF& pos, Connector& connector) const;

    QList<Connector> m_connectors; // 存储连接点
    QList<PinItem*> m_pinItems;    // 存储引脚图形项


};

// 注册 Connector 到 Qt 元对象系统
Q_DECLARE_METATYPE(CellItem::Connector)



#endif // CELLITEM_H
