#include "cellitem_new.h"
#include "pinitem.h"
#include "pineditordialog.h"
#include "canvasscene.h"
#include "connectionline.h"
#include "command.h"
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>
#include <QPainterPath>
#include <QDebug>
#include <QGraphicsView>
#include <QTransform>
#include <QtMath>

CellItem::CellItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , m_size(150, 100)
    , m_resizeEdge(None)
    , m_isDragging(false)
    , m_macroName("MC0")  // 默认无引脚，对应MC0
    , m_instanceName("DefaultInstance")
    , m_updatingOverlap(false)
    , m_lastRecordedPosition()  // 初始化为空点
    , m_positionUpdateInProgress(false)
    , m_dragStartPosition()  // 初始化为空点
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
}

CellItem::~CellItem()
{
    qDeleteAll(m_pinItems);
    m_pinItems.clear();
}

QRectF CellItem::boundingRect() const
{
    return QRectF(0, 0, m_size.width(), m_size.height());
}

void CellItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    // 获取当前视图的缩放因子
    qreal scaleFactor = 1.0;
    if (scene() && !scene()->views().isEmpty()) {
        QGraphicsView* view = scene()->views().first();
        QTransform transform = view->transform();
        scaleFactor = qSqrt(transform.m11() * transform.m11() + transform.m12() * transform.m12());
    }

    // 绘制主矩形，边框宽度根据缩放自适应
    qreal borderWidth = 1.0; // 基础边框宽度
    if (scaleFactor > 0) {
        // 在高缩放时适当减细边框，在低缩放时适当加粗以保持可见性
        if (scaleFactor > 10) {
            borderWidth = 0.5;
        } else if (scaleFactor < 3) {
            borderWidth = 1.5;
        }
    }

    // 限制边框宽度范围
    borderWidth = qBound(0.5, borderWidth, 2.0);

    QPen pen(Qt::black, borderWidth);
    if (isSelected()) {
        pen.setColor(Qt::red);
        pen.setWidth(borderWidth * 1.5); // 选中状态稍粗一点
    }
    painter->setPen(pen);
    painter->setBrush(Qt::lightGray);
    painter->drawRect(boundingRect());

    // 绘制重叠区域阴影
    if (!m_overlapAreas.isEmpty()) {
        painter->save();
        // 设置重叠区域的颜色和透明度
        QBrush overlapBrush(QColor(255, 0, 0, 100)); // 半透明红色
        painter->setBrush(overlapBrush);
        painter->setPen(Qt::NoPen);

        // 绘制所有重叠区域
        for (const QPainterPath& overlapPath : m_overlapAreas) {
            // 获取重叠区域的矩形
            QRectF overlapRect = overlapPath.boundingRect();
            // 转换到当前项的本地坐标系
            QRectF localOverlapRect = QRectF(
                overlapRect.x() - pos().x(),
                overlapRect.y() - pos().y(),
                overlapRect.width(),
                overlapRect.height()
                );
            painter->drawRect(localOverlapRect);
        }
        painter->restore();
    }

    // 绘制文本标签
    painter->setPen(Qt::black);

    // 根据芯片大小和视图缩放自适应字体大小
    QFont font = painter->font();

    // 创建显示文本：格式为 CX|MCY
    QString displayText = m_instanceName + "|" + m_macroName;

    // 计算芯片的可用显示区域（留出边距）
    qreal margin = 4.0; // 边距
    qreal availableWidth = m_size.width() - 2 * margin;
    qreal availableHeight = m_size.height() - 2 * margin;

    // 确保最小可用空间
    availableWidth = qMax(availableWidth, 20.0);
    availableHeight = qMax(availableHeight, 10.0);

    // 基于可用空间计算初始字体大小
    qreal widthBasedSize = availableWidth / displayText.length() * 1.2; // 基于宽度
    qreal heightBasedSize = availableHeight * 0.6; // 基于高度，留出上下空间

    // 取较小值确保文本能完全显示
    qreal baseFontSize = qMin(widthBasedSize, heightBasedSize);

    // 根据芯片大小类别进行调整
    qreal minDimension = qMin(m_size.width(), m_size.height());
    if (minDimension < 50) {
        // 超小芯片：确保最小可读性
        baseFontSize = qMax(baseFontSize, 6.0);
    } else if (minDimension < 100) {
        // 小芯片：适中字体
        baseFontSize = qMax(baseFontSize, 8.0);
    } else if (minDimension > 300) {
        // 大芯片：避免字体过大
        baseFontSize = qMin(baseFontSize, minDimension * 0.15);
    }

    // 根据缩放因子调整字体大小，确保在不同缩放下都清晰可见
    qreal adjustedFontSize = baseFontSize;
    if (scaleFactor > 0) {
        if (scaleFactor > 15) {
            // 高缩放：减小字体避免过大
            adjustedFontSize = baseFontSize * 0.7;
        } else if (scaleFactor > 8) {
            adjustedFontSize = baseFontSize * 0.85;
        } else if (scaleFactor < 2) {
            // 低缩放：增大字体保证可读性
            adjustedFontSize = baseFontSize * 1.8;
        } else if (scaleFactor < 4) {
            adjustedFontSize = baseFontSize * 1.4;
        }
    }

    // 设置合理的字体大小范围
    adjustedFontSize = qBound(5.0, adjustedFontSize, 24.0);

    font.setPointSizeF(adjustedFontSize);
    painter->setFont(font);

    // 计算文本实际需要的空间，并支持多行显示
    QFontMetrics fm(font);
    QRect textBounds = fm.boundingRect(displayText);

    QString finalDisplayText = displayText;
    bool useMultiLine = false;

    // 如果文本太大，尝试多行显示或缩写
    if (textBounds.width() > availableWidth || textBounds.height() > availableHeight) {
        // 首先尝试多行显示（在 | 处换行）
        if (displayText.contains("|") && m_size.height() > 30) {
            QStringList parts = displayText.split("|");
            if (parts.size() == 2) {
                QString line1 = parts[0];
                QString line2 = parts[1];

                // 检查两行是否都能放下
                QRect line1Bounds = fm.boundingRect(line1);
                QRect line2Bounds = fm.boundingRect(line2);
                qreal lineHeight = fm.height();
                qreal totalHeight = lineHeight * 2 + 2; // 加点行间距

                if (line1Bounds.width() <= availableWidth &&
                    line2Bounds.width() <= availableWidth &&
                    totalHeight <= availableHeight) {
                    finalDisplayText = line1 + "\n" + line2;
                    useMultiLine = true;
                }
            }
        }

        // 如果多行还是不行，进一步缩小字体
        if (!useMultiLine) {
            qreal scaleFactorWidth = availableWidth / textBounds.width();
            qreal scaleFactorHeight = availableHeight / textBounds.height();
            qreal scaleFactor = qMin(scaleFactorWidth, scaleFactorHeight) * 0.9; // 留点余量

            adjustedFontSize *= scaleFactor;
            adjustedFontSize = qMax(adjustedFontSize, 4.0); // 保证最小可读性

            font.setPointSizeF(adjustedFontSize);
            painter->setFont(font);
        }
    }

    QRectF textRect = boundingRect();

    // 根据是否多行使用不同的绘制方式
    if (useMultiLine) {
        painter->drawText(textRect, Qt::AlignCenter, finalDisplayText);
    } else {
        painter->drawText(textRect, Qt::AlignCenter, finalDisplayText);
    }

    // 注意：引脚由PinItem单独绘制，不在这里重复绘制
}

