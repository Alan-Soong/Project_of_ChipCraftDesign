#include "dialogs.h"
#include "ui_dialogs.h"
#include "cellitem.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileDialog>
#include <QMouseEvent>
#include <QTimer>

// 在 dialogs.cpp 中实现
PinItem::PinItem(QGraphicsItem* parentRect, qreal size, QGraphicsItem* parent)
    : QGraphicsEllipseItem(0, 0, size, size, parent), parentRect(parentRect) {
    // setFlag(ItemIsMovable, true); // 启用鼠标拖动
    // setFlag(ItemIsMovable, false); // 禁用拖动
    setPen(Qt::NoPen);
    setFlags(ItemIsMovable | ItemSendsGeometryChanges); // 启用拖动
    setAcceptHoverEvents(true);
    setBrush(QColor("darkblue")); // 引脚颜色
}

QVariant PinItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    // if (change == ItemPositionChange && parentRect) {
    //     QPointF newPos = value.toPointF();
    //     QRectF rect = parentRect->boundingRect();
    //     // 限制在父项的局部坐标系内
    //     qreal x = qMax(0.0, qMin(newPos.x(), rect.width() - boundingRect().width()));
    //     qreal y = qMax(0.0, qMin(newPos.y(), rect.height() - boundingRect().height()));
    //     return QPointF(x, y);
    // }
    // return QGraphicsEllipseItem::itemChange(change, value);
    if (change == ItemPositionChange && parentRect) {
        QPointF newPos = value.toPointF();
        QRectF rect = parentRect->boundingRect();
        qreal pinSize = boundingRect().width();
        qreal x = qBound(0.0, newPos.x(), rect.width() - pinSize);
        qreal y = qBound(0.0, newPos.y(), rect.height() - pinSize);
        return QPointF(x, y);
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}

void PinItem::updateConnector(const QString& id, qreal x, qreal y) {
    // m_side = side;
    // m_percentage = percentage;
    m_id = id;
    m_x = x;
    m_y = y;
    qDebug() << "PinItem updated: id=" << m_id << ", x=" << m_x << ", y=" << m_y;
}

QPointF PinItem::restrictToEdge(const QPointF& pos, qreal width, qreal height, QString& side, qreal& percentage) const {
    qreal x = pos.x();
    qreal y = pos.y();
    qreal margin = 20.0; // 吸附范围
    side.clear();
    percentage = 0.0;

    // 计算到各边的距离
    qreal distTop = std::abs(y);
    qreal distBottom = std::abs(y - height);
    qreal distLeft = std::abs(x);
    qreal distRight = std::abs(x - width);

    // 选择最近的边
    qreal minDist = std::min({distTop, distBottom, distLeft, distRight});
    if (minDist > margin) return pos; // 超出吸附范围

    if (minDist == distTop) {
        side = "top";
        x = qBound(0.0, x, width);
        y = 0;
        percentage = x / width * 100.0;
    } else if (minDist == distBottom) {
        side = "bottom";
        x = qBound(0.0, x, width);
        y = height;
        percentage = x / width * 100.0;
    } else if (minDist == distLeft) {
        side = "left";
        x = 0;
        y = qBound(0.0, y, height);
        percentage = y / height * 100.0;
    } else if (minDist == distRight) {
        side = "right";
        x = width;
        y = qBound(0.0, y, height);
        percentage = y / height * 100.0;
    }

    percentage = qBound(0.0, percentage, 100.0);
    return QPointF(x, y);
}

void PinItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    // if (!parentRect) return;

    // // 获取芯片尺寸
    // QRectF rect = parentRect->boundingRect();
    // qreal width = rect.width();
    // qreal height = rect.height();

    // // 计算新位置，限制到边缘
    // QString newSide;
    // qreal newPercentage;
    // QPointF newPos = restrictToEdge(event->scenePos() - parentRect->scenePos(), width, height, newSide, newPercentage);

    // if (!newSide.isEmpty()) {
    //     // 更新位置（相对于 parentRect）
    //     setPos(newPos);
    //     // 临时存储新 side 和 percentage
    //     m_side = newSide;
    //     m_percentage = newPercentage;
    // }

    // QGraphicsEllipseItem::mouseMoveEvent(event); // 使用默认拖动
    // qDebug() << "PinItem moved: id=" << m_id << ", pos=" << pos();
    if (!parentRect) {
        qWarning() << "PinItem: parentRect is null in mouseMoveEvent";
        return;
    }
    QGraphicsEllipseItem::mouseMoveEvent(event); // 使用默认拖动
    qDebug() << "PinItem moved: id=" << m_id << ", pos=" << pos();
}

void PinItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    // if (!parentRect) return;

    // // 确认最终位置
    // QRectF rect = parentRect->boundingRect();
    // qreal width = rect.width();
    // qreal height = rect.height();
    // QString newSide;
    // qreal newPercentage;
    // QPointF newPos = restrictToEdge(pos(), width, height, newSide, newPercentage);

    // if (!newSide.isEmpty()) {
    //     setPos(newPos);
    //     updateConnector(newSide, newPercentage, m_id);
    //     // 通知 Dialogs 引脚移动
    //     if (scene()) {
    //         if (auto* dialogs = qobject_cast<Dialogs*>(scene()->parent())) {
    //             dialogs->pinMoved(this);
    //         }
    //     }
    // }

    // // QGraphicsRectItem::mouseReleaseEvent(event);
    if (!parentRect) {
        qWarning() << "PinItem: parentRect is null in mouseReleaseEvent";
        return;
    }
    QGraphicsEllipseItem::mouseReleaseEvent(event);
    updateConnector(m_id, pos().x(), pos().y());
    if (scene()) {
        if (auto* dialogs = qobject_cast<Dialogs*>(scene()->parent())) {
            dialogs->on_pinMoved(this);
            qDebug() << "Notified Dialogs::on_pinMoved for id=" << m_id;
        } else {
            qWarning() << "Failed to cast scene parent to Dialogs";
        }
    }
}

Dialogs::Dialogs(QGraphicsItem* item, QWidget *parent)
    : QDialog(parent), targetItem(item), ui(new Ui::Dialogs)
    , pinScene(new QGraphicsScene(this))
{
    ui->setupUi(this);
    setWindowTitle("设置");
    ui->addPinButton->setAutoRepeat(false);
    if (!ui->addPinButton) {
        qWarning() << "ui->addPinButton is null! Check dialogs.ui objectName.";
    } else {
        disconnect(ui->addPinButton, &QPushButton::clicked, this, &Dialogs::on_addPinButton_clicked);
        qDebug() << "Connecting addPinButton signal";
        connect(ui->addPinButton, &QPushButton::clicked, this, &Dialogs::on_addPinButton_clicked);
    }
    connect(ui->nameEdit, &QLineEdit::textChanged, this, &Dialogs::on_nameEdit_textEdited);
    connect(ui->colorCombo, QOverload<int>::of(&QComboBox::activated), this, &Dialogs::on_colorCombo_activated);
    // connect(ui->addPinButton, &QPushButton::clicked, this, &Dialogs::on_addPinButton_clicked);
    ui->pinGraphicsView->installEventFilter(this);
    currentInfo.width = 100;
    currentInfo.height = 100;
    setupPinScene();
    // loadFromFile("component.txt");
}

Dialogs::~Dialogs()
{
    pinItems.clear(); // 清空列表，但不删除 PinItem
    delete pinScene;
    delete ui;
}

void Dialogs::setupPinScene() {
    ui->pinGraphicsView->setScene(pinScene);
    chipRect = new QGraphicsRectItem(0, 0, 100, 100);
    chipRect->setPen(QPen(Qt::black));
    chipRect->setBrush(Qt::lightGray);
    pinScene->addItem(chipRect);
    ui->pinGraphicsView->setSceneRect(0, 0, 100, 100);
    ui->pinGraphicsView->setRenderHint(QPainter::Antialiasing);

    // 连接点击事件
    ui->pinGraphicsView->viewport()->installEventFilter(this);
}

