#ifndef CANVASSCENE_H
#define CANVASSCENE_H

#pragma once

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QList>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QGraphicsView>
#include <QUndoStack>
#include <QMap>
#include "cellitem_new.h"
#include "connectionline.h"

struct CellSnapshot {
    QPointF pos;
    QSizeF size;
};

class CanvasScene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit CanvasScene(QObject *parent = nullptr);
    ~CanvasScene() override;

    void saveSnapshot();
    void setGridSize(int size);  // 设置网格大小
    void setGridVisible(bool visible);  // 设置网格可见性
    void setGridColor(const QColor &color);  // 设置网格颜色
    void setGridSnap(bool enabled);  // 设置网格对齐
    void zoomIn();  // 放大
    void zoomOut();  // 缩小
    void setZoomFactor(qreal factor);  // 设置缩放因子
    void setRulerVisible(bool visible);  // 设置标尺可见性
    void setRulerColor(const QColor &color);  // 设置标尺颜色
    void set_unit(QString to_unit);//设置单位
    QString get_unit() const; // 获取当前单位

    // 撤销系统
    void setUndoStack(QUndoStack* undoStack) { this->undoStack = undoStack; }
    QUndoStack* getUndoStack() const { return undoStack; }

    void addCellItem(CellItem *item);
    void addConnectionLine(ConnectionLine *line); // 添加连线的方法
    void removeConnectionLine(ConnectionLine *line); // 删除连线的方法
    void setSelectionMode(bool enabled);
    void deleteSelectedItems();
    void undoAction();
    void redoAction();
    void zoomInButton();
    void zoomOutButton();

    // 连线相关方法
    void startConnection(CellItem* startItem, const QString& startPinId);
    void finishConnection(CellItem* endItem, const QString& endPinId);
    void cancelConnection();
    QList<ConnectionLine*> getConnectionLines() const;

    // 命名线网（从文件加载）支持
    struct NetInfo {
        QString name;                                      // 线网名
        QList<QPair<CellItem*, QString>> pins;             // (芯片, 引脚ID)
        QList<ConnectionLine*> lines;                      // 属于该线网的连线
    };
    void resetNamedNets();                                 // 清空所有命名线网数据
    void addParsedNet(const QString& netName,
                      const QList<QPair<CellItem*, QString>>& pinRefs,
                      const QList<ConnectionLine*>& linesInNet); // 添加解析到的线网
    bool hasNamedNets() const { return !m_namedNets.isEmpty(); }
    QMap<QString, NetInfo> getNamedNets() const { return m_namedNets; }
    void removeLineFromNamedNets(ConnectionLine* line);    // 当连线被删除时同步清理
    void setLoading(bool loading) { m_loading = loading; }
    bool isLoading() const { return m_loading; }

    // 获取所有芯片项
    QList<CellItem*> getAllCellItems() const;

    // 添加新方法声明
    void updateOverlapStates();  // 更新所有矩形的重合状态
    bool isGroupMoving() const { return m_isGroupMoving; }  // 检查是否在组移动模式
    void fitToWindow();  // 自动适应视图到所有内容

signals:
    // 当芯片或连线结构发生变化（新增/删除/连接变动）时发出，用于重建线网视图
    void netTopologyChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QGraphicsSceneWheelEvent *event) override;  // 添加滚轮事件处理
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;  // 添加前景绘制方法

private:
    bool m_leftMouseButtonPressed = false;
    bool m_selectionModeEnabled = false;
    QPoint m_lastMousePos;
    QPointF m_dragOffset; // 添加成员变量
    QGraphicsLineItem* m_tempLine = nullptr; // 临时连线
    QUndoStack *undoStack;

    // 连线相关属性
    CellItem* m_connectionStartItem = nullptr;
    QString m_connectionStartPinId;
    QList<ConnectionLine*> m_connectionLines;

    // 组移动相关属性
    bool m_isGroupMoving = false;
    QPointF m_groupMoveStartPos;
    QMap<CellItem*, QPointF> m_groupMoveInitialPositions;

    // 查找引脚
    PinItem* findPinItemAt(const QPointF& scenePos, CellItem** outCellItem = nullptr, QString* outPinId = nullptr);

    // 更新所有连线位置
    void updateAllConnectionLines();

    // 网格相关属性
    int m_gridSize = 20;  // 默认网格大小
    bool m_gridVisible = false;  // 网格不可见，使用CanvasView的固定像素网格
    QColor m_gridColor = Qt::lightGray;  // 网格颜色
    bool m_gridSnap = true;  // 是否启用网格对齐
    int m_majorGridSpacing = 5;  // 主网格间距（多少个网格画一条粗线）
    QString to_unit="cm";

    // 缩放相关属性
    qreal m_zoomFactor = 1.0;  // 当前缩放因子
    qreal m_minZoom = 0.1;     // 最小缩放比例
    qreal m_maxZoom = 10.0;    // 最大缩放比例
    qreal m_zoomStep = 0.1;    // 缩放步长

    // 标尺相关属性
    bool m_rulerVisible = true;  // 标尺是否可见
    QColor m_rulerColor = Qt::black;  // 标尺颜色
    int m_rulerSize = 20;  // 标尺大小（像素），再调小一点
    int m_rulerTickSize = 5;  // 标尺刻度大小，再调小一点
    int m_rulerTextOffset = 5;  // 标尺文字偏移，再调小一点

    // 命名线网数据（仅当从文件加载时填充）
    QMap<QString, NetInfo> m_namedNets;                    // key = 线网名
    bool m_loading = false;                                // 文件加载阶段标志
};
#endif // CANVASSCENE_H
