#include "canvasview.h"
#include "canvasscene.h"
#include <QPainter>
#include <QScrollBar>
#include <QDebug>

CanvasView::CanvasView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), m_scene(scene)
{
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setBackgroundBrush(QBrush(Qt::white));
}

CanvasView::~CanvasView()
{
}

void CanvasView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift) {
        m_shiftHeld = true;
        setDragMode(QGraphicsView::ScrollHandDrag);
    }
    QGraphicsView::keyPressEvent(event);
}

void CanvasView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift) {
        m_shiftHeld = false;
        setDragMode(QGraphicsView::RubberBandDrag);
    }
    QGraphicsView::keyReleaseEvent(event);
}

void CanvasView::wheelEvent(QWheelEvent *event)
{
    if (m_shiftHeld) {
        // 水平滚动
        QScrollBar *scrollBar = horizontalScrollBar();
        scrollBar->setValue(scrollBar->value() - event->angleDelta().y());
    } else if (event->modifiers() & Qt::ControlModifier) {
        // 缩放
        const qreal scaleFactor = 1.1;
        if (event->angleDelta().y() > 0) {
            scale(scaleFactor, scaleFactor);
        } else {
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    } else {
        // 垂直滚动
        QGraphicsView::wheelEvent(event);
    }
}

void CanvasView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_leftMouseButtonPressed = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_leftMouseButtonPressed = false;
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_leftMouseButtonPressed) {
        QPoint delta = event->pos() - m_lastMousePos;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        m_lastMousePos = event->pos();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void CanvasView::setGridSize(int size)
{
    gridSize = size;
    update();
}

void CanvasView::setGridVisible(bool visible)
{
    gridVisible = visible;
    update();
}

void CanvasView::setGridColor(const QColor &color)
{
    gridColor = color;
    update();
}

void CanvasView::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);

    if (!gridVisible) return;
    // 绘制网格
    const QRectF sceneRect = this->sceneRect();
    
    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);
    
    QVarLengthArray<QLineF, 100> lines;
    
    for (qreal x = left; x < rect.right(); x += gridSize) {
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    }
    
    for (qreal y = top; y < rect.bottom(); y += gridSize) {
        lines.append(QLineF(rect.left(), y, rect.right(), y));
    }
    
    painter->setPen(QPen(QColor(200, 200, 200), 0));
    painter->drawLines(lines.data(), lines.size());
}