bool Dialogs::eventFilter(QObject* obj, QEvent* event) {
    // if (obj == ui->pinGraphicsView->viewport() && event->type() == QEvent::MouseButtonPress) {
    //     if (ui->addPinButton->underMouse()) {
    //         qDebug() << "Ignoring mouse press over addPinButton";
    //         return QDialog::eventFilter(obj, event);
    //     }
    //     QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    //     if (mouseEvent) {
    //         QPointF scenePos = ui->pinGraphicsView->mapToScene(mouseEvent->pos());
    //         on_pinSceneClicked(scenePos);
    //         return true;
    //     } else {
    //         qWarning() << "Failed to cast QEvent to QMouseEvent";
    //     }
    // }
    if (obj == ui->pinGraphicsView->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF scenePos = ui->pinGraphicsView->mapToScene(mouseEvent->pos());
        qDebug() << "Mouse clicked in pinGraphicsView, scenePos=" << scenePos << ", m_addingPin=" << m_addingPin;
        on_pinSceneClicked(scenePos);
        return true;
    }
    // // 调试其他鼠标事件
    // if (event->type() == QEvent::MouseButtonPress) {
    //     QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    //     qDebug() << "Mouse press detected, obj=" << obj << ", pos=" << mouseEvent->pos();
    // }
    return QDialog::eventFilter(obj, event);
}

void Dialogs::on_pinSceneClicked(const QPointF& pos) {
    // QRectF rect = chipRect->boundingRect();
    // QString side;
    // qreal percentage = 0.0;

    // // 判断点击的边
    // qreal edgeThreshold = 10.0;
    // if (pos.y() <= edgeThreshold) {
    //     side = "top";
    //     percentage = (pos.x() / rect.width()) * 100.0;
    // } else if (pos.y() >= rect.height() - edgeThreshold) {
    //     side = "bottom";
    //     percentage = (pos.x() / rect.width()) * 100.0;
    // } else if (pos.x() <= edgeThreshold) {
    //     side = "left";
    //     percentage = (pos.y() / rect.height()) * 100.0;
    // } else if (pos.x() >= rect.width() - edgeThreshold) {
    //     side = "right";
    //     percentage = (pos.y() / rect.height()) * 100.0;
    // }

    // if (!side.isEmpty()) {
    //     percentage = qBound(0.0, percentage, 100.0);
    //     QString id = QString("pin_%1").arg(pinItems.size() + 1);
    //     addPin(side, percentage, 10, id);
    //     updatePinScene();
    //     qDebug() << "Clicked on" << side << "at percentage" << percentage;
    //     percentage;
    // }
    qDebug() << "on_pinSceneClicked: pos=" << pos << ", m_addingPin=" << m_addingPin;
    if (!m_addingPin) {
        qDebug() << "Not in pin adding mode, ignoring click";
        return;
    }
    if (!m_addingPin) {
        qDebug() << "Not in pin adding mode, ignoring click";
        return;
    }

    qreal x = pos.x();
    qreal y = pos.y();
    QRectF rect = chipRect->boundingRect();
    // qreal width = currentInfo.width;
    // qreal height = currentInfo.height;
    qreal width = rect.width();
    qreal height = rect.height();
    QString side;
    qreal percentage;
    qreal margin = 10.0;

    if (x >= -margin && x <= width + margin && y >= -margin && y <= margin) {
        side = "top";
        percentage = x / width * 100.0;
    } else if (x >= -margin && x <= width + margin && y >= height - margin && y <= height + margin) {
        side = "bottom";
        percentage = x / width * 100.0;
    } else if (y >= -margin && y <= height + margin && x >= -margin && x <= margin) {
        side = "left";
        percentage = y / height * 100.0;
    } else if (y >= -margin && y <= height + margin && x >= width - margin && x <= width + margin) {
        side = "right";
        percentage = y / height * 100.0;
    }

    if (!side.isEmpty()) {
        percentage = qBound(0.0, percentage, 100.0);
        QString id = QString("pin_%1").arg(currentInfo.pins.size() + 1);

        addPin(side, percentage, 10, id);
        m_addingPin = false;
        ui->pinGraphicsView->setCursor(Qt::ArrowCursor);
        chipRect->setPen(QPen(Qt::black));
        // ui->addPinButton->setText("Add Pin");
        qDebug() << "Added pin via click: id=" << id << ", side=" << side << ", percentage=" << percentage;
        updatePinScene();
    } else {
        qDebug() << "Click outside valid pin area: pos=" << pos;
        // 不重置 m_addingPin，允许用户继续点击button
    }
}

