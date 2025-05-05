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
#include "cellitem.h"

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

    // void saveSnapshot();

    void addCellItem(CellItem *item);
    void setSelectionMode(bool enabled);
    void deleteSelectedItems();
    void undoAction();
    void redoAction();
    void zoomInButton();
    void zoomOutButton();

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
    QGraphicsLineItem* m_tempLine; // 临时连线
    QUndoStack *undoStack;
    QVector<CellItem*> getAllCells() const;

    // 网格相关属性
    int m_gridSize = 20;  // 默认网格大小
    bool m_gridVisible = true;  // 网格是否可见
    QColor m_gridColor = Qt::lightGray;  // 网格颜色
    bool m_gridSnap = true;  // 是否启用网格对齐
    int m_majorGridSpacing = 5;  // 主网格间距（多少个网格画一条粗线）


    // 缩放相关属性
    qreal m_zoomFactor = 1.0;  // 当前缩放因子
    qreal m_minZoom = 0.1;     // 最小缩放比例
    qreal m_maxZoom = 10.0;    // 最大缩放比例
    qreal m_zoomStep = 0.1;    // 缩放步长

    // 标尺相关属性
    bool m_rulerVisible = true;  // 标尺是否可见
    QColor m_rulerColor = Qt::black;  // 标尺颜色
    int m_rulerSize = 20;  // 标尺大小（像素）
    int m_rulerTickSize = 5;  // 标尺刻度大小
    int m_rulerTextOffset = 5;  // 标尺文字偏移
};
#endif // CANVASSCENE_H
