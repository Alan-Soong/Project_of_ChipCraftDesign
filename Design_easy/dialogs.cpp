#include "dialogs.h"
#include "ui_dialogs.h"

Dialogs::Dialogs(QGraphicsItem* item, QWidget *parent)
    : QDialog(parent), targetItem(item)
    , ui(new Ui::Dialogs)
{
    ui->setupUi(this);
    setWindowTitle("设置");

    // // 创建控件
    // nameEdit = new QLineEdit(this);
    // colorCombo = new QComboBox(this);
    // colorCombo->addItems({"红色", "蓝色", "绿色", "黄色"});
    // enableResize = new QCheckBox("启用调整大小", this);

    // // 创建按钮
    // QPushButton* saveButton = new QPushButton("保存", this);
    // QPushButton* cancelButton = new QPushButton("取消", this);

    // // 创建布局
    // QFormLayout* formLayout = new QFormLayout;
    // formLayout->addRow("名称:", nameEdit);
    // formLayout->addRow("颜色:", colorCombo);
    // formLayout->addRow("", enableResize);

    // QHBoxLayout* buttonLayout = new QHBoxLayout;
    // buttonLayout->addWidget(saveButton);
    // buttonLayout->addWidget(cancelButton);

    // QVBoxLayout* mainLayout = new QVBoxLayout(this);
    // mainLayout->addLayout(formLayout);
    // mainLayout->addLayout(buttonLayout);

    // // 连接信号和槽
    // connect(saveButton, &QPushButton::clicked, this, &Dialogs::saveSetting);
    // connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);


    connect(ui->nameEdit, &QLineEdit::textChanged, this, &Dialogs::on_nameEdit_textEdited);
    // 连接颜色选择信号
    connect(ui->colorCombo, QOverload<int>::of(&QComboBox::activated), this, &Dialogs::on_colorCombo_activated);
}

Dialogs::~Dialogs()
{
    delete ui;
}

void Dialogs::saveSetting() {
    // 这里添加保存设置的逻辑
    // 例如：将设置保存到一个配置文件或数据库
    accept(); // 关闭对话框
}

void Dialogs::on_nameEdit_textEdited(const QString &arg1)
{
    currentInfo.name = arg1;
    qDebug() << "名称已更新为:" << currentInfo.name;
}


void Dialogs::on_saveButton_clicked()
{
    // 弹出提示框
    if (targetItem) {
        QRectF rect = targetItem->boundingRect();
        currentInfo.width = static_cast<int>(rect.width());
        currentInfo.height = static_cast<int>(rect.height());
    } else {
        currentInfo.width = this->width();
        currentInfo.height = this->height();
    }
    qDebug() << "size:" << currentInfo.width << currentInfo.height;

    /// 获取选中的颜色文本
    if (ui->colorCombo && ui->colorCombo->currentIndex() >= 0) {
        currentInfo.color = ui->colorCombo->currentText(); // 直接获取选中的文本
        qDebug() << "Selected color:" << currentInfo.color;
    } else {
        qDebug() << "No color selected or color combo box is invalid!";
        currentInfo.color = "none"; // 设置默认值
    }
    QMessageBox::information(this, "提示", "保存成功！");
    accept(); // 关闭对话框
}


void Dialogs::on_colorCombo_activated(int index)
{
    if (!ui->colorCombo) {
        qWarning() << "Color combo box is not valid!";
        return;
    }

    if (index >= 0 && index < ui->colorCombo->count()) {
        currentInfo.color = ui->colorCombo->currentText(); // 直接获取选中的文本
        qDebug() << "Selected color:" << currentInfo.color;
    } else {
        qDebug() << "Invalid index:" << index;
    }
    // 如果 targetItem 是有效的，并且是 QGraphicsRectItem 类型
    if (targetItem) {
        QGraphicsRectItem* RectItem = dynamic_cast<QGraphicsRectItem*>(targetItem);
        if (RectItem) {
            RectItem->setBrush(QColor(currentInfo.color));
            qDebug() << "targetItem is colored!";
        } else {
            qDebug() << "targetItem is not a QGraphicsRectItem!";
        }
    }
}