void Dialogs::updatePinScene() {
    // 清除现有引脚
    for (QGraphicsItem* item : pinScene->items()) {
        if (item != chipRect) {
            pinScene->removeItem(item);
            // delete item;
        }
    }

    // 确保 chipRect 存在
    if (!chipRect) {
        chipRect = new QGraphicsRectItem(0, 0, currentInfo.width, currentInfo.height);
        chipRect->setPen(QPen(Qt::black));
        chipRect->setBrush(Qt::lightGray);
        pinScene->addItem(chipRect);
    }

    // // 添加引脚
    // for (const auto& conn : currentInfo.pins) {
    //     qreal pinSize = 10;
    //     qreal x = 0, y = 0;
    //     if (conn.side == "top") {
    //         x = 100 * conn.percentage / 100.0 - pinSize / 2;
    //         y = 0;
    //     } else if (conn.side == "bottom") {
    //         x = 100 * conn.percentage / 100.0 - pinSize / 2;
    //         y = 100 - pinSize;
    //     } else if (conn.side == "left") {
    //         x = 0;
    //         y = 100 * conn.percentage / 100.0 - pinSize / 2;
    //     } else if (conn.side == "right") {
    //         x = 100 - pinSize;
    //         y = 100 * conn.percentage / 100.0 - pinSize / 2;
    //     }
    //     PinItem* pin = new PinItem(nullptr, pinSize);
    //     pin->setPos(x, y);
    //     pinScene->addItem(pin);
    // }
    // qDebug() << "updatePinScene: pinItems=" << pinItems.size() << ", currentInfo.pins=" << currentInfo.pins.size();
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        auto cellPinItems = cellItem->getPinItems();
        auto connectors = cellItem->getConnectors();
        pinItems.clear();
        int count = qMin(cellPinItems.size(), connectors.size());
        for (int i = 0; i < count; ++i) {
            if (!cellPinItems[i]) {
                qWarning() << "Null PinItem at index" << i;
                continue;
            }
            PinItem* pin = cellPinItems[i];
            // pin->setParentItem(chipRect); // 设置 parentRect
            pin->updateConnector(connectors[i].id, connectors[i].x, connectors[i].y);
            pin->setPos(connectors[i].x, connectors[i].y);
            pinScene->addItem(pin);
            pinItems.append(pin);
            qDebug() << "Added PinItem to scene: id=" << connectors[i].id << ", x=" << connectors[i].x << ", y=" << connectors[i].y;
        }
        currentInfo.pins.clear();
        for (const auto& connector : connectors) {
            currentInfo.pins.append(CellItemNS::Connector(connector.side, connector.percentage, connector.id, connector.x, connector.y));
        }
    } else {
        qWarning() << "Target item is not a CellItem";
    }
}

void Dialogs::saveSetting() {
    // 这里添加保存设置的逻辑
    // 例如：将设置保存到一个配置文件或数据库
    if (targetItem) {
        QRectF rect = targetItem->boundingRect();
        currentInfo.width = static_cast<int>(rect.width());
        currentInfo.height = static_cast<int>(rect.height());
    } else {
        currentInfo.width = this->width();
        currentInfo.height = this->height();
    }
    if (ui->colorCombo && ui->colorCombo->currentIndex() >= 0) {
        currentInfo.color = ui->colorCombo->currentText();
    } else {
        currentInfo.color = "none";
    }
    updatePins();
    saveToFile("component.txt"); // 保存到文件
    emit settingsSaved(currentInfo); // 通知保存
    qDebug() << "Settings saved: pinItems=" << pinItems.size() << ", currentInfo.pins=" << currentInfo.pins.size();
    accept(); // 关闭对话框
}

