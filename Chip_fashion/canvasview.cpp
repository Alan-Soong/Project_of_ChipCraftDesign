#include "canvasview.h"
#include "connectionline.h"
#include <QPainter>
#include <QScrollBar>
#include <QDebug>
#include <QTransform>
#include <QtMath>

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
        // 缩放后更新所有连线的线宽
        updateAllConnectionLineWidths();
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

// void CanvasView::drawBackground(QPainter *painter, const QRectF &rect)
// {
//     QGraphicsView::drawBackground(painter, rect);

//     // 绘制固定像素间距的网格
//     // 获取当前变换的缩放因子
//     QTransform t = transform();
//     qreal currentScale = qSqrt(t.m11() * t.m11() + t.m12() * t.m12());

//     // 固定的屏幕像素间距
//     const qreal smallGridPixels = 20.0;  // 小网格20像素间距
//     const qreal largeGridPixels = 100.0; // 大网格100像素间距

//     // 转换为场景坐标系中的间距（这个会随缩放变化）
//     qreal smallGridSize = smallGridPixels / currentScale;
//     qreal largeGridSize = largeGridPixels / currentScale;

//     // 绘制小网格
//     qreal left = qFloor(rect.left() / smallGridSize) * smallGridSize;
//     qreal top = qFloor(rect.top() / smallGridSize) * smallGridSize;

//     QVarLengthArray<QLineF, 100> lines;

//     for (qreal x = left; x < rect.right(); x += smallGridSize) {
//         lines.append(QLineF(x, rect.top(), x, rect.bottom()));
//     }

//     for (qreal y = top; y < rect.bottom(); y += smallGridSize) {
//         lines.append(QLineF(rect.left(), y, rect.right(), y));
//     }

//     // 绘制细网格线
//     painter->setPen(QPen(QColor(240, 240, 240), 0));
//     painter->drawLines(lines.data(), lines.size());

//     // 绘制大网格线
//     QVarLengthArray<QLineF, 20> majorLines;

//     qreal majorLeft = qFloor(rect.left() / largeGridSize) * largeGridSize;
//     qreal majorTop = qFloor(rect.top() / largeGridSize) * largeGridSize;

//     for (qreal x = majorLeft; x < rect.right(); x += largeGridSize) {
//         majorLines.append(QLineF(x, rect.top(), x, rect.bottom()));
//     }

//     for (qreal y = majorTop; y < rect.bottom(); y += largeGridSize) {
//         majorLines.append(QLineF(rect.left(), y, rect.right(), y));
//     }

//     painter->setPen(QPen(QColor(200, 200, 200), 0));
//     painter->drawLines(majorLines.data(), majorLines.size());
// }

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

void CanvasView::updateAllConnectionLineWidths()
{
    if (!m_scene) return;

    // 遍历场景中的所有图形项
    QList<QGraphicsItem*> items = m_scene->items();
    for (QGraphicsItem* item : items) {
        // 检查是否是 ConnectionLine 类型
        ConnectionLine* connectionLine = dynamic_cast<ConnectionLine*>(item);
        if (connectionLine) {
            connectionLine->updateLineWidth();
        }
    }
}
