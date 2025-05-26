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
#include <QJsonObject>
#include <QJsonArray>
#include <QPainterPath>

class Dialogs;
class PinItem;

class CellItem : public QGraphicsItem
{
public:
    explicit CellItem(QGraphicsItem *parent = nullptr);
    ~CellItem(); // 添加析构函数确保清理资源
    
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

    void restrictSize(QSizeF& size);
    void restrictPosition(QPointF& pos);

    // 连接点接口
    struct Connector {
        QString side; // "top", "bottom", "left", "right", "custom"
        qreal percentage; // 0.0 to 100.0
        QString id;
        qreal x;
        qreal y;
        Connector() = default;
        Connector(const QString& s, qreal p, const QString& i, qreal xPos = 0, qreal yPos = 0) : side(s), percentage(p), id(i), x(xPos), y(yPos) {}
        QPointF calculatePos(const QSizeF& cellSize, qreal pinSize) const;
    };
    
    // 引脚管理方法
    void updateConnector(int index, const QString& side, qreal percentage, qreal x, qreal y);
    void addConnector(const QString& side, qreal percentage, qreal size, const QString& id, qreal x, qreal y);
    bool removeConnector(const QString& id); // 新增：删除指定ID的连接点
    bool removeConnector(int index); // 新增：删除指定索引的连接点
    QList<Connector> getConnectors() const; // 获取所有连接点
    QList<PinItem*> getPinItems() const;    // 获取 PinItem 列表
    bool isOnConnector(const QPointF& pos, Connector& connector) const; // 检测是否点击连接点
    
    // 数据持久化方法
    QJsonObject toJson() const; // 新增：将CellItem转换为JSON对象
    void fromJson(const QJsonObject& json); // 新增：从JSON对象恢复CellItem
    
    // 连线相关方法
    void addConnection(CellItem* targetCell, const QString& sourcePin, const QString& targetPin);
    QList<QPair<CellItem*, QPair<QString, QString>>> getConnections() const;
    
    // 设置和获取宏名称
    void setMacroName(const QString& name) { m_macroName = name; }
    QString getMacroName() const { return m_macroName; }
    
    // 设置和获取实例名称
    void setInstanceName(const QString& name) { m_instanceName = name; }
    QString getInstanceName() const { return m_instanceName; }

    // 添加新方法
    QPainterPath getShape() const;  // 获取矩形的形状路径
    bool isOverlapping(const CellItem* other) const;  // 检查是否与其他矩形重合
    QPainterPath getOverlapArea(const CellItem* other) const;  // 获取重合区域
    void updateOverlapState();  // 更新重合状态

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QSizeF m_size;
    QPointF m_dragOffset;
    ResizeEdge m_resizeEdge; // 当前调整的边
    bool m_isDragging;
    QString m_macroName; // 宏名称
    QString m_instanceName; // 实例名称
    
    bool isOnEdgeOrCorner(const QPointF &pos, ResizeEdge &edge) const; // 检测边框或角点
    void restrictSizeAndPosition(QSizeF& size, QPointF& pos);
    void onDoubleClick();  // 双击事件处理
    
    static constexpr qreal edgeWidth = 5.0; // 边框检测宽度
    static constexpr qreal cornerSize = 5.0; // 角点检测区域大小
    static constexpr qreal connectorSize = 5.0; // 连接点检测区域大小

    QList<Connector> m_connectors; // 存储连接点
    QList<PinItem*> m_pinItems;    // 存储引脚图形项
    
    // 存储连线信息：目标CellItem，源引脚ID，目标引脚ID
    QList<QPair<CellItem*, QPair<QString, QString>>> m_connections;

    QList<QPainterPath> m_overlapAreas;  // 存储重合区域
    QList<CellItem*> m_overlappingItems;  // 存储重合的矩形项
};

// 注册 Connector 到 Qt 元对象系统
Q_DECLARE_METATYPE(CellItem::Connector)

#endif // CELLITEM_H
