#include "pineditordialog.h"
#include "ui_pineditordialog.h"
#include "cellitem_new.h"
#include "pinmanager.h"
#include "pinitem.h"
#include "command.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QGraphicsRectItem>
#include <QListWidgetItem>

PinEditorDialog::PinEditorDialog(CellItem* item, QUndoStack* undoStack, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PinEditorDialog)
    , m_targetItem(item)
    , m_pinManager(new PinManager(this))
    , m_undoStack(undoStack)
    , m_pinScene(new QGraphicsScene(this))
    , m_chipRect(nullptr)
    , m_addingPin(false)
    , m_selectedPin(nullptr)
    , m_originalSize(item ? item->size() : QSizeF(100, 100))  // 保存原始尺寸
{
    ui->setupUi(this);

    setupPinScene();
    setupConnections();
    loadChipProperties();
    updatePinScene();
    updatePinList();
}

PinEditorDialog::~PinEditorDialog()
{
    m_pinItems.clear();
    delete ui;
}

void PinEditorDialog::setupConnections()
{
    // 连接UI信号槽
    connect(ui->addPinButton, &QPushButton::clicked, this, &PinEditorDialog::onAddPinClicked);
    connect(ui->removePinButton, &QPushButton::clicked, this, &PinEditorDialog::onRemovePinClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &PinEditorDialog::onSaveToFileClicked);
    connect(ui->loadButton, &QPushButton::clicked, this, &PinEditorDialog::onLoadFromFileClicked);

    connect(ui->pinList, &QListWidget::itemSelectionChanged,
            this, &PinEditorDialog::onPinSelectionChanged);

    connect(ui->sideCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PinEditorDialog::onSideChanged);
    connect(ui->percentageSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PinEditorDialog::onPercentageChanged);

    // 芯片属性连接
    connect(ui->nameEdit, &QLineEdit::textChanged, this, &PinEditorDialog::onNameChanged);
    connect(ui->instanceEdit, &QLineEdit::textChanged, this, &PinEditorDialog::onInstanceChanged);
    connect(ui->colorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PinEditorDialog::onColorChanged);
    connect(ui->widthSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PinEditorDialog::onSizeChanged);
    connect(ui->heightSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PinEditorDialog::onSizeChanged);
    connect(ui->resizableCheckBox, &QCheckBox::toggled, this, &PinEditorDialog::onResizableChanged);

    // 连接对话框按钮
    connect(ui->acceptButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // 为图形视图安装事件过滤器
    ui->pinGraphicsView->viewport()->installEventFilter(this);
}

void PinEditorDialog::loadChipProperties()
{
    if (!m_targetItem) return;

    // 临时断开信号连接，避免在加载属性时触发尺寸变化
    disconnect(ui->widthSpin, QOverload<int>::of(&QSpinBox::valueChanged),
               this, &PinEditorDialog::onSizeChanged);
    disconnect(ui->heightSpin, QOverload<int>::of(&QSpinBox::valueChanged),
               this, &PinEditorDialog::onSizeChanged);

    // 加载芯片属性到UI
    ui->nameEdit->setText(m_targetItem->getMacroName());
    ui->instanceEdit->setText(m_targetItem->getInstanceName());

    // 设置尺寸
    QSizeF size = m_targetItem->size();
    ui->widthSpin->setValue(static_cast<int>(size.width()));
    ui->heightSpin->setValue(static_cast<int>(size.height()));

    // 重新连接信号
    connect(ui->widthSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PinEditorDialog::onSizeChanged);
    connect(ui->heightSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PinEditorDialog::onSizeChanged);

    // 设置默认颜色
    m_currentColor = "浅灰色";
    ui->colorCombo->setCurrentText(m_currentColor);

    // 初始化侧边组合框选项
    if (ui->sideCombo->count() == 0) {
        ui->sideCombo->addItems({"top", "bottom", "left", "right", "custom"});
    }

    // 可以根据需要设置其他属性
    ui->resizableCheckBox->setChecked(true);
}

void PinEditorDialog::setupPinScene()
{
    if (!m_targetItem) return;

    ui->pinGraphicsView->setScene(m_pinScene);
    ui->pinGraphicsView->setRenderHint(QPainter::Antialiasing);

    QSizeF size = m_targetItem->size();

    // 确保芯片有足够的显示大小，最小300x200
    qreal displayWidth = qMax(size.width(), 300.0);
    qreal displayHeight = qMax(size.height(), 200.0);

    m_chipRect = new QGraphicsRectItem(0, 0, displayWidth, displayHeight);

    // 应用默认颜色
    updateChipColor();

    m_pinScene->addItem(m_chipRect);

    // 设置场景矩形，并添加一些边距
    qreal margin = 50.0;
    m_pinScene->setSceneRect(-margin, -margin, displayWidth + 2*margin, displayHeight + 2*margin);

    // 使用固定缩放而不是fitInView，让芯片显示得更大
    ui->pinGraphicsView->setTransform(QTransform::fromScale(1.5, 1.5));
    ui->pinGraphicsView->centerOn(displayWidth/2, displayHeight/2);
}

void PinEditorDialog::updatePinScene()
{
    if (!m_targetItem || !m_chipRect) return;

    // 清除旧的引脚显示（保留chipRect）
    for (QGraphicsItem* item : m_pinScene->items()) {
        if (item != m_chipRect) {
            m_pinScene->removeItem(item);
            delete item;
        }
    }
    m_pinItems.clear();

    // 从CellItem获取引脚信息
    QList<CellItem::Connector> connectors = m_targetItem->getConnectors();
    QSizeF cellSize = m_targetItem->size();

    // 确保芯片有足够的显示大小，最小300x200
    qreal displayWidth = qMax(cellSize.width(), 300.0);
    qreal displayHeight = qMax(cellSize.height(), 200.0);

    // 更新chipRect尺寸和颜色 - 这里是关键！
    m_chipRect->setRect(0, 0, displayWidth, displayHeight);
    updateChipColor();

    // 添加芯片名称文本
    QString displayText = m_targetItem->getInstanceName();
    if (displayText.isEmpty()) {
        displayText = m_targetItem->getMacroName();
    }
    if (!displayText.isEmpty()) {
        QGraphicsTextItem* textItem = m_pinScene->addText(displayText);
        QRectF chipRect = m_chipRect->rect();
        QRectF textRect = textItem->boundingRect();

        // 居中显示文本
        textItem->setPos(chipRect.center().x() - textRect.width() / 2,
                         chipRect.center().y() - textRect.height() / 2);

        // 设置文本颜色以确保可见性
        textItem->setDefaultTextColor(Qt::black);
    }

    // 添加引脚显示 - 使用绝对场景坐标
    for (const auto& conn : connectors) {
        PinItem* pinItem = new PinItem(nullptr, 15);  // 不设置任何parent，使用绝对场景坐标

        // 计算显示位置 - 芯片矩形起始位置 + 引脚在芯片内的相对位置
        qreal displayX, displayY;
        QRectF currentChipRect = m_chipRect->rect();
        qreal chipLeft = currentChipRect.left();  // 芯片左边界（通常是0）
        qreal chipTop = currentChipRect.top();    // 芯片上边界（通常是0）

        if (conn.side == "custom") {
            // 自定义位置：按比例转换到显示坐标
            displayX = chipLeft + (conn.x * currentChipRect.width() / cellSize.width());
            displayY = chipTop + (conn.y * currentChipRect.height() / cellSize.height());
        } else {
            // 边缘位置：直接计算绝对位置
            if (conn.side == "top") {
                displayX = chipLeft + (currentChipRect.width() * conn.percentage / 100.0);
                displayY = chipTop;
            } else if (conn.side == "bottom") {
                displayX = chipLeft + (currentChipRect.width() * conn.percentage / 100.0);
                displayY = chipTop + currentChipRect.height();
            } else if (conn.side == "left") {
                displayX = chipLeft;
                displayY = chipTop + (currentChipRect.height() * conn.percentage / 100.0);
            } else if (conn.side == "right") {
                displayX = chipLeft + currentChipRect.width();
                displayY = chipTop + (currentChipRect.height() * conn.percentage / 100.0);
            } else {
                // 备用方案
                displayX = chipLeft + (conn.x * currentChipRect.width() / cellSize.width());
                displayY = chipTop + (conn.y * currentChipRect.height() / cellSize.height());
            }
        }

        // 设置引脚位置（绝对场景坐标）
        pinItem->setPos(displayX, displayY);
        pinItem->updateConnector(conn.id, displayX, displayY, conn.side, conn.percentage);

        // 添加到场景中
        m_pinScene->addItem(pinItem);
        m_pinItems.append(pinItem);

        qDebug() << "添加引脚到预览:" << conn.id
                 << "存储坐标:(" << conn.x << "," << conn.y << ")"
                 << "芯片位置:(" << chipLeft << "," << chipTop << ")"
                 << "芯片大小:(" << currentChipRect.width() << "x" << currentChipRect.height() << ")"
                 << "绝对显示位置:(" << displayX << "," << displayY << ")"
                 << "边:" << conn.side;
    }

    // 设置场景矩形，并添加一些边距
    qreal margin = 50.0;
    m_pinScene->setSceneRect(-margin, -margin, displayWidth + 2*margin, displayHeight + 2*margin);
}

void PinEditorDialog::updatePinList()
{
    ui->pinList->clear();

    if (!m_targetItem) return;

    QList<CellItem::Connector> connectors = m_targetItem->getConnectors();
    qDebug() << "更新引脚列表，总引脚数:" << connectors.size();

    for (const auto& conn : connectors) {
        QString itemText = QString("%1 (%2, %.1f%%)").arg(conn.id).arg(conn.side).arg(conn.percentage);
        QListWidgetItem* item = new QListWidgetItem(itemText);
        item->setData(Qt::UserRole, conn.id);
        ui->pinList->addItem(item);
        qDebug() << "添加引脚到列表:" << conn.id << itemText;
    }

    qDebug() << "引脚列表项数量:" << ui->pinList->count();
}

void PinEditorDialog::updatePinProperties()
{
    if (!m_selectedPin || !m_targetItem) {
        ui->pinIdEdit->clear();
        ui->sideCombo->setCurrentIndex(-1);
        ui->percentageSpin->setValue(0);
        return;
    }

    QString selectedPinId = m_selectedPin->getId();
    ui->pinIdEdit->setText(selectedPinId);

    // 从 CellItem 的 Connector 数据获取引脚信息
    QList<CellItem::Connector> connectors = m_targetItem->getConnectors();
    for (const auto& conn : connectors) {
        if (conn.id == selectedPinId) {
            // 设置边
            int sideIndex = ui->sideCombo->findText(conn.side);
            if (sideIndex >= 0) {
                ui->sideCombo->setCurrentIndex(sideIndex);
            }

            // 设置百分比
            ui->percentageSpin->setValue(static_cast<int>(conn.percentage));

            qDebug() << "更新引脚属性:" << selectedPinId
                     << "边:" << conn.side
                     << "百分比:" << conn.percentage;
            return;
        }
    }

    qDebug() << "警告：未找到引脚" << selectedPinId << "的连接器数据";
}

bool PinEditorDialog::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == ui->pinGraphicsView->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && m_addingPin) {
            QPointF scenePos = ui->pinGraphicsView->mapToScene(mouseEvent->pos());
            onPinSceneClicked(scenePos);
            return true;  // 阻止事件继续传播，避免重复处理
        }
    }
    return QDialog::eventFilter(obj, event);
}

