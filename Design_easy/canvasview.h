#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#pragma once

#include <QGraphicsView>
#include <QKeyEvent>
#include <QWheelEvent>

class QGraphicsScene;

class CanvasView : public QGraphicsView {
    Q_OBJECT

public:
    explicit CanvasView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~CanvasView();
    void setGridSize(int size);  // 设置网格大小
    void setGridVisible(bool visible);  // 设置网格可见性
    void setGridColor(const QColor &color);  // 设置网格颜色
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    bool m_shiftHeld = false;
    bool m_leftMouseButtonPressed = false;
    int gridSize = 100;  // 默认网格大小
    bool gridVisible = true;  // 网格是否可见
    QColor gridColor = Qt::lightGray;  // 网格颜色
    QPoint m_lastMousePos;
    QGraphicsScene *m_scene;
};
#endif // CANVASVIEW_H