// void Dialogs::accept() {
//     saveSetting(); // 关闭窗口前保存
//     QDialog::accept();
// }

// void Dialogs::reject() {
//     saveSetting(); // 即使取消也保存
//     QDialog::reject();
// }

void Dialogs::on_nameEdit_textEdited(const QString &arg1)
{
    currentInfo.name = arg1;
    qDebug() << "名称已更新为:" << currentInfo.name;
}


void Dialogs::on_saveButton_clicked()
{
    // // 弹出提示框
    // if (targetItem) {
    //     QRectF rect = targetItem->boundingRect();
    //     currentInfo.width = static_cast<int>(rect.width());
    //     currentInfo.height = static_cast<int>(rect.height());
    // } else {
    //     currentInfo.width = this->width();
    //     currentInfo.height = this->height();
    // }
    // qDebug() << "size:" << currentInfo.width << currentInfo.height;

    // /// 获取选中的颜色文本
    // if (ui->colorCombo && ui->colorCombo->currentIndex() >= 0) {
    //     currentInfo.color = ui->colorCombo->currentText(); // 直接获取选中的文本
    //     qDebug() << "Selected color:" << currentInfo.color;
    // } else {
    //     qDebug() << "No color selected or color combo box is invalid!";
    //     currentInfo.color = "none"; // 设置默认值
    // }
    // QMessageBox::information(this, "提示", "保存成功！");
    // accept(); // 关闭对话框
    if (targetItem) {
        QRectF rect = targetItem->boundingRect();
        currentInfo.width = static_cast<int>(rect.width());
        currentInfo.height = static_cast<int>(rect.height());
    } else {
        currentInfo.width = this->width();
        currentInfo.height = this->height();
    }
    qDebug() << "size:" << currentInfo.width << currentInfo.height;

    if (ui->colorCombo && ui->colorCombo->currentIndex() >= 0) {
        currentInfo.color = ui->colorCombo->currentText();
        qDebug() << "Selected color:" << currentInfo.color;
    } else {
        qDebug() << "No color selected or color combo box is invalid!";
        currentInfo.color = "none";
    }

    updatePins();
    QString pinInfo;
    // if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
    //     auto connectors = cellItem->getConnectors();
    //     auto pinItems = cellItem->getPinItems();
    //     for (int i = 0; i < connectors.size(); ++i) {
    //         const auto& conn = connectors[i];
    //         QRectF pinRect = pinItems[i]->boundingRect().translated(pinItems[i]->pos() + targetItem->pos());
    //         pinInfo += QString("Pin %1 (ID: %2):\n  Top-left: (%3, %4)\n  Top-right: (%5, %6)\n"
    //                            "  Bottom-left: (%7, %8)\n  Bottom-right: (%9, %10)\n")
    //                        .arg(i + 1).arg(conn.id)
    //                        .arg(pinRect.left()).arg(pinRect.top())
    //                        .arg(pinRect.right()).arg(pinRect.top())
    //                        .arg(pinRect.left()).arg(pinRect.bottom())
    //                        .arg(pinRect.right()).arg(pinRect.bottom());
    //     }
    // }
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        auto connectors = cellItem->getConnectors();
        auto pinItemsList = cellItem->getPinItems();
        if (connectors.size() != pinItemsList.size()) {
            qWarning() << "connectors and pinItems out of sync in on_saveButton_clicked: connectors=" << connectors.size()
            << ", pinItems=" << pinItemsList.size();
        }
        int count = qMin(connectors.size(), pinItemsList.size());
        for (int i = 0; i < count; ++i) {
            if (i >= pinItemsList.size() || i >= connectors.size()) {
                qWarning() << "Index out of range in on_saveButton_clicked: i=" << i << ", pinItems=" << pinItemsList.size()
                << ", connectors=" << connectors.size();
                break;
            }
            const auto& conn = connectors[i];
            PinItem* pinItem = pinItemsList[i];
            if (!pinItem) {
                qWarning() << "Null pinItem at index" << i << "in on_saveButton_clicked";
                continue;
            }
            QRectF pinRect = pinItem->boundingRect().translated(pinItem->pos() + targetItem->pos());
            pinInfo += QString("Pin %1 (ID: %2, Side: %3, Percentage: %4):\n"
                               "  Top-left: (%5, %6)\n  Top-right: (%7, %8)\n"
                               "  Bottom-left: (%9, %10)\n  Bottom-right: (%11, %12)\n")
                           .arg(i + 1).arg(conn.id).arg(conn.side).arg(conn.percentage)
                           .arg(pinRect.left()).arg(pinRect.top())
                           .arg(pinRect.right()).arg(pinRect.top())
                           .arg(pinRect.left()).arg(pinRect.bottom())
                           .arg(pinRect.right()).arg(pinRect.bottom());
        }
    }
    qDebug().noquote() << "Pins:\n" << pinInfo;

    QString message = pinInfo.isEmpty() ? "保存成功！" : QString("保存成功！\n引脚信息：\n%1").arg(pinInfo);
    QMessageBox::information(this, "提示", message);

    // saveToFile("component.json");
    saveToFile("component.txt");
    emit settingsSaved(currentInfo);
    accept();
}


