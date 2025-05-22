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
#include <QMessageBox>
#include <QDebug>

// PinItem 实现
PinItem::PinItem(QGraphicsItem* parentRect, qreal size, QGraphicsItem* parent)
    : QGraphicsEllipseItem(0, 0, size, size, parent), parentRect(parentRect) {
    setFlags(ItemIsMovable | ItemSendsGeometryChanges | ItemIsSelectable);
    setAcceptHoverEvents(true);
    setBrush(QColor("darkblue"));
}

QVariant PinItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && parentRect) {
        QPointF newPos = value.toPointF();
        QRectF rect = parentRect->boundingRect();
        qreal pinSize = boundingRect().width();
        
        // 允许在整个芯片区域内自由放置引脚，不再限制只能在边缘
        qreal x = qBound(0.0, newPos.x(), rect.width() - pinSize);
        qreal y = qBound(0.0, newPos.y(), rect.height() - pinSize);
        
        return QPointF(x, y);
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}

void PinItem::updateConnector(const QString& id, qreal x, qreal y) {
    m_id = id;
    m_x = x;
    m_y = y;
    qDebug() << "PinItem updated: id=" << m_id << ", x=" << m_x << ", y=" << m_y;
}

QPointF PinItem::restrictToEdge(const QPointF& pos, qreal width, qreal height, QString& side, qreal& percentage) const {
    qreal x = pos.x();
    qreal y = pos.y();
    qreal margin = 20.0;
    side.clear();
    percentage = 0.0;

    qreal distTop = std::abs(y);
    qreal distBottom = std::abs(y - height);
    qreal distLeft = std::abs(x);
    qreal distRight = std::abs(x - width);

    qreal minDist = std::min({distTop, distBottom, distLeft, distRight});
    
    // 如果距离边缘很近，则吸附到边缘
    if (minDist <= margin) {
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
    } else {
        // 如果不在边缘附近，则保持原位置，使用自定义side标识
        side = "custom";
        percentage = 0.0; // 对于自定义位置，percentage不再有意义
        return pos;
    }
}

void PinItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (!parentRect) {
        qWarning() << "PinItem: parentRect is null in mouseMoveEvent";
        return;
    }
    QGraphicsEllipseItem::mouseMoveEvent(event);
    qDebug() << "PinItem moved: id=" << m_id << ", pos=" << pos();
}

void PinItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
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

// Dialogs 构造函数
Dialogs::Dialogs(QGraphicsItem* item, QWidget *parent)
    : QDialog(parent), targetItem(item), ui(new Ui::Dialogs), pinScene(new QGraphicsScene(this)), selectedPin(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("设置");
    ui->addPinButton->setAutoRepeat(false);
    if (!ui->addPinButton) {
        qWarning() << "ui->addPinButton is null! Check dialogs.ui objectName.";
    } else {
        connect(ui->addPinButton, &QPushButton::clicked, this, &Dialogs::on_addPinButton_clicked);
    }
    if (!ui->removePinButton) {
        qWarning() << "ui->removePinButton is null! Check dialogs.ui objectName.";
    } else {
        connect(ui->removePinButton, &QPushButton::clicked, this, &Dialogs::on_removePinButton_clicked);
    }
    connect(ui->nameEdit, &QLineEdit::textChanged, this, &Dialogs::on_nameEdit_textEdited);
    connect(ui->colorCombo, QOverload<int>::of(&QComboBox::activated), this, &Dialogs::on_colorCombo_activated);
    ui->pinGraphicsView->installEventFilter(this);
    
    // 初始化边缘选择下拉框
    ui->sideCombo->clear();
    ui->sideCombo->addItem("top");
    ui->sideCombo->addItem("bottom");
    ui->sideCombo->addItem("left");
    ui->sideCombo->addItem("right");
    ui->sideCombo->addItem("custom"); // 添加自定义位置选项
    
    // 设置百分比范围
    ui->percentageSpin->setRange(0, 100);
    ui->percentageSpin->setValue(50);
    
    currentInfo.width = 100;
    currentInfo.height = 100;
    setupPinScene();
    
    // 加载现有引脚信息
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        updatePins();
    }
}