void CellItem::setSize(const QSizeF& size)
{
    prepareGeometryChange();
    qDebug() << "setSize called: 原始尺寸:" << size;
    m_size = size;
    restrictSize(m_size);
    qDebug() << "setSize: 限制后尺寸:" << m_size;
    updatePinPositions();
    updateConnectedLines(); // 大小改变时也更新连线
    update();
}

void CellItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event || !scene()) {
        qDebug() << "无效的事件或场景";
        return;
    }
    qDebug() << "图元：鼠标按下位置:" << event->scenePos();

    if (event->button() == Qt::LeftButton) {
        // 记录拖动开始时的位置，用于撤销系统
        m_dragStartPosition = pos();

        // 先选中当前项
        if (!isSelected()) {
            setSelected(true);
        }

        // 检测是否在连接点上
        Connector connector;
        if (isOnConnector(event->pos(), connector)) {
            // 通知场景开始连线（通过自定义事件或信号）
            scene()->setProperty("startItem", QVariant::fromValue(this));
            scene()->setProperty("startConnector", QVariant::fromValue(connector));
            setCursor(QCursor(Qt::CrossCursor));
        }
        // 检测是否在边框或角点上
        else {
            ResizeEdge edge;
            if (isOnEdgeOrCorner(event->pos(), edge)) {
                m_resizeEdge = edge;
                switch (edge) {
                case TopLeft:
                case BottomRight:
                    setCursor(QCursor(Qt::SizeFDiagCursor));
                    break;
                case TopRight:
                case BottomLeft:
                    setCursor(QCursor(Qt::SizeBDiagCursor));
                    break;
                case Left:
                case Right:
                    setCursor(QCursor(Qt::SizeHorCursor));
                    break;
                case Top:
                case Bottom:
                    setCursor(QCursor(Qt::SizeVerCursor));
                    break;
                default:
                    break;
                }
            } else {
                m_resizeEdge = None;
                m_dragOffset = event->scenePos() - pos();
                setCursor(QCursor(Qt::ClosedHandCursor));
            }
        }
    }
    QGraphicsItem::mousePressEvent(event);
}

void CellItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (!event || !scene()) {
        qDebug() << "无效的事件或场景";
        return;
    }

    if (event->buttons() & Qt::LeftButton) {
        if (!scene()->property("startItem").isNull()) {
            // 连线模式：由 CanvasScene 处理临时连线
            event->accept();
        } else if (m_resizeEdge != None) {
            // 调整矩形大小（边框或角点）
            QRectF rect = boundingRect();
            QPointF localPos = mapFromScene(event->scenePos());
            QSizeF newSize = m_size;
            QPointF newPos = pos();

            switch (m_resizeEdge) {
            case Left: // 调整左边，保持右边固定
                newSize.setWidth(rect.width() - localPos.x());
                newPos.setX(event->scenePos().x());
                break;
            case Right: // 调整右边，保持左边固定
                newSize.setWidth(localPos.x());
                break;
            case Top: // 调整上边，保持下边固定
                newSize.setHeight(rect.height() - localPos.y());
                newPos.setY(event->scenePos().y());
                break;
            case Bottom: // 调整下边，保持上边固定
                newSize.setHeight(localPos.y());
                break;
            case TopLeft: // 调整左上角，保持右下角固定
                newSize.setWidth(rect.width() - localPos.x());
                newSize.setHeight(rect.height() - localPos.y());
                newPos.setX(event->scenePos().x());
                newPos.setY(event->scenePos().y());
                break;
            case TopRight: // 调整右上角，保持左下角固定
                newSize.setWidth(localPos.x());
                newSize.setHeight(rect.height() - localPos.y());
                newPos.setY(event->scenePos().y());
                break;
            case BottomLeft: // 调整左下角，保持右上角固定
                newSize.setWidth(rect.width() - localPos.x());
                newSize.setHeight(localPos.y());
                newPos.setX(event->scenePos().x());
                break;
            case BottomRight: // 调整右下角，保持左上角固定
                newSize.setWidth(localPos.x());
                newSize.setHeight(localPos.y());
                break;
            default:
                break;
            }

            // 限制大小
            restrictSize(newSize);
            // 更新引脚位置
            setSize(newSize); // setSize 会更新引脚位置和连线
            // 调整位置（仅对需要移动的边缘/角点）
            if (m_resizeEdge == Left || m_resizeEdge == Top || m_resizeEdge == TopLeft ||
                m_resizeEdge == TopRight || m_resizeEdge == BottomLeft) {
                restrictPosition(newPos);
                setPos(newPos);  // 实际设置新位置
                updateConnectedLines(); // 位置改变时更新连线
            }

            // 更新重合状态
            updateOverlapState();
            // 通知其他重合的矩形也更新状态
            for (CellItem* other : m_overlappingItems) {
                if (other) {
                    other->updateOverlapState();
                }
            }

            qDebug() << "调整大小，边：" << m_resizeEdge << ", 新大小：" << newSize << ", 新位置：" << pos();
            event->accept();
        } else {
            // 拖动整个矩形
            QPointF newPos = event->scenePos() - m_dragOffset;
            restrictPosition(newPos);
            setPos(newPos);  // 实际设置新位置

            // 更新重合状态
            updateOverlapState();
            // 通知其他重合的矩形也更新状态
            for (CellItem* other : m_overlappingItems) {
                if (other) {
                    other->updateOverlapState();
                }
            }

            qDebug() << "拖动，新位置：" << newPos;
            event->accept();
        }
    }
    // 避免调用基类的默认拖动行为
    // QGraphicsItem::mouseMoveEvent(event);
}

void CellItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!event || !scene()) {
        qDebug() << "无效的事件或场景";
        return;
    }
    qDebug() << "图元：鼠标释放位置:" << event->scenePos();

    // 检查是否进行了位置移动，如果是则创建撤销命令
    if (event->button() == Qt::LeftButton && m_resizeEdge == None) {
        // 检查是否在组移动模式中，如果是则不创建单个撤销命令
        CanvasScene* canvasScene = dynamic_cast<CanvasScene*>(scene());

        if (!canvasScene || !canvasScene->isGroupMoving()) {
            QPointF currentPos = pos();
            // 只有当位置真正发生变化时才创建撤销命令（距离大于1像素）
            if (QLineF(m_dragStartPosition, currentPos).length() > 1.0) {
                if (canvasScene && canvasScene->getUndoStack()) {
                    MoveCellCommand* moveCommand = new MoveCellCommand(this, m_dragStartPosition, currentPos);
                    canvasScene->getUndoStack()->push(moveCommand);
                    m_lastRecordedPosition = currentPos;
                    qDebug() << "创建移动撤销命令: 从" << m_dragStartPosition << "到" << currentPos;
                }
            }
        }
    }

    m_resizeEdge = None;
    m_dragOffset = QPointF(0, 0);
    setCursor(QCursor(Qt::OpenHandCursor));
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void CellItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    onDoubleClick();
}

void CellItem::addConnector(const QString& side, qreal percentage, qreal size, const QString& id, qreal x, qreal y)
{
    if (id.isEmpty()) {
        qWarning() << "Cannot add connector with empty id";
        return;
    }

    percentage = qBound(0.0, percentage, 100.0);

    // 如果是边缘引脚但x,y为0，需要重新计算位置
    if (side != "custom" && (qAbs(x) < 0.01 && qAbs(y) < 0.01)) {
        QPointF calculatedPos;
        if (side == "top") {
            calculatedPos = QPointF(m_size.width() * percentage / 100.0, 0);
        } else if (side == "bottom") {
            calculatedPos = QPointF(m_size.width() * percentage / 100.0, m_size.height());
        } else if (side == "left") {
            calculatedPos = QPointF(0, m_size.height() * percentage / 100.0);
        } else if (side == "right") {
            calculatedPos = QPointF(m_size.width(), m_size.height() * percentage / 100.0);
        }
        x = calculatedPos.x();
        y = calculatedPos.y();
    }

    Connector connector(side, percentage, id, x, y);
    m_connectors.append(connector);

    PinItem* pinItem = new PinItem(nullptr, size, this);  // 不设置parentRect，使用CellItem作为parent
    pinItem->setZValue(1); // 确保引脚显示在芯片之上

    // 使用绝对坐标：直接使用计算出的引脚位置
    QPointF pinPos = connector.calculatePos(m_size, size);
    pinItem->setPos(pinPos);  // 这是相对于CellItem的本地坐标
    pinItem->updateConnector(id, pinPos.x(), pinPos.y(), side, percentage);
    m_pinItems.append(pinItem);

    update();
    qDebug() << "Added PinItem for connector:" << id << "原始坐标: (" << x << "," << y << ")"
             << "边缘:" << side << "百分比:" << percentage << "本地位置: (" << pinPos.x() << "," << pinPos.y() << ")"
             << "芯片尺寸:" << m_size;
    qDebug() << "After addConnector: pinItems=" << m_pinItems.size() << ", connectors=" << m_connectors.size();

    // 更新宏名称
    updateMacroName();
}

