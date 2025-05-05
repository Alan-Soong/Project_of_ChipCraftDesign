#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#pragma once

#include <QGraphicsView>
#include <QKeyEvent>
#include <QWheelEvent>

class CanvasView : public QGraphicsView {
    Q_OBJECT

public:
    explicit CanvasView(QWidget *parent = nullptr);
    ~CanvasView();

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
    QPoint m_lastMousePos;
    QGraphicsScene *m_scene;
};
#endif // CANVASVIEW_H
