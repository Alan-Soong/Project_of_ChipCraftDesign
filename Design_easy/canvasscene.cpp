#include "canvasscene.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QDebug>
#include <QKeyEvent>
#include <QApplication>
#include <QMessageBox>
#include <QGraphicsSceneWheelEvent>
#include <QFont>
#include <QFontMetrics>
#include <cmath>

CanvasScene::CanvasScene(QObject *parent)
    : QGraphicsScene(parent), undoStack(new QUndoStack(this))
{
    setSceneRect(-1000, -1000, 1000, 1000);
    m_tempLine = nullptr;
}

CanvasScene::~CanvasScene()
{
    delete undoStack;
    // 清理连线对象
    for (ConnectionLine* line : m_connectionLines) {
        delete line;
    }
    m_connectionLines.clear();
}

void CanvasScene::saveSnapshot()
{
    // 保存当前场景状态的快照
    // 这里可以实现保存所有CellItem的位置和大小
}

void CanvasScene::setGridSize(int size)
{
    m_gridSize = size;
    update();
}

void CanvasScene::setGridVisible(bool visible)
{
    m_gridVisible = visible;
    update();
}

void CanvasScene::setGridColor(const QColor &color)
{
    m_gridColor = color;
    update();
}

void CanvasScene::setGridSnap(bool enabled)
{
    m_gridSnap = enabled;
}

void CanvasScene::zoomIn()
{
    m_zoomFactor = qMin(m_zoomFactor + m_zoomStep, m_maxZoom);
    QList<QGraphicsView*> views = this->views();
    for (QGraphicsView* view : views) {
        view->scale(1.1, 1.1);
    }
}

void CanvasScene::zoomOut()
{
    m_zoomFactor = qMax(m_zoomFactor - m_zoomStep, m_minZoom);
    QList<QGraphicsView*> views = this->views();
    for (QGraphicsView* view : views) {
        view->scale(0.9, 0.9);
    }
}

void CanvasScene::setZoomFactor(qreal factor)
{
    m_zoomFactor = qBound(m_minZoom, factor, m_maxZoom);
    // 更新所有视图的变换矩阵
    for (QGraphicsView *view : views()) {
        view->resetTransform();
        view->scale(m_zoomFactor, m_zoomFactor);

        // 根据缩放因子调整场景大小
        qreal newSize = 2000 * (1.0 / factor);
        setSceneRect(-newSize/2, -newSize/2, newSize, newSize);
    }

    update();
}

void CanvasScene::setRulerVisible(bool visible)
{
    m_rulerVisible = visible;
    update();
}

void CanvasScene::setRulerColor(const QColor &color)
{
    m_rulerColor = color;
    update();
}

void CanvasScene::addCellItem(CellItem *item)
{
    if (!item) return;
    addItem(item);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    item->setFlag(QGraphicsItem::ItemIsMovable, true);
}

void CanvasScene::setSelectionMode(bool enabled)
{
    m_selectionModeEnabled = enabled;
    if (enabled) {
        QApplication::setOverrideCursor(Qt::CrossCursor);
    } else {
        QApplication::restoreOverrideCursor();
    }
}

void CanvasScene::deleteSelectedItems()
{
    QList<QGraphicsItem*> selectedItems = this->selectedItems();
    for (QGraphicsItem* item : selectedItems) {
        // 如果是连线，需要特殊处理
        if (ConnectionLine* line = dynamic_cast<ConnectionLine*>(item)) {
            m_connectionLines.removeOne(line);
        }
        
        // 如果是CellItem，需要删除与之相关的所有连线
        if (CellItem* cellItem = dynamic_cast<CellItem*>(item)) {
            // 删除所有与此CellItem相关的连线
            for (int i = m_connectionLines.size() - 1; i >= 0; --i) {
                ConnectionLine* line = m_connectionLines[i];
                if (line->getStartItem() == cellItem || line->getEndItem() == cellItem) {
                    removeItem(line);
                    m_connectionLines.removeAt(i);
                    delete line;
                }
            }
        }
        
        removeItem(item);
        delete item;
    }
}

void CanvasScene::undoAction()
{
    undoStack->undo();
}

void CanvasScene::redoAction()
{
    undoStack->redo();
}

void CanvasScene::zoomInButton()
{
    zoomIn();
}

void CanvasScene::zoomOutButton()
{
    zoomOut();
}