void PinEditorDialog::onAddPinClicked()
{
    enterAddPinMode();
}

void PinEditorDialog::onRemovePinClicked()
{
    QListWidgetItem* currentItem = ui->pinList->currentItem();
    if (!currentItem) {
        QMessageBox::information(this, "提示", "请先选择要删除的引脚");
        return;
    }

    QString pinId = currentItem->data(Qt::UserRole).toString();
    qDebug() << "尝试删除引脚:" << pinId;

    // 检查引脚是否存在
    if (!pinIdExists(pinId)) {
        QMessageBox::warning(this, "错误", "引脚不存在：" + pinId);
        return;
    }

    // 使用命令模式删除引脚
    if (m_undoStack) {
        qDebug() << "使用撤销栈删除引脚:" << pinId;
        RemovePinCommand* command = new RemovePinCommand(m_targetItem, pinId);
        m_undoStack->push(command);
    } else {
        qDebug() << "直接删除引脚（无撤销栈）:" << pinId;
        // 如果没有撤销栈，直接删除
        if (!m_targetItem->removeConnector(pinId)) {
            QMessageBox::warning(this, "错误", "删除引脚失败：" + pinId);
            return;
        }
    }

    // 清空当前选择
    m_selectedPin = nullptr;
    ui->pinList->setCurrentItem(nullptr);

    updatePinScene();
    updatePinList();
    updatePinProperties();  // 清空属性显示
    QMessageBox::information(this, "成功", "引脚已删除：" + pinId);
}