Dialogs::~Dialogs()
{
    pinItems.clear();
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
    ui->pinGraphicsView->viewport()->installEventFilter(this);
}

bool Dialogs::eventFilter(QObject* obj, QEvent* event) {
    if (obj == ui->pinGraphicsView->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF scenePos = ui->pinGraphicsView->mapToScene(mouseEvent->pos());
        qDebug() << "Mouse clicked in pinGraphicsView, scenePos=" << scenePos << ", m_addingPin=" << m_addingPin;
        on_pinSceneClicked(scenePos);
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

void Dialogs::on_pinSceneClicked(const QPointF& pos) {
    qDebug() << "on_pinSceneClicked: pos=" << pos << ", m_addingPin=" << m_addingPin;

    // 检查是否点击了现有引脚
    QGraphicsItem* item = pinScene->itemAt(pos, QTransform());
    if (auto* pin = dynamic_cast<PinItem*>(item)) {
        selectedPin = pin;
        pin->setBrush(QColor("red"));
        for (PinItem* otherPin : pinItems) {
            if (otherPin != pin) {
                otherPin->setBrush(QColor("darkblue"));
            }
        }
        qDebug() << "Selected pin: id=" << pin->getId();
        return;
    }

    // 添加引脚逻辑
    if (m_addingPin) {
        qreal x = pos.x();
        qreal y = pos.y();
        QRectF rect = chipRect->boundingRect();
        qreal width = rect.width();
        qreal height = rect.height();
        QString side;
        qreal percentage;
        qreal margin = 10.0;

        // 检查是否在边缘附近
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
        } else if (x >= 0 && x <= width && y >= 0 && y <= height) {
            // 如果在芯片内部，允许添加自定义位置的引脚
            side = "custom";
            percentage = 0.0;
        }

        if (!side.isEmpty()) {
            percentage = qBound(0.0, percentage, 100.0);
            QString id = QString("pin_%1").arg(currentInfo.pins.size() + 1);
            
            // 对于自定义位置，直接使用点击坐标
            if (side == "custom") {
                addPin(side, percentage, 10, id, x, y);
            } else {
                addPin(side, percentage, 10, id);
            }
            
            m_addingPin = false;
            ui->pinGraphicsView->setCursor(Qt::ArrowCursor);
            chipRect->setPen(QPen(Qt::black));
            qDebug() << "Added pin via click: id=" << id << ", side=" << side << ", percentage=" << percentage << ", x=" << x << ", y=" << y;
            updatePinScene();
        } else {
            qDebug() << "Click outside valid pin area: pos=" << pos;
            QMessageBox::information(this, "提示", "请在芯片区域内点击以添加引脚");
        }
    }
}

void Dialogs::updatePinScene() {
    // 保留chipRect，移除其他所有项
    for (QGraphicsItem* item : pinScene->items()) {
        if (item != chipRect) {
            pinScene->removeItem(item);
        }
    }

    // 确保chipRect存在且尺寸正确
    if (!chipRect) {
        chipRect = new QGraphicsRectItem(0, 0, currentInfo.width, currentInfo.height);
        chipRect->setPen(QPen(Qt::black));
        chipRect->setBrush(Qt::lightGray);
        pinScene->addItem(chipRect);
    } else {
        chipRect->setRect(0, 0, currentInfo.width, currentInfo.height);
    }

    // 从CellItem获取最新的引脚信息
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        auto cellPinItems = cellItem->getPinItems();
        auto connectors = cellItem->getConnectors();
        
        // 清空当前引脚列表
        pinItems.clear();
        
        // 添加所有引脚到场景
        int count = qMin(cellPinItems.size(), connectors.size());
        for (int i = 0; i < count; ++i) {
            if (!cellPinItems[i]) {
                qWarning() << "Null PinItem at index" << i;
                continue;
            }
            
            // 创建新的PinItem用于对话框显示
            PinItem* pin = new PinItem(chipRect, 10);
            pin->updateConnector(connectors[i].id, connectors[i].x, connectors[i].y);
            pin->setPos(connectors[i].x, connectors[i].y);
            pinScene->addItem(pin);
            pinItems.append(pin);
            
            // 设置选中状态
            if (selectedPin && selectedPin->getId() == connectors[i].id) {
                pin->setBrush(QColor("red"));
                selectedPin = pin; // 更新selectedPin指向新创建的引脚
            } else {
                pin->setBrush(QColor("darkblue"));
            }
            
            qDebug() << "Added PinItem to scene: id=" << connectors[i].id << ", x=" << connectors[i].x << ", y=" << connectors[i].y;
        }
        
        // 更新currentInfo中的引脚信息
        currentInfo.pins.clear();
        for (const auto& connector : connectors) {
            currentInfo.pins.append(CellItemNS::Connector(connector.side, connector.percentage, connector.id, connector.x, connector.y));
        }
    } else {
        qWarning() << "Target item is not a CellItem";
    }
}

