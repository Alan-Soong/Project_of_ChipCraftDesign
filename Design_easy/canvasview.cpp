#include "canvasview.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPainter>

CanvasView::CanvasView(QWidget *parent)
    : QGraphicsView(parent), m_scene(scene()) {
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

CanvasView::~CanvasView() {}

void CanvasView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift) {
        m_shiftHeld = true;
    }
    QGraphicsView::keyPressEvent(event);
}

void CanvasView::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift) {
        m_shiftHeld = false;
    }
    QGraphicsView::keyReleaseEvent(event);
}

void CanvasView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        // Zoom in/out
        qreal factor = 1.1;
        if (event->angleDelta().y() < 0) factor = 1.0 / factor;
        scale(factor, factor);
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void CanvasView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_leftMouseButtonPressed = true;
        m_lastMousePos = event->pos();
    }
    QGraphicsView::mousePressEvent(event);
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_leftMouseButtonPressed = false;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::mouseMoveEvent(QMouseEvent *event) {
    if (m_leftMouseButtonPressed) {
        QPointF delta = mapToScene(event->pos()) - mapToScene(m_lastMousePos);
        m_lastMousePos = event->pos();
        m_scene->setSceneRect(m_scene->sceneRect().translated(delta.toPoint()));
    }
    QGraphicsView::mouseMoveEvent(event);
}

void CanvasView::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsView::drawBackground(painter, rect);
    // Custom background drawing logic (e.g., grid lines, etc.)
}