void PinEditorDialog::onSaveToFileClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "保存组件", "", "Text Files (*.txt)");
    if (filePath.isEmpty()) return;

    // 这里可以使用FileManager来保存
    QMessageBox::information(this, "提示", "保存功能待实现");
}

void PinEditorDialog::onLoadFromFileClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "加载组件", "", "Text Files (*.txt)");
    if (filePath.isEmpty()) return;

    // 这里可以使用FileManager来加载
    QMessageBox::information(this, "提示", "加载功能待实现");
}

void PinEditorDialog::onPinSceneClicked(const QPointF& pos)
{
    if (!m_addingPin || !m_targetItem || !m_chipRect) return;

    QRectF chipRect = m_chipRect->rect();
    qreal x = pos.x();
    qreal y = pos.y();
    QString side;
    qreal percentage = 0.0;
    qreal edgeThreshold = 20.0;  // 增加边缘检测阈值

    qDebug() << "点击位置(场景坐标):" << x << "," << y;
    qDebug() << "芯片矩形大小:" << chipRect.width() << "x" << chipRect.height();

    // 确保点击在芯片区域内
    if (x < -edgeThreshold || x > chipRect.width() + edgeThreshold ||
        y < -edgeThreshold || y > chipRect.height() + edgeThreshold) {
        QMessageBox::information(this, "提示", "请在芯片区域内点击以添加引脚");
        return;
    }

    // 判断点击位置并计算边缘参数（优先级：top > bottom > left > right）
    if (y >= -edgeThreshold && y <= edgeThreshold && x >= 0 && x <= chipRect.width()) {
        // 顶部边缘
        side = "top";
        percentage = (x / chipRect.width()) * 100.0;
        y = 0;  // 固定到边缘
    } else if (y >= chipRect.height() - edgeThreshold && y <= chipRect.height() + edgeThreshold &&
               x >= 0 && x <= chipRect.width()) {
        // 底部边缘
        side = "bottom";
        percentage = (x / chipRect.width()) * 100.0;
        y = chipRect.height();  // 固定到边缘
    } else if (x >= -edgeThreshold && x <= edgeThreshold && y >= 0 && y <= chipRect.height()) {
        // 左边缘
        side = "left";
        percentage = (y / chipRect.height()) * 100.0;
        x = 0;  // 固定到边缘
    } else if (x >= chipRect.width() - edgeThreshold && x <= chipRect.width() + edgeThreshold &&
               y >= 0 && y <= chipRect.height()) {
        // 右边缘
        side = "right";
        percentage = (y / chipRect.height()) * 100.0;
        x = chipRect.width();  // 固定到边缘
    } else if (x >= 0 && x <= chipRect.width() && y >= 0 && y <= chipRect.height()) {
        // 内部区域
        side = "custom";
        percentage = 0.0;
        qDebug() << "检测到内部区域，自由位置:" << x << "," << y;
    }

    if (!side.isEmpty()) {
        percentage = qBound(0.0, percentage, 100.0);

        // 生成唯一的引脚ID，避免重复
        QString id;
        int counter = 1;
        do {
            id = QString("pin_%1").arg(counter);
            counter++;
        } while (pinIdExists(id));

        // 计算实际坐标（相对于原始芯片大小）
        QSizeF cellSize = m_targetItem->size();
        qreal actualX, actualY;

        if (side == "custom") {
            // 自定义位置：使用简单直接的转换
            // 获取当前芯片矩形的实际显示尺寸
            QRectF currentChipRect = m_chipRect->rect();

            actualX = x * cellSize.width() / currentChipRect.width();
            actualY = y * cellSize.height() / currentChipRect.height();

            qDebug() << "自定义引脚位置计算:";
            qDebug() << "  点击位置:" << x << "," << y;
            qDebug() << "  芯片实际大小:" << cellSize.width() << "x" << cellSize.height();
            qDebug() << "  当前显示大小:" << currentChipRect.width() << "x" << currentChipRect.height();
            qDebug() << "  转换后实际坐标:" << actualX << "," << actualY;
        } else {
            // 边缘位置：让addConnector内部计算
            actualX = 0;
            actualY = 0;
        }

        // 添加引脚 - 使用命令模式
        if (m_undoStack) {
            AddPinCommand* command = new AddPinCommand(m_targetItem, side, percentage, 10.0, id, actualX, actualY);
            m_undoStack->push(command);
        } else {
            // 如果没有撤销栈，直接添加
            m_targetItem->addConnector(side, percentage, 1.0, id, actualX, actualY);
        }

        updatePinScene();
        updatePinList();
        exitAddPinMode();

        QMessageBox::information(this, "成功", QString("引脚 %1 已添加到 %2 边").arg(id).arg(side));
    } else {
        QMessageBox::information(this, "提示", "请在芯片区域内点击以添加引脚");
    }
}