void Dialogs::saveSetting() {
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
    saveToFile("component.txt");
    emit settingsSaved(currentInfo);
    qDebug() << "Settings saved: pinItems=" << pinItems.size() << ", currentInfo.pins=" << currentInfo.pins.size();
    accept();
}

void Dialogs::on_nameEdit_textEdited(const QString &arg1)
{
    currentInfo.name = arg1;
    qDebug() << "名称已更新为:" << currentInfo.name;
}

void Dialogs::on_saveButton_clicked()
{
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
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        auto connectors = cellItem->getConnectors();
        auto pinItemsList = cellItem->getPinItems();
        if (connectors.size() != pinItemsList.size()) {
            qWarning() << "connectors and pinItems out of sync in on_saveButton_clicked: connectors=" << connectors.size()
            << ", pinItems=" << pinItemsList.size();
        }
        int count = qMin(connectors.size(), pinItemsList.size());
        for (int i = 0; i < count; ++i) {
            if (!pinItemsList[i]) {
                qWarning() << "Null pinItem at index" << i << "in on_saveButton_clicked";
                continue;
            }
            const auto& conn = connectors[i];
            PinItem* pinItem = pinItemsList[i];
            QRectF pinRect = pinItem->boundingRect().translated(pinItem->pos() + targetItem->pos());
            pinInfo += QString("Pin %1 (ID: %2, Side: %3, Percentage: %4):\n"
                               "  Position: (%5, %6)\n")
                           .arg(i + 1).arg(conn.id).arg(conn.side).arg(conn.percentage)
                           .arg(pinItem->pos().x()).arg(pinItem->pos().y());
        }
    }
    QString message = pinInfo.isEmpty() ? "保存成功！" : QString("保存成功！\n引脚信息：\n%1").arg(pinInfo);
    QMessageBox::information(this, "提示", message);

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

void Dialogs::addPin(const QString& side, qreal percentage, qreal size, const QString& id, qreal x, qreal y) {
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        int prevPinCount = cellItem->getPinItems().size();
        
        // 如果是自定义位置且坐标已提供，则直接使用提供的坐标
        if (side == "custom" && x > 0 && y > 0) {
            // 使用提供的坐标
        } else if (x == 0 && y == 0) {
            // 计算边缘位置的坐标
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
            } else if (side == "custom") {
                // 对于自定义位置，如果未提供坐标，则使用中心位置
                x = chipWidth / 2 - size / 2;
                y = chipHeight / 2 - size / 2;
            }
        }
        
        // 通过CellItem添加连接器
        cellItem->addConnector(side, percentage, size, id, x, y);
        
        // 更新对话框中的引脚显示
        updatePinScene();
        
        qDebug() << "Added pin:" << id << " at side=" << side << ", percentage=" << percentage << ", x=" << x << ", y=" << y;
    } else {
        qWarning() << "Target item is not a CellItem";
    }
}

void Dialogs::removePin(const QString& id) {
    if (id.isEmpty()) {
        QMessageBox::warning(this, "错误", "未选择引脚");
        return;
    }

    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        // 使用CellItem的安全删除方法
        if (cellItem->removeConnector(id)) {
            // 删除成功，更新选中的引脚
            if (selectedPin && selectedPin->getId() == id) {
                selectedPin = nullptr;
            }
            
            // 更新对话框中的引脚显示
            updatePinScene();
            qDebug() << "Removed pin:" << id;
        } else {
            qWarning() << "Failed to remove pin id=" << id;
            QMessageBox::warning(this, "错误", "删除引脚失败");
        }
    } else {
        qWarning() << "Target item is not a CellItem";
        QMessageBox::warning(this, "错误", "无法删除引脚：目标项无效");
    }
}