bool CellItem::removeConnector(const QString& id) {
    if (id.isEmpty()) {
        qWarning() << "Cannot remove connector with empty id";
        return false;
    }

    int index = -1;
    for (int i = 0; i < m_connectors.size(); ++i) {
        if (m_connectors[i].id == id) {
            index = i;
            break;
        }
    }

    if (index >= 0 && index < m_connectors.size() && index < m_pinItems.size()) {
        return removeConnector(index);
    }

    qWarning() << "Connector id=" << id << " not found or index out of range";
    return false;
}

bool CellItem::removeConnector(int index) {
    if (index < 0 || index >= m_connectors.size() || index >= m_pinItems.size()) {
        qWarning() << "Invalid index for removeConnector: index=" << index
                   << ", connectors=" << m_connectors.size()
                   << ", pinItems=" << m_pinItems.size();
        return false;
    }

    QString removedId = m_connectors[index].id;

    // 从场景中移除引脚
    PinItem* pinItem = m_pinItems[index];
    if (pinItem) {
        if (pinItem->scene()) {
            pinItem->scene()->removeItem(pinItem);
        }
        delete pinItem;
    }

    // 从列表中移除引脚和连接器
    m_pinItems.removeAt(index);
    m_connectors.removeAt(index);

    // 移除与此引脚相关的所有连线
    for (int i = m_connections.size() - 1; i >= 0; --i) {
        if (m_connections[i].second.first == removedId || m_connections[i].second.second == removedId) {
            m_connections.removeAt(i);
        }
    }

    update();
    qDebug() << "Removed connector at index=" << index << ", remaining connectors=" << m_connectors.size()
             << ", remaining pinItems=" << m_pinItems.size();

    // 更新宏名称
    updateMacroName();

    return true;
}

void CellItem::updateConnector(int index, const QString& side, qreal percentage, qreal x, qreal y)
{
    if (index >= 0 && index < m_connectors.size()) {
        m_connectors[index].side = side;
        m_connectors[index].percentage = percentage;
        m_connectors[index].x = x;
        m_connectors[index].y = y;

        if (index < m_pinItems.size()) {
            m_pinItems[index]->updateConnector(m_connectors[index].id, static_cast<qreal>(x), static_cast<qreal>(y));
        }

        update();
    }
}

bool CellItem::isOnConnector(const QPointF& pos, Connector& connector) const
{
    for (const auto& conn : m_connectors) {
        QPointF pinPos = conn.calculatePos(m_size, connectorSize);
        QRectF pinRect(pinPos.x() - connectorSize/2, pinPos.y() - connectorSize/2,
                       connectorSize, connectorSize);
        if (pinRect.contains(pos)) {
            connector = conn;
            return true;
        }
    }
    return false;
}

void CellItem::addConnection(CellItem* targetCell, const QString& sourcePin, const QString& targetPin) {
    if (!targetCell) {
        qWarning() << "Cannot add connection to null target cell";
        return;
    }

    // 检查源引脚是否存在
    bool sourceExists = false;
    for (const Connector& conn : m_connectors) {
        if (conn.id == sourcePin) {
            sourceExists = true;
            break;
        }
    }

    // 检查目标引脚是否存在
    bool targetExists = false;
    for (const Connector& conn : targetCell->getConnectors()) {
        if (conn.id == targetPin) {
            targetExists = true;
            break;
        }
    }

    if (!sourceExists || !targetExists) {
        qWarning() << "Cannot add connection: source or target pin does not exist";
        return;
    }

    // 添加连接
    m_connections.append(qMakePair(targetCell, qMakePair(sourcePin, targetPin)));
    // qDebug() << "Added connection from" << sourcePin << "to" << targetPin;
    update();
}