void CanvasScene::startConnection(CellItem* startItem, const QString& startPinId)
{
    if (!startItem) {
        qWarning() << "Cannot start connection: Invalid start item";
        return;
    }
    
    m_connectionStartItem = startItem;
    m_connectionStartPinId = startPinId;
    
    // 创建临时线
    if (!m_tempLine) {
        m_tempLine = new QGraphicsLineItem();
        m_tempLine->setPen(QPen(Qt::red, 2, Qt::DashLine));
        addItem(m_tempLine);
    }
    
    // 设置临时线的起点
    QPointF startPos;
    auto startPins = startItem->getPinItems();
    auto startConnectors = startItem->getConnectors();
    for (int i = 0; i < startConnectors.size() && i < startPins.size(); ++i) {
        if (startConnectors[i].id == startPinId) {
            QPointF pinCenter = startPins[i]->boundingRect().center() + startPins[i]->pos();
            startPos = startItem->pos() + pinCenter;
            break;
        }
    }
    
    m_tempLine->setLine(QLineF(startPos, startPos));
    
    qDebug() << "Started connection from" << startItem << "pin" << startPinId;
}

void CanvasScene::finishConnection(CellItem* endItem, const QString& endPinId)
{
    if (!m_connectionStartItem || !endItem || m_connectionStartPinId.isEmpty() || endPinId.isEmpty()) {
        qWarning() << "Cannot finish connection: Invalid parameters";
        cancelConnection();
        return;
    }
    
    // 不允许连接到同一个芯片
    if (m_connectionStartItem == endItem) {
        qWarning() << "Cannot connect to the same cell item";
        QMessageBox::warning(nullptr, "连线错误", "不能连接到同一个芯片");
        cancelConnection();
        return;
    }
    
    // 查找起始引脚
    CellItem::Connector startConnector;
    bool startFound = false;
    auto startConnectors = m_connectionStartItem->getConnectors();
    for (const auto& conn : startConnectors) {
        if (conn.id == m_connectionStartPinId) {
            startConnector = conn;
            startFound = true;
            break;
        }
    }
    
    // 查找结束引脚
    CellItem::Connector endConnector;
    bool endFound = false;
    auto endConnectors = endItem->getConnectors();
    for (const auto& conn : endConnectors) {
        if (conn.id == endPinId) {
            endConnector = conn;
            endFound = true;
            break;
        }
    }
    
    if (!startFound || !endFound) {
        qWarning() << "Cannot finish connection: Pin not found";
        cancelConnection();
        return;
    }
    
    // 创建连线
    ConnectionLine* line = new ConnectionLine(m_connectionStartItem, startConnector, endItem, endConnector);
    addItem(line);
    m_connectionLines.append(line);
    
    // 记录连线关系
    m_connectionStartItem->addConnection(endItem, m_connectionStartPinId, endPinId);
    
    // 清理临时线
    if (m_tempLine) {
        removeItem(m_tempLine);
        delete m_tempLine;
        m_tempLine = nullptr;
    }
    
    // 重置连接状态
    m_connectionStartItem = nullptr;
    m_connectionStartPinId.clear();
    
    qDebug() << "Finished connection to" << endItem << "pin" << endPinId;
}

void CanvasScene::cancelConnection()
{
    // 清理临时线
    if (m_tempLine) {
        removeItem(m_tempLine);
        delete m_tempLine;
        m_tempLine = nullptr;
    }
    
    // 重置连接状态
    m_connectionStartItem = nullptr;
    m_connectionStartPinId.clear();
    
    qDebug() << "Connection cancelled";
}

QList<ConnectionLine*> CanvasScene::getConnectionLines() const
{
    return m_connectionLines;
}

QList<CellItem*> CanvasScene::getAllCellItems() const
{
    QList<CellItem*> cellItems;
    for (QGraphicsItem* item : items()) {
        if (CellItem* cellItem = dynamic_cast<CellItem*>(item)) {
            cellItems.append(cellItem);
        }
    }
    return cellItems;
}

PinItem* CanvasScene::findPinItemAt(const QPointF& scenePos, CellItem** outCellItem, QString* outPinId)
{
    // 查找点击位置的引脚
    for (QGraphicsItem* item : items(scenePos)) {
        if (PinItem* pinItem = dynamic_cast<PinItem*>(item)) {
            // 找到引脚所属的CellItem
            QGraphicsItem* parentItem = pinItem->parentItem();
            if (CellItem* cellItem = dynamic_cast<CellItem*>(parentItem)) {
                if (outCellItem) *outCellItem = cellItem;
                
                // 查找引脚ID
                auto pinItems = cellItem->getPinItems();
                auto connectors = cellItem->getConnectors();
                for (int i = 0; i < pinItems.size() && i < connectors.size(); ++i) {
                    if (pinItems[i] == pinItem) {
                        if (outPinId) *outPinId = connectors[i].id;
                        return pinItem;
                    }
                }
            }
        }
    }
    
    return nullptr;
}