void Dialogs::on_removePinButton_clicked() {
    if (!selectedPin) {
        QMessageBox::warning(this, "错误", "请先选择一个引脚");
        return;
    }
    QString id = selectedPin->getId();
    removePin(id);
}

void Dialogs::on_pinMoved(PinItem* pin) {
    if (!pin) return;

    QString id = pin->getId();
    qreal newX = pin->pos().x();
    qreal newY = pin->pos().y();
    
    // 检查引脚是否靠近边缘，如果是则更新side和percentage
    QRectF rect = chipRect->boundingRect();
    qreal width = rect.width();
    qreal height = rect.height();
    QString newSide;
    qreal newPercentage;
    
    QPointF adjustedPos = pin->restrictToEdge(pin->pos(), width, height, newSide, newPercentage);
    
    // 如果引脚不在边缘附近，则使用自定义位置
    if (newSide.isEmpty()) {
        newSide = "custom";
        newPercentage = 0.0;
    }
    
    if (id.isEmpty()) {
        qWarning() << "on_pinMoved: pin id is empty";
        return;
    }

    // 更新CellItem中的连接器
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        auto connectors = cellItem->getConnectors();
        bool found = false;
        for (int i = 0; i < connectors.size(); ++i) {
            if (connectors[i].id == id) {
                cellItem->updateConnector(i, newSide, newPercentage, newX, newY);
                found = true;
                break;
            }
        }
        if (!found) {
            qWarning() << "Pin id=" << id << " not found in CellItem connectors";
        }
    }

    // 更新对话框中的引脚显示
    updatePinScene();
    
    qDebug() << "Pin moved: id=" << id << ", side=" << newSide << ", percentage=" << newPercentage << ", x=" << newX << ", y=" << newY;
}

void Dialogs::updatePins() {
    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        // 从CellItem获取最新的引脚信息
        auto connectors = cellItem->getConnectors();
        
        // 更新currentInfo中的引脚信息
        currentInfo.pins.clear();
        for (const auto& connector : connectors) {
            currentInfo.pins.append(CellItemNS::Connector(connector.side, connector.percentage, connector.id, connector.x, connector.y));
        }
        
        // 更新对话框中的引脚显示
        updatePinScene();
        
        qDebug() << "updatePins completed: pinItems=" << pinItems.size() << ", currentInfo.pins=" << currentInfo.pins.size();
    }
}

void Dialogs::on_addPinButton_clicked() {
    // 切换到添加引脚模式
    m_addingPin = true;
    ui->pinGraphicsView->setCursor(Qt::CrossCursor);
    chipRect->setPen(QPen(Qt::red, 2));
    QMessageBox::information(this, "添加引脚", "请在芯片区域内点击以添加引脚。\n靠近边缘会自动吸附到边缘，内部区域则创建自由位置引脚。");
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
                qreal percentage = 0.0, x = 0.0, y = 0.0, size = 10;
                for (const QString& param : params) {
                    if (param.startsWith("side=")) side = param.mid(5);
                    else if (param.startsWith("percentage=")) percentage = param.mid(11).toDouble();
                    else if (param.startsWith("x=")) x = param.mid(2).toDouble();
                    else if (param.startsWith("y=")) y = param.mid(2).toDouble();
                    else if (param.startsWith("size=")) size = param.mid(5).toDouble();
                }
                if (!side.isEmpty()) {
                    newPins.append(CellItemNS::Connector(side, percentage, id, x, y));
                }
            }
        }
    }
    file.close();

    if (auto* cellItem = dynamic_cast<CellItem*>(targetItem)) {
        // 清除现有引脚
        auto connectors = cellItem->getConnectors();
        for (int i = connectors.size() - 1; i >= 0; --i) {
            cellItem->removeConnector(i);
        }

        // 添加新引脚
        for (const auto& conn : newPins) {
            addPin(conn.side, conn.percentage, 10, conn.id, conn.x, conn.y);
        }
        
        // 更新对话框中的引脚显示
        updatePinScene();
    }

    QMessageBox::information(this, "提示", "加载成功！");
}
