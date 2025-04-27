#include "canvasscene.h"
#include "connectionline.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

CanvasScene::CanvasScene(QObject *parent)
    : QGraphicsScene(parent) {
    setItemIndexMethod(QGraphicsScene::NoIndex);
    undoStack = new QUndoStack(this);
}

CanvasScene::~CanvasScene() {
}

QVector<CellItem*> CanvasScene::getAllCells() const {
    QVector<CellItem*> cells;
    for (QGraphicsItem *item : items()) {
        if (CellItem *cell = dynamic_cast<CellItem*>(item))
            cells.append(cell);
    }
    return cells;
}

void CanvasScene::saveSnapshot() {
    QVector<CellItem*> itemsBefore = getAllCells();
    QVector<CellSnapshot> snapshotBefore;
    for (CellItem *item : itemsBefore) {
        snapshotBefore.append({item->pos(), item->size()});
    }

    // 创建匿名命令类
    class SnapshotCommand : public QUndoCommand {
    public:
        SnapshotCommand(CanvasScene *scene,
                        QVector<CellItem*> beforeItems,
                        QVector<CellSnapshot> beforeState,
                        QUndoCommand *parent = nullptr)
            : QUndoCommand(parent), m_scene(scene),
            m_items(beforeItems), m_before(beforeState)
        {
            for (CellItem *item : m_items) {
                m_after.append({item->pos(), item->size()});
            }
        }

        void undo() override {
            for (int i = 0; i < m_items.size(); ++i)
                m_items[i]->setPos(m_before[i].pos);
        }

        void redo() override {
            for (int i = 0; i < m_items.size(); ++i)
                m_items[i]->setPos(m_after[i].pos);
        }

    private:
        CanvasScene *m_scene;
        QVector<CellItem*> m_items;
        QVector<CellSnapshot> m_before, m_after;
    };

    // 提交命令
    undoStack->push(new SnapshotCommand(this, itemsBefore, snapshotBefore));
}

void CanvasScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (!event) {
        qDebug() << "无效的事件或场景";
        return;
    }
    qDebug() << "鼠标按下位置:" << event->scenePos();
    if (event->button() == Qt::LeftButton) {
        if (!views().isEmpty()) {
            views().first()->setCursor(QCursor(Qt::ClosedHandCursor));
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void CanvasScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (!event) {
        qDebug() << "无效的鼠标事件";
        return;
    }

    if (m_gridSnap) {
        // 将鼠标位置对齐到网格
        QPointF snappedPos = event->scenePos();
        snappedPos.setX(std::round(snappedPos.x() / m_gridSize) * m_gridSize);
        snappedPos.setY(std::round(snappedPos.y() / m_gridSize) * m_gridSize);
        event->setScenePos(snappedPos);
    }

    // 原有的鼠标移动逻辑
    QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
    if (item && item->flags() & QGraphicsItem::ItemIsMovable) {
        if (m_gridSnap) {
            // 将图元位置对齐到网格
            QPointF snappedPos = event->scenePos() - item->boundingRect().center();
            snappedPos.setX(std::round(snappedPos.x() / m_gridSize) * m_gridSize);
            snappedPos.setY(std::round(snappedPos.y() / m_gridSize) * m_gridSize);
            item->setPos(snappedPos);
        } else {
            item->setPos(event->scenePos() - item->boundingRect().center());
        }
    }

    // 连线模式
    if (!property("startItem").isNull()) {
        CellItem* startItem = qvariant_cast<CellItem*>(property("startItem"));
        CellItem::Connector startConnector = qvariant_cast<CellItem::Connector>(property("startConnector"));
        if (startItem) {
            if (!m_tempLine) {
                m_tempLine = new QGraphicsLineItem();
                m_tempLine->setPen(QPen(Qt::black, 2, Qt::DashLine));
                addItem(m_tempLine);
            }
            QPointF startPos = startItem->mapToScene(startConnector.pos);
            QPointF endPos = event->scenePos();
            
            if (m_gridSnap) {
                // 将连线端点对齐到网格
                endPos.setX(std::round(endPos.x() / m_gridSize) * m_gridSize);
                endPos.setY(std::round(endPos.y() / m_gridSize) * m_gridSize);
            }
            
            m_tempLine->setLine(QLineF(startPos, endPos));
            update();
        }
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void CanvasScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_leftMouseButtonPressed = false;
    }

    // 清理临时连线
    if (!property("startItem").isNull()) {
        CellItem* startItem = qvariant_cast<CellItem*>(property("startItem"));
        CellItem::Connector startConnector = qvariant_cast<CellItem::Connector>(property("startConnector"));

        // 查找结束图元和连接点
        CellItem* endItem = nullptr;
        CellItem::Connector endConnector(QPointF(), "");
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        if (item && item != startItem) {
            endItem = dynamic_cast<CellItem*>(item);
            if (endItem && endItem->isOnConnector(endItem->mapFromScene(event->scenePos()), endConnector)) {
                ConnectionLine* line = new ConnectionLine(startItem, startConnector, endItem, endConnector);
                addItem(line);
            }
        }

        // 清理临时连线
        if (m_tempLine) {
            removeItem(m_tempLine);
            delete m_tempLine;
            m_tempLine = nullptr;
            update(); // 强制场景更新
        }
        setProperty("startItem", QVariant());
        setProperty("startConnector", QVariant());
        if (!views().isEmpty()) {
            views().first()->setCursor(QCursor(Qt::OpenHandCursor));
        }
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void CanvasScene::keyPressEvent(QKeyEvent *event) {
    QGraphicsScene::keyPressEvent(event);
}

void CanvasScene::keyReleaseEvent(QKeyEvent *event) {
    QGraphicsScene::keyReleaseEvent(event);
}

void CanvasScene::drawBackground(QPainter *painter, const QRectF &rect) {
    // 调用基类方法
    QGraphicsScene::drawBackground(painter, rect);

    if (!m_gridVisible) {
        return;
    }

    // 保存当前绘制状态
    painter->save();

    // 设置网格线样式
    QPen lightPen(m_gridColor);
    lightPen.setWidth(1);
    lightPen.setStyle(Qt::DotLine);

    QPen darkPen(m_gridColor.darker(150));
    darkPen.setWidth(2);
    darkPen.setStyle(Qt::SolidLine);

    // 计算网格线的起始位置
    qreal left = std::floor(rect.left() / m_gridSize) * m_gridSize;
    qreal top = std::floor(rect.top() / m_gridSize) * m_gridSize;

    // 预分配线条数组
    QVarLengthArray<QLineF, 100> linesLight;
    QVarLengthArray<QLineF, 100> linesDark;

    // 绘制垂直线
    for (qreal x = left; x < rect.right(); x += m_gridSize) {
        if (static_cast<int>(x) % (m_gridSize * m_majorGridSpacing) == 0)
            linesDark.append(QLineF(x, rect.top(), x, rect.bottom()));
        else
            linesLight.append(QLineF(x, rect.top(), x, rect.bottom()));
    }

    // 绘制水平线
    for (qreal y = top; y < rect.bottom(); y += m_gridSize) {
        if (static_cast<int>(y) % (m_gridSize * m_majorGridSpacing) == 0)
            linesDark.append(QLineF(rect.left(), y, rect.right(), y));
        else
            linesLight.append(QLineF(rect.left(), y, rect.right(), y));
    }

    // 绘制网格线
    painter->setPen(lightPen);
    painter->drawLines(linesLight.data(), linesLight.size());

    painter->setPen(darkPen);
    painter->drawLines(linesDark.data(), linesDark.size());

    // 恢复绘制状态
    painter->restore();
}

void CanvasScene::addCellItem(CellItem *item) {
    this->addItem(item);  // 通过 QGraphicsScene 的 addItem 方法添加
}

void CanvasScene::setSelectionMode(bool enabled) {
    m_selectionModeEnabled = enabled;

    if (enabled) {
        // 启用选择模式，允许选择图元
        // setSelectionArea(QRectF());  // 例如，清空当前选择区域
        // continue;
    } else {
        // 禁用选择模式，取消所有选择
        clearSelection();
    }
}

void CanvasScene::deleteSelectedItems() {
    // 获取所有选中的项
    QList<QGraphicsItem*> selectedItems = this->selectedItems();

    // 删除每一个选中的项
    for (QGraphicsItem* item : selectedItems) {
        this->removeItem(item);  // 移除项
        delete item;  // 删除项
    }
}

void CanvasScene::undoAction() {
    // 简单占位逻辑，后期可集成 QUndoStack
    qDebug() << "Undo action triggered (未实现具体操作)";
    if (undoStack->canUndo())
        qDebug() << "Undo action triggered (实现具体操作)";
        undoStack->undo();
}

void CanvasScene::redoAction() {
    // 简单占位逻辑，后期可集成 QUndoStack
    qDebug() << "Redo action triggered (未实现具体操作)";
    if (undoStack->canRedo())
        qDebug() << "Undo action triggered (实现具体操作)";
        undoStack->redo();
}

void CanvasScene::setGridSize(int size) {
    if (size > 0) {
        m_gridSize = size;
        update();  // 更新场景以显示新的网格
    }
}

void CanvasScene::setGridVisible(bool visible) {
    m_gridVisible = visible;
    update();
}

void CanvasScene::setGridColor(const QColor &color) {
    m_gridColor = color;
    update();
}

void CanvasScene::setGridSnap(bool enabled) {
    m_gridSnap = enabled;
}

void CanvasScene::zoomIn() {
    setZoomFactor(m_zoomFactor + m_zoomStep);
}

void CanvasScene::zoomOut() {
    setZoomFactor(m_zoomFactor - m_zoomStep);
}

void CanvasScene::setZoomFactor(qreal factor) {
    // 限制缩放范围
    factor = qBound(m_minZoom, factor, m_maxZoom);
    
    if (factor != m_zoomFactor) {
        m_zoomFactor = factor;
        
        // 更新所有视图的变换矩阵
        for (QGraphicsView *view : views()) {
            view->resetTransform();
            view->scale(m_zoomFactor, m_zoomFactor);
        }
        
        update();
    }
}

void CanvasScene::wheelEvent(QGraphicsSceneWheelEvent *event) {
    // 检查是否按住Ctrl键
    if (event->modifiers() & Qt::ControlModifier) {
        // 根据滚轮方向决定放大或缩小
        if (event->delta() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
    } else {
        // 如果没有按住Ctrl键，则调用基类方法处理其他滚轮事件
        QGraphicsScene::wheelEvent(event);
    }
}

void CanvasScene::setRulerVisible(bool visible) {
    m_rulerVisible = visible;
    update();
}

void CanvasScene::setRulerColor(const QColor &color) {
    m_rulerColor = color;
    update();
}

void CanvasScene::drawForeground(QPainter *painter, const QRectF &rect) {
    if (!m_rulerVisible) {
        return;
    }

    // 保存当前画笔状态
    painter->save();
    
    // 获取视图信息
    QGraphicsView *view = views().first();
    if (!view) {
        painter->restore();
        return;
    }

    // 保存当前的变换矩阵
    QTransform originalTransform = painter->transform();
    
    // 重置变换矩阵，使绘制在视图坐标系统中进行
    painter->resetTransform();
    
    // 设置标尺颜色
    painter->setPen(m_rulerColor);
    
    // 获取视图的视口尺寸
    QRect viewportRect = view->viewport()->rect();
    
    // 计算标尺区域
    QRectF horizontalRuler(0, 0, viewportRect.width(), m_rulerSize);
    QRectF verticalRuler(0, 0, m_rulerSize, viewportRect.height());

    // 绘制水平标尺背景
    painter->fillRect(horizontalRuler, Qt::white);
    painter->drawRect(horizontalRuler);

    // 绘制垂直标尺背景
    painter->fillRect(verticalRuler, Qt::white);
    painter->drawRect(verticalRuler);

    // 设置文字样式
    QFont font = painter->font();
    font.setPointSize(8);
    painter->setFont(font);

    // 获取视图的可见区域
    QRectF viewRect = view->mapToScene(viewportRect).boundingRect();
    
    // 绘制水平标尺刻度
    for (qreal x = std::ceil(viewRect.left() / m_gridSize) * m_gridSize; 
         x <= viewRect.right(); x += m_gridSize) {
        // 将场景坐标转换为视图坐标
        QPointF viewPoint = view->mapFromScene(QPointF(x, 0));
        
        // 绘制刻度线
        painter->drawLine(QPointF(viewPoint.x(), 0), QPointF(viewPoint.x(), m_rulerSize));

        // 绘制刻度值
        if (static_cast<int>(x) % (m_gridSize * m_majorGridSpacing) == 0) {
            QString text = QString::number(static_cast<int>(x));
            QRectF textRect(viewPoint.x() - 20, 0, 40, m_rulerSize - m_rulerTextOffset);
            painter->drawText(textRect, Qt::AlignCenter, text);
        }
    }

    // 绘制垂直标尺刻度
    for (qreal y = std::ceil(viewRect.top() / m_gridSize) * m_gridSize; 
         y <= viewRect.bottom(); y += m_gridSize) {
        // 将场景坐标转换为视图坐标
        QPointF viewPoint = view->mapFromScene(QPointF(0, y));
        
        // 绘制刻度线
        painter->drawLine(QPointF(0, viewPoint.y()), QPointF(m_rulerSize, viewPoint.y()));

        // 绘制刻度值
        if (static_cast<int>(y) % (m_gridSize * m_majorGridSpacing) == 0) {
            QString text = QString::number(static_cast<int>(y));
            QRectF textRect(0, viewPoint.y() - 20, m_rulerSize - m_rulerTextOffset, 40);
            painter->drawText(textRect, Qt::AlignCenter, text);
        }
    }

    // 恢复原始变换矩阵
    painter->setTransform(originalTransform);
    
    // 恢复画笔状态
    painter->restore();
}

