#include "settingdialog.h"

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("设置");

    // 创建控件
    nameEdit = new QLineEdit(this);
    colorCombo = new QComboBox(this);
    colorCombo->addItems({"红色", "蓝色", "绿色", "黄色"});
    enableResize = new QCheckBox("启用调整大小", this);

    // 创建按钮
    QPushButton* saveButton = new QPushButton("保存", this);
    QPushButton* cancelButton = new QPushButton("取消", this);

    // 创建布局
    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow("名称:", nameEdit);
    formLayout->addRow("颜色:", colorCombo);
    formLayout->addRow("", enableResize);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    // 连接信号和槽
    connect(saveButton, &QPushButton::clicked, this, &SettingDialog::saveSettings);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void SettingDialog::saveSettings() {
    // 这里添加保存设置的逻辑
    // 例如：将设置保存到一个配置文件或数据库
    accept(); // 关闭对话框
}