void CellItem::removeConnection(CellItem* targetCell, const QString& sourcePin, const QString& targetPin)
{
    if (!targetCell) {
        qWarning() << "Cannot remove connection: target cell is null";
        return;
    }

    // 查找并删除连接
    for (int i = 0; i < m_connections.size(); ++i) {
        const auto& connection = m_connections[i];
        if (connection.first == targetCell &&
            connection.second.first == sourcePin &&
            connection.second.second == targetPin) {
            m_connections.removeAt(i);
            qDebug() << "Removed connection from" << sourcePin << "to" << targetPin;
            update();
            return;
        }
    }

    qWarning() << "Connection not found:" << sourcePin << "to" << targetPin;
}

QPainterPath CellItem::getShape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

bool CellItem::isOverlapping(const CellItem* other) const
{
    if (!other) return false;

    QRectF thisRect = QRectF(pos(), m_size);
    QRectF otherRect = QRectF(other->pos(), other->m_size);

    return thisRect.intersects(otherRect);
}

QPainterPath CellItem::getOverlapArea(const CellItem* other) const
{
    QPainterPath path;
    if (isOverlapping(other)) {
        QRectF thisRect = QRectF(pos(), m_size);
        QRectF otherRect = QRectF(other->pos(), other->m_size);
        QRectF intersection = thisRect.intersected(otherRect);
        path.addRect(intersection);
    }
    return path;
}

void CellItem::updateOverlapState()
{
    // 更新重叠状态的实现
    m_overlapAreas.clear();
    m_overlappingItems.clear();

    if (scene()) {
        QList<QGraphicsItem*> items = scene()->items();
        for (QGraphicsItem* item : items) {
            if (CellItem* other = dynamic_cast<CellItem*>(item)) {
                if (other != this && isOverlapping(other)) {
                    m_overlappingItems.append(other);
                    m_overlapAreas.append(getOverlapArea(other));
                }
            }
        }
    }

    // 重新绘制以显示重叠效果
    update();
}

QJsonObject CellItem::toJson() const {
    QJsonObject json;

    // 基本属性
    json["macroName"] = m_macroName;
    json["instanceName"] = m_instanceName;
    json["width"] = m_size.width();
    json["height"] = m_size.height();
    json["posX"] = pos().x();
    json["posY"] = pos().y();

    // 引脚信息
    QJsonArray pinsArray;
    for (const Connector& conn : m_connectors) {
        QJsonObject pinJson;
        pinJson["id"] = conn.id;
        pinJson["side"] = conn.side;
        pinJson["percentage"] = conn.percentage;
        pinJson["x"] = conn.x;
        pinJson["y"] = conn.y;
        pinsArray.append(pinJson);
    }
    json["pins"] = pinsArray;

    // 连线信息
    QJsonArray connectionsArray;
    for (const auto& conn : m_connections) {
        QJsonObject connJson;
        connJson["targetCell"] = conn.first ? conn.first->getInstanceName() : "";
        connJson["sourcePin"] = conn.second.first;
        connJson["targetPin"] = conn.second.second;
        connectionsArray.append(connJson);
    }
    json["connections"] = connectionsArray;

    return json;
}

void CellItem::fromJson(const QJsonObject& json) {
    // 清理现有数据
    for (PinItem* pin : m_pinItems) {
        if (pin && pin->scene()) {
            pin->scene()->removeItem(pin);
        }
        delete pin;
    }
    m_pinItems.clear();
    m_connectors.clear();
    m_connections.clear();

    // 加载基本属性
    m_macroName = json["macroName"].toString();
    m_instanceName = json["instanceName"].toString();
    setSize(QSizeF(json["width"].toDouble(), json["height"].toDouble()));
    setPos(json["posX"].toDouble(), json["posY"].toDouble());

    // 加载引脚信息
    QJsonArray pinsArray = json["pins"].toArray();
    for (const QJsonValue& pinValue : pinsArray) {
        QJsonObject pinJson = pinValue.toObject();
        QString id = pinJson["id"].toString();
        QString side = pinJson["side"].toString();
        qreal percentage = static_cast<qreal>(pinJson["percentage"].toDouble());
        qreal x = static_cast<qreal>(pinJson["x"].toDouble());
        qreal y = static_cast<qreal>(pinJson["y"].toDouble());

        // 添加引脚
        addConnector(side, percentage, connectorSize, id, x, y);
    }

    // 连线信息需要在所有CellItem都加载完成后处理
    // 这里只记录连线信息，实际连线在外部完成

    update();
}