void Dialogs::on_colorCombo_activated(int index)
{
    if (!ui->colorCombo) {
        qWarning() << "Color combo box is not valid!";
        return;
    }

    // if (index >= 0 && index < ui->colorCombo->count()) {
    //     currentInfo.color = ui->colorCombo->currentText(); // 直接获取选中的文本
    //     qDebug() << "Selected color:" << currentInfo.color;
    // } else {
    //     qDebug() << "Invalid index:" << index;
    // }
    // // 如果 targetItem 是有效的，并且是 QGraphicsRectItem 类型
    // if (targetItem) {
    //     QGraphicsRectItem* RectItem = dynamic_cast<QGraphicsRectItem*>(targetItem);
    //     if (RectItem) {
    //         RectItem->setBrush(QColor(currentInfo.color));
    //         qDebug() << "targetItem is colored!";
    //     } else {
    //         qDebug() << "targetItem is not a QGraphicsRectItem!";
    //     }
    // }
    if (index >= 0 && index < ui->colorCombo->count()) {
        currentInfo.color = ui->colorCombo->currentText();
        qDebug() << "Selected color:" << currentInfo.color;
        if (targetItem) {
            if (auto* rectItem = dynamic_cast<QGraphicsRectItem*>(targetItem)) {
                rectItem->setBrush(QColor(currentInfo.color));
            }
        }
    }
}

void Dialogs::addPin(const QString& side, qreal percentage, qreal size, const QString& id) {
    // if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
    //     int prevPinCount = cellItem->getPinItems().size();
    //     cellItem->addConnector(side, percentage, size, id);
    //     auto pinItemsList = cellItem->getPinItems();
    //     if (pinItemsList.size() > prevPinCount && !pinItemsList.isEmpty()) {
    //         // pinItems.append(pinItemsList.last());
    //         PinItem* pinItem = pinItemsList.last();
    //         pinItem->updateConnector(side, percentage, id); // 初始化 Connector 数据
    //         pinItems.append(pinItem);
    //         currentInfo.pins.append(CellItemNS::Connector(side, percentage, id));
    //         qDebug() << "Added pin:" << id << "at side=" << side << ", percentage=" << percentage;
    //     } else {
    //         qWarning() << "Failed to add pin: no new PinItem added for" << id;
    //     }
    //     if (pinItems.size() != currentInfo.pins.size()) {
    //         qWarning() << "pinItems and currentInfo.pins out of sync in addPin: pinItems=" << pinItems.size()
    //         << ", pins=" << currentInfo.pins.size();
    //     }
    // } else {
    //     qWarning() << "Target item is not a CellItem";
    // }
    // qDebug() << "After addPin: pinItems=" << pinItems.size() << ", currentInfo.pins=" << currentInfo.pins.size();
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        int prevPinCount = cellItem->getPinItems().size();
        qreal x = 0, y = 0;
        qreal chipWidth = currentInfo.width;
        qreal chipHeight = currentInfo.height;
        if (side == "top") {
            x = chipWidth * percentage / 100.0 - size / 2;
            y = 0;
        } else if (side == "bottom") {
            x = chipWidth * percentage / 100.0 - size / 2;
            y = chipHeight - size;
        } else if (side == "left") {
            x = 0;
            y = chipHeight * percentage / 100.0 - size / 2;
        } else if (side == "right") {
            x = chipWidth - size;
            y = chipHeight * percentage / 100.0 - size / 2;
        }
        cellItem->addConnector(side, percentage, size, id, x, y); // 更新 CellItem
        auto pinItemsList = cellItem->getPinItems();
        if (pinItemsList.size() > prevPinCount && !pinItemsList.isEmpty()) {
            PinItem* pinItem = pinItemsList.last();
            pinItem->updateConnector(id, x, y);
            pinItem->setParentRect(chipRect); // 使用 setter
            pinItems.append(pinItem);
            pinScene->addItem(pinItem);
            currentInfo.pins.append(CellItemNS::Connector(side, percentage, id, x, y));
            qDebug() << "Added pin:" << id << " at side=" << side << ", percentage=" << percentage << ", x=" << x << ", y=" << y;
        } else {
            qWarning() << "Failed to add pin: no new PinItem added for" << id;
        }
        if (pinItems.size() != currentInfo.pins.size()) {
            qWarning() << "Sync issue: pinItems=" << pinItems.size() << ", pins=" << currentInfo.pins.size();
        }
    } else {
        qWarning() << "Target item is not a CellItem";
    }
}