void PinEditorDialog::onPinSelectionChanged()
{
    QListWidgetItem* currentItem = ui->pinList->currentItem();
    m_selectedPin = nullptr; // 先清空选择

    if (currentItem) {
        QString pinId = currentItem->data(Qt::UserRole).toString();
        qDebug() << "尝试选择引脚:" << pinId;
        qDebug() << "当前场景中的引脚数量:" << m_pinItems.size();

        // 在场景中查找对应的引脚
        for (PinItem* pinItem : m_pinItems) {
            qDebug() << "检查引脚:" << pinItem->getId();
            if (pinItem->getId() == pinId) {
                m_selectedPin = pinItem;
                qDebug() << "找到匹配的引脚:" << pinId;

                // 高亮显示选中的引脚
                pinItem->setPen(QPen(Qt::red, 3));
                pinItem->setBrush(Qt::yellow);
                break;
            }
        }

        // 清除其他引脚的高亮
        for (PinItem* pinItem : m_pinItems) {
            if (pinItem != m_selectedPin) {
                pinItem->setPen(QPen(Qt::darkBlue, 1));
                pinItem->setBrush(Qt::blue);
            }
        }

        if (!m_selectedPin) {
            qDebug() << "警告：未找到ID为" << pinId << "的引脚";
        }
    } else {
        qDebug() << "没有选中任何列表项";
        // 清除所有引脚的高亮
        for (PinItem* pinItem : m_pinItems) {
            pinItem->setPen(QPen(Qt::darkBlue, 1));
            pinItem->setBrush(Qt::blue);
        }
    }

    updatePinProperties();
}