QVariant CellItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange) {
        QPointF newPos = value.toPointF();

        // 添加安全检查，避免在拖动时崩溃
        if (scene()) {
            QRectF sceneRect = scene()->sceneRect();
            // 如果场景矩形无效，跳过位置限制
            if (sceneRect.isValid() && !sceneRect.isNull()) {
                restrictPosition(newPos);
            }
        }
        return newPos;
    } else if (change == ItemPositionHasChanged && !m_updatingOverlap) {
        // 位置已经改变，更新重叠状态和连线
        m_updatingOverlap = true;
        updateOverlapState();

        // 更新与此芯片相关的所有连线
        updateConnectedLines();

        // 通知场景中所有其他CellItem也更新重叠状态
        if (scene()) {
            QList<QGraphicsItem*> items = scene()->items();
            for (QGraphicsItem* item : items) {
                if (CellItem* other = dynamic_cast<CellItem*>(item)) {
                    if (other != this && !other->m_updatingOverlap) {
                        other->m_updatingOverlap = true;
                        other->updateOverlapState();
                        other->m_updatingOverlap = false;
                    }
                }
            }
        }
        m_updatingOverlap = false;
    }
    return QGraphicsItem::itemChange(change, value);
}

bool CellItem::isOnEdgeOrCorner(const QPointF &pos, ResizeEdge &edge) const
{
    QRectF rect = boundingRect();
    qreal x = pos.x();
    qreal y = pos.y();
    edge = None;

    // 获取当前视图的缩放因子
    qreal scaleFactor = 1.0;
    if (scene() && !scene()->views().isEmpty()) {
        QGraphicsView* view = scene()->views().first();
        QTransform transform = view->transform();
        scaleFactor = qSqrt(transform.m11() * transform.m11() + transform.m12() * transform.m12());
    }

    // 根据缩放因子调整检测区域大小
    // 在高缩放时检测区域更小，在低缩放时检测区域更大，保持屏幕上的视觉大小一致
    qreal adaptiveEdgeWidth = edgeWidth;
    qreal adaptiveCornerSize = cornerSize;

    if (scaleFactor > 0) {
        // 让检测区域在屏幕上保持大约5-8像素的固定大小
        adaptiveEdgeWidth = 6.0 / scaleFactor;
        adaptiveCornerSize = 8.0 / scaleFactor;
    }

    // 限制检测区域的最小和最大值
    adaptiveEdgeWidth = qBound(2.0, adaptiveEdgeWidth, 20.0);
    adaptiveCornerSize = qBound(3.0, adaptiveCornerSize, 25.0);

    // 检测角点（优先级高于边框）
    if (qAbs(x) <= adaptiveCornerSize && qAbs(y) <= adaptiveCornerSize) {
        edge = TopLeft;
    } else if (qAbs(x - rect.width()) <= adaptiveCornerSize && qAbs(y) <= adaptiveCornerSize) {
        edge = TopRight;
    } else if (qAbs(x) <= adaptiveCornerSize && qAbs(y - rect.height()) <= adaptiveCornerSize) {
        edge = BottomLeft;
    } else if (qAbs(x - rect.width()) <= adaptiveCornerSize && qAbs(y - rect.height()) <= adaptiveCornerSize) {
        edge = BottomRight;
    }
    // 检测边框
    else if (x >= -adaptiveEdgeWidth && x <= adaptiveEdgeWidth) {
        edge = Left;
    } else if (x >= rect.width() - adaptiveEdgeWidth && x <= rect.width() + adaptiveEdgeWidth) {
        edge = Right;
    } else if (y >= -adaptiveEdgeWidth && y <= adaptiveEdgeWidth) {
        edge = Top;
    } else if (y >= rect.height() - adaptiveEdgeWidth && y <= rect.height() + adaptiveEdgeWidth) {
        edge = Bottom;
    }

    return edge != None;
}