void CanvasScene::updateAllConnectionLines()
{
    for (ConnectionLine* line : m_connectionLines) {
        line->updatePosition();
    }
}

void CanvasScene::updateOverlapStates()
{
    // 获取场景中所有的CellItem
    QList<QGraphicsItem*> items = this->items();
    for (QGraphicsItem* item : items) {
        CellItem* cell = dynamic_cast<CellItem*>(item);
        if (cell) {
            cell->updateOverlapState();
        }
    }
}

void CanvasScene::addRectangle()
{
    // 创建新的CellItem
    CellItem* newItem = new CellItem();
    newItem->setPos(0, 0);  // 设置初始位置
    addCellItem(newItem);
    
    // 在添加新矩形后更新所有矩形的重合状态
    updateOverlapStates();
}

void CanvasScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event) return;
    
    QPointF scenePos = event->scenePos();
    qDebug() << "Scene: Mouse press at" << scenePos;
    
    if (event->button() == Qt::LeftButton) {
        m_leftMouseButtonPressed = true;
        
        // 检查是否点击了引脚
        CellItem* cellItem = nullptr;
        QString pinId;
        PinItem* pinItem = findPinItemAt(scenePos, &cellItem, &pinId);
        
        if (pinItem && cellItem) {
            // 如果已经开始连线，则完成连线
            if (m_connectionStartItem && !m_connectionStartPinId.isEmpty()) {
                finishConnection(cellItem, pinId);
            } else {
                // 否则开始新的连线
                startConnection(cellItem, pinId);
            }
            event->accept();
            return;
        }
        
        // 如果正在连线但点击了空白区域，取消连线
        if (m_connectionStartItem && !m_connectionStartPinId.isEmpty()) {
            cancelConnection();
            event->accept();
            return;
        }
    } else if (event->button() == Qt::RightButton) {
        // 右键取消连线
        if (m_connectionStartItem && !m_connectionStartPinId.isEmpty()) {
            cancelConnection();
            event->accept();
            return;
        }
    }
    
    QGraphicsScene::mousePressEvent(event);
}

void CanvasScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event) return;
    
    QPointF scenePos = event->scenePos();
    
    // 如果正在连线，更新临时线的终点
    if (m_connectionStartItem && !m_connectionStartPinId.isEmpty() && m_tempLine) {
        QLineF line = m_tempLine->line();
        line.setP2(scenePos);
        m_tempLine->setLine(line);
        event->accept();
        return;
    }
    
    QGraphicsScene::mouseMoveEvent(event);
    
    // 更新所有连线位置
    updateAllConnectionLines();
    
    // 更新重合状态
    updateOverlapStates();
}

void CanvasScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event) return;
    
    m_leftMouseButtonPressed = false;
    
    QGraphicsScene::mouseReleaseEvent(event);
    
    // 更新所有连线位置
    updateAllConnectionLines();
}

void CanvasScene::keyPressEvent(QKeyEvent *event)
{
    if (!event) return;
    
    // 按ESC键取消连线
    if (event->key() == Qt::Key_Escape) {
        if (m_connectionStartItem && !m_connectionStartPinId.isEmpty()) {
            cancelConnection();
            event->accept();
            return;
        }
    }
    
    // 按Delete键删除选中项
    if (event->key() == Qt::Key_Delete) {
        deleteSelectedItems();
        event->accept();
        return;
    }
    
    QGraphicsScene::keyPressEvent(event);
}

void CanvasScene::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsScene::keyReleaseEvent(event);
}

void CanvasScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QGraphicsScene::wheelEvent(event);
}

void CanvasScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);

    if (!m_gridVisible) return;

    // 绘制网格
    QPen pen(m_gridColor, 0);
    painter->setPen(pen);

    qreal left = int(rect.left()) - (int(rect.left()) % m_gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % m_gridSize);

    QVector<QLineF> lines;

    // 绘制垂直线
    for (qreal x = left; x < rect.right(); x += m_gridSize) {
        // 每隔m_majorGridSpacing个网格绘制一条粗线
        if (int(x / m_gridSize) % m_majorGridSpacing == 0) {
            pen.setWidth(1);
            pen.setColor(m_gridColor.darker(120));
        } else {
            pen.setWidth(0);
            pen.setColor(m_gridColor);
        }
        painter->setPen(pen);
        painter->drawLine(QLineF(x, rect.top(), x, rect.bottom()));
    }

    // 绘制水平线
    for (qreal y = top; y < rect.bottom(); y += m_gridSize) {
        // 每隔m_majorGridSpacing个网格绘制一条粗线
        if (int(y / m_gridSize) % m_majorGridSpacing == 0) {
            pen.setWidth(1);
            pen.setColor(m_gridColor.darker(120));
        } else {
            pen.setWidth(0);
            pen.setColor(m_gridColor);
        }
        painter->setPen(pen);
        painter->drawLine(QLineF(rect.left(), y, rect.right(), y));
    }
}

void CanvasScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawForeground(painter, rect);
    
    if (!m_rulerVisible) return;
    
    // 绘制标尺
    painter->save();
    
    QPen pen(m_rulerColor, 1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    
    // 获取视图的可见区域
    QRectF visibleRect;
    QList<QGraphicsView*> views = this->views();
    if (!views.isEmpty()) {
        QGraphicsView* view = views.first();
        visibleRect = view->mapToScene(view->viewport()->rect()).boundingRect();
        
        // 计算标尺宽度为视口宽度的相对比例，并考虑缩放因子
        qreal viewportWidth = view->viewport()->width();
        qreal scale = view->transform().m11(); // 获取水平缩放比例
        
        // 基础标尺宽度为视口宽度的3%，但会根据缩放比例调整
        qreal baseRulerWidth = viewportWidth * 0.03;
        // 缩放时标尺宽度会相应调整
        qreal rulerWidth = baseRulerWidth / scale;
        
        // 设置字体大小，根据缩放比例动态调整
        QFont font = painter->font();
        // 基础字体大小为12，根据缩放比例调整
        qreal baseFontSize = 12.0;
        qreal fontSize = baseFontSize / scale;
        font.setPointSizeF(fontSize);
        painter->setFont(font);
        
        QFontMetrics fm(font);
        
        // 绘制水平标尺
        QRectF horizontalRuler(visibleRect.left(), visibleRect.top(), visibleRect.width(), rulerWidth);
        painter->fillRect(horizontalRuler, QColor(240, 240, 240));
        painter->drawRect(horizontalRuler);
        
        // 绘制垂直标尺
        QRectF verticalRuler(visibleRect.left(), visibleRect.top(), rulerWidth, visibleRect.height());
        painter->fillRect(verticalRuler, QColor(240, 240, 240));
        painter->drawRect(verticalRuler);
        
        // 计算合适的刻度间隔，使屏幕内显示固定数量的刻度
        const int targetTickCount = 80; // 目标刻度数量
        qreal visibleWidth = visibleRect.width();
        qreal visibleHeight = visibleRect.height();
        
        // 计算基础间隔
        qreal baseInterval = qMax(visibleWidth, visibleHeight) / targetTickCount;
        
        // 将间隔调整为网格大小的整数倍
        qreal gridMultiplier = qCeil(baseInterval / m_gridSize);
        qreal interval = m_gridSize * gridMultiplier;
        
        // 主刻度间隔为普通间隔的5倍
        qreal majorInterval = interval * 5;
        
        // 绘制水平标尺刻度
        for (qreal x = qFloor(visibleRect.left() / interval) * interval; x < visibleRect.right(); x += interval) {
            // 每隔majorInterval绘制一个带数字的刻度
            if (qAbs(fmod(x, majorInterval)) < 0.1) {
                painter->drawLine(QPoint(x, visibleRect.top()), QPoint(x, visibleRect.top() + rulerWidth * 0.3));
                QString text = QString::number(qRound(x));
                painter->drawText(x - fm.horizontalAdvance(text) / 2, visibleRect.top() + rulerWidth * 0.8, text);
            } else {
                painter->drawLine(QPoint(x, visibleRect.top()), QPoint(x, visibleRect.top() + rulerWidth * 0.2));
            }
        }
        
        // 绘制垂直标尺刻度
        for (qreal y = qFloor(visibleRect.top() / interval) * interval; y < visibleRect.bottom(); y += interval) {
            // 每隔majorInterval绘制一个带数字的刻度
            if (qAbs(fmod(y, majorInterval)) < 0.1) {
                painter->drawLine(QPoint(visibleRect.left(), y), QPoint(visibleRect.left() + rulerWidth * 0.3, y));
                QString text = QString::number(qRound(y));
                painter->drawText(visibleRect.left() + rulerWidth * 0.2, y + fm.height() / 2, text);
            } else {
                painter->drawLine(QPoint(visibleRect.left(), y), QPoint(visibleRect.left() + rulerWidth * 0.2, y));
            }
        }
    }
    
    painter->restore();
}