void PinEditorDialog::onSideChanged(int index)
{
    Q_UNUSED(index)
    // 这里可以实现引脚位置的实时更新
}

void PinEditorDialog::onPercentageChanged(int value)
{
    Q_UNUSED(value)
    // 这里可以实现引脚位置的实时更新
}

void PinEditorDialog::enterAddPinMode()
{
    m_addingPin = true;
    ui->pinGraphicsView->setCursor(Qt::CrossCursor);
    if (m_chipRect) {
        m_chipRect->setPen(QPen(Qt::red, 2));
    }
    QMessageBox::information(this, "添加引脚", "请在芯片区域内点击以添加引脚。\n靠近边缘会自动吸附到边缘，内部区域则创建自由位置引脚。");
}

void PinEditorDialog::exitAddPinMode()
{
    m_addingPin = false;
    ui->pinGraphicsView->setCursor(Qt::ArrowCursor);
    if (m_chipRect) {
        m_chipRect->setPen(QPen(Qt::black));
    }
}

// 芯片属性变化处理函数
void PinEditorDialog::onNameChanged(const QString& text)
{
    if (m_targetItem) {
        m_targetItem->setMacroName(text);
        updatePinScene();  // 重新绘制场景以显示新名称
    }
}

void PinEditorDialog::onInstanceChanged(const QString& text)
{
    if (m_targetItem) {
        m_targetItem->setInstanceName(text);
        updatePinScene();  // 重新绘制场景以显示新实例名
    }
}