void CellItem::restrictSizeAndPosition(QSizeF& size, QPointF& pos)
{
    restrictSize(size);
    restrictPosition(pos);
}

void CellItem::restrictSize(QSizeF& size)
{
    // 对于从文件加载的芯片，不应用尺寸限制，保持原始尺寸
    // 只做基本的有效性检查
    if (size.width() <= 0) size.setWidth(1.0);
    if (size.height() <= 0) size.setHeight(1.0);

    // 移除最小尺寸限制，允许加载小尺寸芯片
    // const qreal minSize = 5.0;
    // const qreal maxSize = 1000.0;
    // size.setWidth(qBound(minSize, size.width(), maxSize));
    // size.setHeight(qBound(minSize, size.height(), maxSize));
}

void CellItem::restrictPosition(QPointF& pos)
{
    // 完全移除边界限制，允许芯片自由移动
    Q_UNUSED(pos);
    // 如果将来需要添加特定的限制，可以在这里实现
}

void CellItem::onDoubleClick()
{
    // 显示引脚编辑对话框
    QUndoStack* undoStack = nullptr;
    if (scene()) {
        CanvasScene* canvasScene = qobject_cast<CanvasScene*>(scene());
        if (canvasScene) {
            undoStack = canvasScene->getUndoStack();
        }
    }

    PinEditorDialog dialog(this, undoStack);
    if (dialog.exec() == QDialog::Accepted) {
        // 对话框关闭后，所有更改已经应用到CellItem
        update();
    }
}

void CellItem::updatePinPositions()
{
    for (int i = 0; i < m_connectors.size() && i < m_pinItems.size(); ++i) {
        QPointF pos = m_connectors[i].calculatePos(m_size, connectorSize);

        // 只有非custom类型的引脚才更新存储的x,y坐标
        // custom类型的引脚保持原始坐标不变
        if (m_connectors[i].side != "custom") {
            m_connectors[i].x = pos.x();
            m_connectors[i].y = pos.y();
        }

        // 更新引脚位置显示
        m_pinItems[i]->setPos(pos);
        m_pinItems[i]->updateConnector(m_connectors[i].id, pos.x(), pos.y(), m_connectors[i].side, m_connectors[i].percentage);
    }
}

QPointF CellItem::Connector::calculatePos(const QSizeF& cellSize, qreal pinSize) const
{
    Q_UNUSED(pinSize)

    if (side == "custom") {
        return QPointF(x, y);
    }

    qreal px, py;
    if (side == "top") {
        px = cellSize.width() * percentage / 100.0;
        py = 0;
    } else if (side == "bottom") {
        px = cellSize.width() * percentage / 100.0;
        py = cellSize.height();
    } else if (side == "left") {
        px = 0;
        py = cellSize.height() * percentage / 100.0;
    } else if (side == "right") {
        px = cellSize.width();
        py = cellSize.height() * percentage / 100.0;
    } else {
        // 对于custom位置，直接使用x,y坐标
        // 这些坐标应该已经是相对于芯片左上角的正确位置
        px = x;
        py = y;
    }

    return QPointF(px, py);
}

void CellItem::updateConnectedLines()
{
    // 更新与此芯片相关的所有连线
    if (!scene()) return;

    QList<QGraphicsItem*> sceneItems = scene()->items();
    for (QGraphicsItem* item : sceneItems) {
        if (ConnectionLine* line = dynamic_cast<ConnectionLine*>(item)) {
            // 检查连线是否有效且连接到当前芯片
            if (line && (line->getStartItem() == this || line->getEndItem() == this)) {
                line->updatePosition();
            }
        }
    }
}

void CellItem::updateMacroName()
{
    // 根据引脚数量生成宏名称：MC + 引脚数量
    int pinCount = m_connectors.size();
    // 引脚数量可以为0，对应MC0

    QString newMacroName = "MC" + QString::number(pinCount);
    if (m_macroName != newMacroName) {
        m_macroName = newMacroName;
        update(); // 重新绘制以显示新的名称
        qDebug() << "更新宏名称为:" << m_macroName << "基于引脚数量:" << pinCount;
    }
}