// 新增 Dialogs::on_pinMoved，处理引脚移动
void Dialogs::on_pinMoved(PinItem* pin) {
    if (!pin) return;

    QString id = pin->getId();
    QString newSide = pin->getSide();
    qreal newPercentage = pin->getPercentage();
    qreal newX = pin->getX();
    qreal newY = pin->getY();
    if (id.isEmpty()) {
        qWarning() << "on_pinMoved: pin id is empty";
        return;
    }

    // 更新 currentInfo.pins
    bool found = false;
    for (auto& connector : currentInfo.pins) {
        if (connector.id == id) {
            connector.side = newSide;
            connector.percentage = newPercentage;
            connector.x = newX;
            connector.y = newY;
            found = true;
            break;
        }
    }
    if (!found) {
        qWarning() << "Pin id=" << id << " not found in currentInfo.pins";
    }

    // 更新 CellItem 的引脚
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        auto connectors = cellItem->getConnectors();
        auto pinItems = cellItem->getPinItems();
        found = false;
        for (int i = 0; i < connectors.size() && i < pinItems.size(); ++i) {
            if (connectors[i].id == id && pinItems[i] == pin) {
                // 更新 CellItem 的 Connector
                // cellItem->updateConnector(i, newSide, newPercentage);
                cellItem->updateConnector(i, connectors[i].side, connectors[i].percentage, newX, newY);
                found = true;
                break;
            }
        }
        if (!found) {
            qWarning() << "Pin id=" << id << " not found in CellItem connectors or pinItems";
        }
    }

    qDebug() << "Pin moved: id=" << id << ", side=" << newSide << ", percentage=" << newPercentage;
    updatePinScene();
}

void Dialogs::updatePins() {
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        currentInfo.pins.clear();
        pinItems.clear();
        auto connectors = cellItem->getConnectors();
        auto pinItemsList = cellItem->getPinItems();
        if (connectors.size() != pinItemsList.size()) {
            qWarning() << "connectors and pinItems out of sync in updatePins: connectors=" << connectors.size()
            << ", pinItems=" << pinItemsList.size();
        }
        int count = qMin(connectors.size(), pinItemsList.size());
        for (int i = 0; i < count; ++i) {
            if (i >= pinItemsList.size() || i >= connectors.size()) {
                qWarning() << "Index out of range in updatePins: i=" << i << ", pinItems=" << pinItemsList.size()
                << ", connectors=" << connectors.size();
                break;
            }
            pinItems.append(pinItemsList[i]);
            currentInfo.pins.append(CellItemNS::Connector(connectors[i].side, connectors[i].percentage, connectors[i].id));
        }
        qDebug() << "updatePins completed: pinItems=" << pinItems.size() << ", currentInfo.pins=" << currentInfo.pins.size();
    }
    updatePinScene();
}

