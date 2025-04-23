#ifndef CELLITEM_H
#define CELLITEM_H

#pragma once

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>

class CellItem : public QGraphicsItem
{
public:
    explicit CellItem(QGraphicsItem *parent = nullptr);

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

    // 连接点接口
    struct Connector {
        QPointF pos; // 连接点相对于图元的局部坐标
        QString id; // 唯一标识（如 "top", "bottom"）
        Connector() = default; // 添加默认构造函数
        Connector(const QPointF& p, const QString& i) : pos(p), id(i) {}
    };
    QList<Connector> getConnectors() const; // 获取所有连接点
    bool isOnConnector(const QPointF& pos, Connector& connector) const; // 检测是否点击连接点

private:
    enum ResizeEdge { None, Top, Bottom, Left, Right,
                      TopLeft, TopRight, BottomLeft, BottomRight // 角点
    };
    QSizeF m_size;
    QPointF m_dragOffset;
    ResizeEdge m_resizeEdge; // 当前调整的边
    // bool isOnEdge(const QPointF &pos, ResizeEdge &edge) const; // 检测是否在边框上
    bool isOnEdgeOrCorner(const QPointF &pos, ResizeEdge &edge) const; // 检测边框或角点
    void restrictSizeAndPosition(QSizeF& size, QPointF& pos);
    static constexpr qreal edgeWidth = 5.0; // 边框检测宽度
    static constexpr qreal cornerSize = 5.0; // 角点检测区域大小
    static constexpr qreal connectorSize = 5.0; // 连接点检测区域大小
};

// 注册 Connector 到 Qt 元对象系统
Q_DECLARE_METATYPE(CellItem::Connector)

#endif // CELLITEM_H
