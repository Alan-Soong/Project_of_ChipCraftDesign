#include "canvasscene.h"
#include "pinitem.h"
#include "command.h"
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
    // 设置一个合理的默认场景矩形：从(-1000, -1000)到(1000, 1000)
    setSceneRect(-1000, -1000, 1000, 1000);
    setSceneRect(-1000, -1000, 1000, 1000);
    m_tempLine = nullptr;
    m_isGroupMoving = false;
    m_groupMoveStartPos = QPointF();
}

CanvasScene::~CanvasScene()
{
    delete undoStack;
    // 清理连线对象
    if (!m_connectionLines.isEmpty()) {
        for (ConnectionLine* line : m_connectionLines) {
            if (line) {
                delete line;
            }
        }
        m_connectionLines.clear();
    }
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

        // 只在场景矩形是默认大小时才调整，避免覆盖从文件加载的尺寸
        QRectF currentRect = sceneRect();
        if (qAbs(currentRect.width() - 2000) < 1.0 && qAbs(currentRect.height() - 2000) < 1.0) {
            qreal newSize = 2000 * (1.0 / factor);
            setSceneRect(-newSize/2, -newSize/2, newSize, newSize);
        }
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

void CanvasScene::set_unit(const QString unit)
{
    to_unit = unit;
    update();
}

QString CanvasScene::get_unit() const
{
    return to_unit;
}

void CanvasScene::addCellItem(CellItem *item)
{
    if (!item) return;
    addItem(item);
    item->setFlag(QGraphicsItem::ItemIsSelectable, true);
    item->setFlag(QGraphicsItem::ItemIsMovable, true);
}

void CanvasScene::addConnectionLine(ConnectionLine *line)
{
    if (!line) return;
    addItem(line);
    m_connectionLines.append(line);
}

void CanvasScene::removeConnectionLine(ConnectionLine *line)
{
    if (!line) return;

    // 从场景中移除图形项
    removeItem(line);

    // 从连线列表中移除
    m_connectionLines.removeOne(line);

    // 删除对象
    delete line;

    // qDebug() << "已删除连线，当前连线数量:" << m_connectionLines.size();
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
    if (selectedItems.isEmpty()) {
        return;
    }

    QList<CellItem*> cellsToDelete;
    QList<ConnectionLine*> linesToDelete;

    // 分类选中的项目
    for (QGraphicsItem* item : selectedItems) {
        if (CellItem* cellItem = dynamic_cast<CellItem*>(item)) {
            cellsToDelete.append(cellItem);
        } else if (ConnectionLine* line = dynamic_cast<ConnectionLine*>(item)) {
            linesToDelete.append(line);
        }
    }

    // 使用撤销命令删除芯片
    if (!cellsToDelete.isEmpty() && undoStack) {
        for (CellItem* cellItem : cellsToDelete) {
            DeleteRectangleCommand* deleteCommand = new DeleteRectangleCommand(
                this, cellItem, undoStack);
            undoStack->push(deleteCommand);
        }
    } else {
        // 如果没有撤销栈，直接删除（向后兼容）
        for (CellItem* cellItem : cellsToDelete) {
            // 删除所有与此CellItem相关的连线
            if (!m_connectionLines.isEmpty()) {
                for (int i = m_connectionLines.size() - 1; i >= 0; --i) {
                    ConnectionLine* line = m_connectionLines[i];
                    if (line && (line->getStartItem() == cellItem || line->getEndItem() == cellItem)) {
                        removeItem(line);
                        m_connectionLines.removeAt(i);
                        delete line;
                    }
                }
            }

            // 删除芯片本身
            removeItem(cellItem);
            delete cellItem;
        }
    }

    // 使用撤销命令删除连线
    if (!linesToDelete.isEmpty() && undoStack) {
        for (ConnectionLine* line : linesToDelete) {
            RemoveConnectionCommand* removeCommand = new RemoveConnectionCommand(
                this, line);
            undoStack->push(removeCommand);
        }
    } else {
        // 如果没有撤销栈，直接删除连线（向后兼容）
        for (ConnectionLine* line : linesToDelete) {
            removeConnectionLine(line);
        }
    }

    // 保留芯片删除相关的调试信息
    qDebug() << "删除完成 - 芯片:" << cellsToDelete.size() << "个，连线:" << linesToDelete.size() << "条";
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

    // qDebug() << "Started connection from" << startItem << "pin" << startPinId;
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

    // qDebug() << "Finished connection to" << endItem << "pin" << endPinId;
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

    // qDebug() << "Connection cancelled";
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
    if (m_connectionLines.isEmpty()) {
        return;
    }

    for (ConnectionLine* line : m_connectionLines) {
        if (line) {
            line->updatePosition();
        }
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

        // 检查是否有选中的项目，如果有多个选中项目，启用组移动模式
        QList<QGraphicsItem*> selectedItems = this->selectedItems();
        if (selectedItems.size() > 1) {
            // 检查点击的是否是选中项目之一
            QGraphicsItem* clickedItem = itemAt(scenePos, QTransform());
            if (clickedItem && selectedItems.contains(clickedItem)) {
                m_isGroupMoving = true;
                m_groupMoveStartPos = scenePos;

                // 记录所有选中项目的初始位置
                m_groupMoveInitialPositions.clear();
                for (QGraphicsItem* item : selectedItems) {
                    if (CellItem* cellItem = dynamic_cast<CellItem*>(item)) {
                        if (cellItem) { // 添加空指针检查
                            m_groupMoveInitialPositions[cellItem] = cellItem->pos();
                        }
                    }
                }

                event->accept();
                return;
            }
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

    // 如果正在进行组移动
    if (m_isGroupMoving && m_leftMouseButtonPressed) {
        QPointF delta = scenePos - m_groupMoveStartPos;

        // 移动所有选中的CellItem
        for (auto it = m_groupMoveInitialPositions.begin(); it != m_groupMoveInitialPositions.end(); ++it) {
            CellItem* cellItem = it.key();
            if (!cellItem) continue; // 添加空指针检查

            QPointF initialPos = it.value();
            QPointF newPos = initialPos + delta;

            // 应用位置限制
            cellItem->restrictPosition(newPos);
            cellItem->setPos(newPos);
        }

        // 更新所有连线位置
        updateAllConnectionLines();

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

    // 结束组移动模式
    if (m_isGroupMoving) {
        m_isGroupMoving = false;
        m_groupMoveInitialPositions.clear();

        // 确保连线位置正确更新
        updateAllConnectionLines();

        event->accept();
        return; // 直接返回，避免重复处理
    }

    QGraphicsScene::mouseReleaseEvent(event);

    // 更新所有连线位置（仅在非组移动时）
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

    QFont font = painter->font();
    font.setPointSize(2);  // 调小字体
    painter->setFont(font);

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

        // 获取网格单位（假设m_gridSize为10表示1mm）
        qreal gridUnit = m_gridSize / 10.0; // 1个网格单位对应的实际距离（mm）

        // 单位转换函数
        auto convertUnit = [](qreal value, const QString& fromUnit, const QString& toUnit) -> qreal {
            if (fromUnit == "mm" && toUnit == "cm") {
                return value / 10.0;
            } else if (fromUnit == "mm" && toUnit == "dm") {
                return value / 100.0;
            }
            return value;
        };

        // 绘制水平标尺刻度
        for (qreal x = qFloor(visibleRect.left() / interval) * interval; x < visibleRect.right(); x += interval) {
            // 每隔majorInterval绘制一个带数字的刻度
            if (qAbs(fmod(x, majorInterval)) < 0.1) {
                painter->drawLine(QPoint(x, visibleRect.top()), QPoint(x, visibleRect.top() + rulerWidth * 0.3));
                // 将坐标转换为实际距离（以毫米为基准）
                qreal distance = x * gridUnit; // 距离（mm）
                // 根据当前单位转换距离
                qreal convertedDistance = convertUnit(distance, "mm", to_unit);
                QString text = QString::number(convertedDistance, 'f', 1) + to_unit;
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
                // 将坐标转换为实际距离（以毫米为基准）
                qreal distance = y * gridUnit; // 距离（mm）
                // 根据当前单位转换距离
                qreal convertedDistance = convertUnit(distance, "mm", to_unit);
                QString text = QString::number(convertedDistance, 'f', 1) + to_unit;
                painter->drawText(visibleRect.left() + rulerWidth * 0.2, y + fm.height() / 2, text);
            } else {
                painter->drawLine(QPoint(visibleRect.left(), y), QPoint(visibleRect.left() + rulerWidth * 0.2, y));
            }
        }
    }
    
    painter->restore();
}