void PinEditorDialog::onColorChanged(int index)
{
    if (index >= 0 && index < ui->colorCombo->count()) {
        m_currentColor = ui->colorCombo->itemText(index);
        updateChipColor();
        updatePinScene();
    }
}

void PinEditorDialog::onSizeChanged(int value)
{
    Q_UNUSED(value)
    if (m_targetItem) {
        QSizeF newSize(ui->widthSpin->value(), ui->heightSpin->value());
        m_targetItem->setSize(newSize);
        updatePinScene();
        // 重新设置视图变换以保持合适的显示比例
        qreal displayWidth = qMax(newSize.width(), 300.0);
        qreal displayHeight = qMax(newSize.height(), 200.0);
        ui->pinGraphicsView->setTransform(QTransform::fromScale(1.5, 1.5));
        ui->pinGraphicsView->centerOn(displayWidth/2, displayHeight/2);
    }
}

void PinEditorDialog::onResizableChanged(bool checked)
{
    // 这里可以设置芯片是否可调整大小的属性
    Q_UNUSED(checked)
    // m_targetItem->setResizable(checked);
}

void PinEditorDialog::updateChipColor()
{
    if (m_chipRect) {
        QColor borderColor = getColorFromString(m_currentColor);
        QColor fillColor = borderColor.lighter(160);  // 浅色填充，避免与重叠颜色冲突

        m_chipRect->setPen(QPen(borderColor, 2));
        m_chipRect->setBrush(QBrush(fillColor));
    }
}

QColor PinEditorDialog::getColorFromString(const QString& colorName)
{
    if (colorName == "蓝色") return QColor(0, 100, 200);
    else if (colorName == "绿色") return QColor(50, 150, 50);
    else if (colorName == "黄色") return QColor(200, 180, 0);
    else if (colorName == "红色") return QColor(200, 50, 50);
    else if (colorName == "青色") return QColor(0, 150, 150);
    else if (colorName == "洋红色") return QColor(150, 0, 150);
    else return QColor(120, 120, 120);  // 浅灰色
}

QString PinEditorDialog::getCurrentColorName()
{
    return m_currentColor;
}

bool PinEditorDialog::pinIdExists(const QString& id)
{
    if (!m_targetItem) return false;

    QList<CellItem::Connector> connectors = m_targetItem->getConnectors();
    for (const auto& conn : connectors) {
        if (conn.id == id) {
            return true;
        }
    }
    return false;
}

void PinEditorDialog::reject()
{
    // 恢复原始尺寸
    if (m_targetItem) {
        m_targetItem->setSize(m_originalSize);
    }

    // 调用基类的reject方法
    QDialog::reject();
}
