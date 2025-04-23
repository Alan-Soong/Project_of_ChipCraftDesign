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
    qDebug() << "鼠标移动到:" << event->scenePos();

    // 示例：假设第 25 行操作了一个 CellItem
    QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
    if (item && item->flags() & QGraphicsItem::ItemIsMovable) {
        // 确保只移动可移动的图元
        item->setPos(event->scenePos() - item->boundingRect().center());
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
            m_tempLine->setLine(QLineF(startPos, event->scenePos()));
            update(); // 强制场景更新
        }
    }

    QGraphicsScene::mouseMoveEvent(event); // 调用基类方法
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
    QGraphicsScene::drawBackground(painter, rect);
    // Custom background drawing logic (e.g., grid lines)
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

