#ifndef PINITEM_H
#define PINITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QVariant>

class CellItem;

/**
 * 引脚图形项 - 负责引脚的可视化显示和交互
 */
class PinItem : public QGraphicsEllipseItem
{
public:
    PinItem(QGraphicsItem* parentRect, qreal size, QGraphicsItem* parent = nullptr);

    // 引脚属性管理
    void updateConnector(const QString& id, qreal x, qreal y);
    void updateConnector(const QString& id, qreal x, qreal y, const QString& side, qreal percentage);
    QString getSide() const { return m_side; }
    qreal getPercentage() const { return m_percentage; }
    QString getId() const { return m_id; }
    qreal getX() const { return m_x; }
    qreal getY() const { return m_y; }
    void setParentRect(QGraphicsItem* parentRect) { this->parentRect = parentRect; }

    // 引脚属性设置方法
    void setSide(const QString& side) { m_side = side; }
    void setPercentage(qreal percentage) { m_percentage = percentage; }

    // 位置约束
    QPointF restrictToEdge(const QPointF& pos, qreal width, qreal height,
                           QString& side, qreal& percentage) const;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QGraphicsItem* parentRect; // 父矩形，用于边界检查
    QString m_side;
    qreal m_percentage;
    QString m_id;
    qreal m_x;
    qreal m_y;
};

#endif // PINITEM_H