void Dialogs::on_addPinButton_clicked() {
    if (!targetItem) {
        qWarning() << "No targetItem in on_addPinButton_clicked";
        return;
    }
    QString side = ui->sideCombo->currentText();
    qreal percentage = ui->percentageSpin->value();
    qreal pinSize = 10;
    QString id = QString("pin_%1").arg(pinItems.size() + 1);
    addPin(side, percentage, pinSize, id);
    updatePinScene();
}

void Dialogs::saveToFile(const QString& defaultName) {
    QString path = QFileDialog::getSaveFileName(this, "Save Component", defaultName, "Text Files (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "name: " << currentInfo.name << "\n";
        out << "color: " << currentInfo.color << "\n";
        out << "width: " << currentInfo.width << "\n";
        out << "height: " << currentInfo.height << "\n";
        out << "pins:\n";
        for (const auto& conn : currentInfo.pins) {
            out << "  " << conn.id << ": side=" << conn.side << ", percentage=" << conn.percentage
                << ", x=" << conn.x << ", y=" << conn.y << ", size=10\n";
        }
        file.close();
        qDebug() << "Saved to" << path;
    } else {
        QMessageBox::warning(this, "错误", "无法保存文件：" + path);
    }
}

void Dialogs::loadFromFile(const QString& defaultName) {
    QString path = QFileDialog::getOpenFileName(this, "Load Component", defaultName, "Text Files (*.txt)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法加载文件：" + path);
        return;
    }

    QTextStream in(&file);
    QString line;
    QList<CellItemNS::Connector> newPins;
    while (!in.atEnd()) {
        line = in.readLine().trimmed();
        if (line.startsWith("name: ")) {
            currentInfo.name = line.mid(6);
        } else if (line.startsWith("color: ")) {
            currentInfo.color = line.mid(7);
        } else if (line.startsWith("width: ")) {
            currentInfo.width = line.mid(7).toInt();
        } else if (line.startsWith("height: ")) {
            currentInfo.height = line.mid(8).toInt();
        } else if (line.startsWith("pins:")) {
            while (!in.atEnd()) {
                line = in.readLine().trimmed();
                if (line.isEmpty()) break;
                QStringList parts = line.split(": ");
                if (parts.size() < 2) {
                    qWarning() << "Invalid pin format:" << line;
                    continue;
                }
                QString id = parts[0].trimmed();
                QStringList params = parts[1].split(", ");
                QString side;
                qreal percentage = 0.0, x = 0.0, y = 0.0,  size = 10;
                for (const QString& param : params) {
                    if (param.startsWith("side=")) side = param.mid(5);
                    else if (param.startsWith("percentage=")) percentage = param.mid(11).toDouble();
                    else if (param.startsWith("size=")) size = param.mid(5).toDouble();
                }
                if (!side.isEmpty()) {
                    newPins.append(CellItemNS::Connector(side, percentage, id));
                }
            }
        }
    }
    file.close();

    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        for (PinItem* pinItem : pinItems) {
            if (pinItem && pinItem->scene()) {
                pinItem->scene()->removeItem(pinItem);
            }
            delete pinItem;
        }
        pinItems.clear();
        currentInfo.pins.clear();
        while (!cellItem->getPinItems().isEmpty()) {
            PinItem* pinItem = cellItem->getPinItems().first();
            if (pinItem && pinItem->scene()) {
                pinItem->scene()->removeItem(pinItem);
            }
            delete pinItem;
            cellItem->getPinItems().removeFirst();
            cellItem->getConnectors().removeFirst();
        }
        for (const auto& conn : newPins) {
            addPin(conn.side, conn.percentage, 10, conn.id);
        }
        cellItem->setSize(QSizeF(currentInfo.width, currentInfo.height));
    }
    updatePinScene();
    qDebug() << "Loaded from" << path << ": pinItems=" << pinItems.size()
             << ", currentInfo.pins=" << currentInfo.pins.size();
}
