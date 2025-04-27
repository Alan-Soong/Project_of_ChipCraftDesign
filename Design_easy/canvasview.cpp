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
    qDebug() << "鼠标按下ctrl";
    // 检查是否按下了 Ctrl 键
    if (event->modifiers() & Qt::ControlModifier) {
        // 获取滚轮的角度变化
        int delta = event->angleDelta().y();

        // 缩放因子，控制缩放速度
        const qreal scaleFactor = 1.2;

        // 根据滚轮的方向来缩放视图
        if (delta > 0) {
            scale(scaleFactor, scaleFactor);  // 放大
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);  // 缩小
        }

        event->accept();  // 标记事件为已处理
    } else {
        // 如果没有按下 Ctrl 键，调用基类的事件处理
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
