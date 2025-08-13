/********************************************************************************
** Form generated from reading UI file 'pineditordialog.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PINEDITORDIALOG_H
#define UI_PINEDITORDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PinEditorDialog
{
public:
    QHBoxLayout *mainLayout;
    QSplitter *splitter;
    QWidget *leftWidget;
    QVBoxLayout *leftLayout;
    QLabel *layoutLabel;
    QGraphicsView *pinGraphicsView;
    QHBoxLayout *operationButtonLayout;
    QPushButton *addPinButton;
    QPushButton *removePinButton;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QWidget *rightWidget;
    QVBoxLayout *rightLayout;
    QGroupBox *chipPropertiesGroup;
    QFormLayout *chipFormLayout;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QLabel *instanceLabel;
    QLineEdit *instanceEdit;
    QLabel *colorLabel;
    QComboBox *colorCombo;
    QLabel *sizeLabel;
    QHBoxLayout *sizeLayout;
    QSpinBox *widthSpin;
    QLabel *xLabel;
    QSpinBox *heightSpin;
    QSpacerItem *sizeSpacer;
    QCheckBox *resizableCheckBox;
    QGroupBox *pinListGroup;
    QVBoxLayout *pinListLayout;
    QListWidget *pinList;
    QGroupBox *pinPropertiesGroup;
    QFormLayout *pinFormLayout;
    QLabel *pinIdLabel;
    QLineEdit *pinIdEdit;
    QLabel *sideLabel;
    QComboBox *sideCombo;
    QLabel *percentageLabel;
    QSpinBox *percentageSpin;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *dialogButtonLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *acceptButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *PinEditorDialog)
    {
        if (PinEditorDialog->objectName().isEmpty())
            PinEditorDialog->setObjectName("PinEditorDialog");
        PinEditorDialog->resize(900, 700);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PinEditorDialog->sizePolicy().hasHeightForWidth());
        PinEditorDialog->setSizePolicy(sizePolicy);
        PinEditorDialog->setStyleSheet(QString::fromUtf8("/* \345\257\271\350\257\235\346\241\206\346\225\264\344\275\223\346\240\267\345\274\217 */\n"
"QDialog {\n"
"    background-color: #faf9f8;\n"
"    font-family: \"Segoe UI\", \"Microsoft YaHei\";\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"/* \346\240\207\347\255\276\346\240\267\345\274\217 */\n"
"QLabel {\n"
"    color: #323130;\n"
"    font-weight: 500;\n"
"    padding: 8px 4px;\n"
"}\n"
"\n"
"/* \350\276\223\345\205\245\346\241\206\346\240\267\345\274\217 */\n"
"QLineEdit, QSpinBox, QDoubleSpinBox {\n"
"    background-color: #ffffff;\n"
"    border: 2px solid #e1dfdd;\n"
"    border-radius: 6px;\n"
"    padding: 8px 12px;\n"
"    font-size: 14px;\n"
"    color: #323130;\n"
"}\n"
"\n"
"QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {\n"
"    border-color: #0078d4;\n"
"    outline: none;\n"
"}\n"
"\n"
"QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover {\n"
"    border-color: #c8c6c4;\n"
"}\n"
"\n"
"/* \347\273\204\345\220\210\346\241\206\346\240\267\345\274\217 */\n"
"QComboBox {\n"
"    background-col"
                        "or: #ffffff;\n"
"    border: 2px solid #e1dfdd;\n"
"    border-radius: 6px;\n"
"    padding: 8px 12px;\n"
"    font-size: 14px;\n"
"    color: #323130;\n"
"    min-width: 120px;\n"
"}\n"
"\n"
"QComboBox:focus {\n"
"    border-color: #0078d4;\n"
"}\n"
"\n"
"QComboBox:hover {\n"
"    border-color: #c8c6c4;\n"
"}\n"
"\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    width: 30px;\n"
"}\n"
"\n"
"QComboBox::down-arrow {\n"
"    image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTYiIGhlaWdodD0iMTYiIHZpZXdCb3g9IjAgMCAxNiAxNiIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTQgNkw4IDEwTDEyIDYiIHN0cm9rZT0iIzMyMzEzMCIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiLz4KPC9zdmc+);\n"
"}\n"
"\n"
"/* \345\244\215\351\200\211\346\241\206\346\240\267\345\274\217 */\n"
"QCheckBox {\n"
"    color: #323130;\n"
"    font-size: 14px;\n"
"    spacing: 8px;\n"
"}\n"
"\n"
"QCheckBox::indicator {\n"
"    width: 18px;\n"
"    height: 18px;\n"
"    border: 2px s"
                        "olid #e1dfdd;\n"
"    border-radius: 3px;\n"
"    background-color: #ffffff;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked {\n"
"    background-color: #0078d4;\n"
"    border-color: #0078d4;\n"
"    image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTYiIGhlaWdodD0iMTYiIHZpZXdCb3g9IjAgMCAxNiAxNiIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTEzIDRMNiAxMUwzIDgiIHN0cm9rZT0iI2ZmZmZmZiIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiLz4KPC9zdmc+);\n"
"}\n"
"\n"
"/* \346\214\211\351\222\256\346\240\267\345\274\217 */\n"
"QPushButton {\n"
"    background-color: #ffffff;\n"
"    border: 2px solid #e1dfdd;\n"
"    border-radius: 6px;\n"
"    padding: 10px 20px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    color: #323130;\n"
"    min-height: 20px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #f3f2f1;\n"
"    border-color: #c8c6c4;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #edebe9;\n"
"    border-color"
                        ": #a19f9d;\n"
"}\n"
"\n"
"QPushButton:focus {\n"
"    border-color: #0078d4;\n"
"    outline: none;\n"
"}\n"
"\n"
"/* \344\270\273\350\246\201\346\214\211\351\222\256\346\240\267\345\274\217 */\n"
"QPushButton#saveButton, QPushButton#acceptButton {\n"
"    background-color: #0078d4;\n"
"    color: #ffffff;\n"
"    border-color: #0078d4;\n"
"}\n"
"\n"
"QPushButton#saveButton:hover, QPushButton#acceptButton:hover {\n"
"    background-color: #106ebe;\n"
"    border-color: #106ebe;\n"
"}\n"
"\n"
"QPushButton#addPinButton {\n"
"    background-color: #107c10;\n"
"    color: #ffffff;\n"
"    border-color: #107c10;\n"
"}\n"
"\n"
"QPushButton#addPinButton:hover {\n"
"    background-color: #0e6e0e;\n"
"    border-color: #0e6e0e;\n"
"}\n"
"\n"
"QPushButton#removePinButton {\n"
"    background-color: #d13438;\n"
"    color: #ffffff;\n"
"    border-color: #d13438;\n"
"}\n"
"\n"
"QPushButton#removePinButton:hover {\n"
"    background-color: #b4282c;\n"
"    border-color: #b4282c;\n"
"}\n"
"\n"
"/* \345\210\227\350\241\250\346"
                        "\216\247\344\273\266\346\240\267\345\274\217 */\n"
"QListWidget {\n"
"    background-color: #ffffff;\n"
"    border: 2px solid #e1dfdd;\n"
"    border-radius: 6px;\n"
"    padding: 4px;\n"
"    selection-background-color: #f3f2f1;\n"
"}\n"
"\n"
"QListWidget::item {\n"
"    padding: 8px;\n"
"    border-radius: 4px;\n"
"    margin: 2px;\n"
"}\n"
"\n"
"QListWidget::item:selected {\n"
"    background-color: #e8f4fd;\n"
"    color: #323130;\n"
"}\n"
"\n"
"QListWidget::item:hover {\n"
"    background-color: #f3f2f1;\n"
"}\n"
"\n"
"/* \345\233\276\345\275\242\350\247\206\345\233\276\346\240\267\345\274\217 */\n"
"QGraphicsView {\n"
"    background-color: #ffffff;\n"
"    border: 2px solid #e1dfdd;\n"
"    border-radius: 6px;\n"
"}\n"
"\n"
"QGraphicsView:focus {\n"
"    border-color: #0078d4;\n"
"}\n"
"\n"
"/* \345\210\206\345\211\262\345\231\250\346\240\267\345\274\217 */\n"
"QSplitter::handle {\n"
"    background-color: #e1dfdd;\n"
"    width: 4px;\n"
"    border-radius: 2px;\n"
"}\n"
"\n"
"QSplitter::handle:hover {"
                        "\n"
"    background-color: #c8c6c4;\n"
"}\n"
"\n"
"/* \347\273\204\346\241\206\346\240\267\345\274\217 */\n"
"QGroupBox {\n"
"    font-weight: 600;\n"
"    font-size: 16px;\n"
"    color: #323130;\n"
"    border: 2px solid #e1dfdd;\n"
"    border-radius: 8px;\n"
"    margin-top: 12px;\n"
"    padding-top: 8px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 12px;\n"
"    padding: 0 8px 0 8px;\n"
"    background-color: #faf9f8;\n"
"}"));
        mainLayout = new QHBoxLayout(PinEditorDialog);
        mainLayout->setSpacing(20);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(24, 24, 24, 24);
        splitter = new QSplitter(PinEditorDialog);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Horizontal);
        leftWidget = new QWidget(splitter);
        leftWidget->setObjectName("leftWidget");
        leftLayout = new QVBoxLayout(leftWidget);
        leftLayout->setSpacing(16);
        leftLayout->setObjectName("leftLayout");
        layoutLabel = new QLabel(leftWidget);
        layoutLabel->setObjectName("layoutLabel");
        layoutLabel->setStyleSheet(QString::fromUtf8("font-weight: 600; font-size: 16px;"));

        leftLayout->addWidget(layoutLabel);

        pinGraphicsView = new QGraphicsView(leftWidget);
        pinGraphicsView->setObjectName("pinGraphicsView");
        pinGraphicsView->setMinimumSize(QSize(500, 400));

        leftLayout->addWidget(pinGraphicsView);

        operationButtonLayout = new QHBoxLayout();
        operationButtonLayout->setSpacing(12);
        operationButtonLayout->setObjectName("operationButtonLayout");
        addPinButton = new QPushButton(leftWidget);
        addPinButton->setObjectName("addPinButton");

        operationButtonLayout->addWidget(addPinButton);

        removePinButton = new QPushButton(leftWidget);
        removePinButton->setObjectName("removePinButton");

        operationButtonLayout->addWidget(removePinButton);

        saveButton = new QPushButton(leftWidget);
        saveButton->setObjectName("saveButton");

        operationButtonLayout->addWidget(saveButton);

        loadButton = new QPushButton(leftWidget);
        loadButton->setObjectName("loadButton");

        operationButtonLayout->addWidget(loadButton);


        leftLayout->addLayout(operationButtonLayout);

        splitter->addWidget(leftWidget);
        rightWidget = new QWidget(splitter);
        rightWidget->setObjectName("rightWidget");
        rightLayout = new QVBoxLayout(rightWidget);
        rightLayout->setSpacing(16);
        rightLayout->setObjectName("rightLayout");
        chipPropertiesGroup = new QGroupBox(rightWidget);
        chipPropertiesGroup->setObjectName("chipPropertiesGroup");
        chipFormLayout = new QFormLayout(chipPropertiesGroup);
        chipFormLayout->setObjectName("chipFormLayout");
        chipFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        chipFormLayout->setHorizontalSpacing(12);
        chipFormLayout->setVerticalSpacing(12);
        nameLabel = new QLabel(chipPropertiesGroup);
        nameLabel->setObjectName("nameLabel");

        chipFormLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, nameLabel);

        nameEdit = new QLineEdit(chipPropertiesGroup);
        nameEdit->setObjectName("nameEdit");

        chipFormLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, nameEdit);

        instanceLabel = new QLabel(chipPropertiesGroup);
        instanceLabel->setObjectName("instanceLabel");

        chipFormLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, instanceLabel);

        instanceEdit = new QLineEdit(chipPropertiesGroup);
        instanceEdit->setObjectName("instanceEdit");

        chipFormLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, instanceEdit);

        colorLabel = new QLabel(chipPropertiesGroup);
        colorLabel->setObjectName("colorLabel");

        chipFormLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, colorLabel);

        colorCombo = new QComboBox(chipPropertiesGroup);
        colorCombo->addItem(QString());
        colorCombo->addItem(QString());
        colorCombo->addItem(QString());
        colorCombo->addItem(QString());
        colorCombo->addItem(QString());
        colorCombo->addItem(QString());
        colorCombo->addItem(QString());
        colorCombo->setObjectName("colorCombo");

        chipFormLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, colorCombo);

        sizeLabel = new QLabel(chipPropertiesGroup);
        sizeLabel->setObjectName("sizeLabel");

        chipFormLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, sizeLabel);

        sizeLayout = new QHBoxLayout();
        sizeLayout->setSpacing(8);
        sizeLayout->setObjectName("sizeLayout");
        widthSpin = new QSpinBox(chipPropertiesGroup);
        widthSpin->setObjectName("widthSpin");
        widthSpin->setMinimum(50);
        widthSpin->setMaximum(1000);
        widthSpin->setValue(150);

        sizeLayout->addWidget(widthSpin);

        xLabel = new QLabel(chipPropertiesGroup);
        xLabel->setObjectName("xLabel");
        xLabel->setAlignment(Qt::AlignCenter);

        sizeLayout->addWidget(xLabel);

        heightSpin = new QSpinBox(chipPropertiesGroup);
        heightSpin->setObjectName("heightSpin");
        heightSpin->setMinimum(50);
        heightSpin->setMaximum(1000);
        heightSpin->setValue(100);

        sizeLayout->addWidget(heightSpin);

        sizeSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        sizeLayout->addItem(sizeSpacer);


        chipFormLayout->setLayout(3, QFormLayout::ItemRole::FieldRole, sizeLayout);

        resizableCheckBox = new QCheckBox(chipPropertiesGroup);
        resizableCheckBox->setObjectName("resizableCheckBox");
        resizableCheckBox->setChecked(true);

        chipFormLayout->setWidget(4, QFormLayout::ItemRole::SpanningRole, resizableCheckBox);


        rightLayout->addWidget(chipPropertiesGroup);

        pinListGroup = new QGroupBox(rightWidget);
        pinListGroup->setObjectName("pinListGroup");
        pinListLayout = new QVBoxLayout(pinListGroup);
        pinListLayout->setSpacing(12);
        pinListLayout->setObjectName("pinListLayout");
        pinList = new QListWidget(pinListGroup);
        pinList->setObjectName("pinList");
        pinList->setMinimumSize(QSize(0, 150));

        pinListLayout->addWidget(pinList);


        rightLayout->addWidget(pinListGroup);

        pinPropertiesGroup = new QGroupBox(rightWidget);
        pinPropertiesGroup->setObjectName("pinPropertiesGroup");
        pinFormLayout = new QFormLayout(pinPropertiesGroup);
        pinFormLayout->setObjectName("pinFormLayout");
        pinFormLayout->setHorizontalSpacing(12);
        pinFormLayout->setVerticalSpacing(12);
        pinIdLabel = new QLabel(pinPropertiesGroup);
        pinIdLabel->setObjectName("pinIdLabel");

        pinFormLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, pinIdLabel);

        pinIdEdit = new QLineEdit(pinPropertiesGroup);
        pinIdEdit->setObjectName("pinIdEdit");
        pinIdEdit->setReadOnly(true);

        pinFormLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, pinIdEdit);

        sideLabel = new QLabel(pinPropertiesGroup);
        sideLabel->setObjectName("sideLabel");

        pinFormLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, sideLabel);

        sideCombo = new QComboBox(pinPropertiesGroup);
        sideCombo->addItem(QString());
        sideCombo->addItem(QString());
        sideCombo->addItem(QString());
        sideCombo->addItem(QString());
        sideCombo->addItem(QString());
        sideCombo->setObjectName("sideCombo");

        pinFormLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, sideCombo);

        percentageLabel = new QLabel(pinPropertiesGroup);
        percentageLabel->setObjectName("percentageLabel");

        pinFormLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, percentageLabel);

        percentageSpin = new QSpinBox(pinPropertiesGroup);
        percentageSpin->setObjectName("percentageSpin");
        percentageSpin->setMaximum(100);

        pinFormLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, percentageSpin);


        rightLayout->addWidget(pinPropertiesGroup);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        rightLayout->addItem(verticalSpacer);

        dialogButtonLayout = new QHBoxLayout();
        dialogButtonLayout->setSpacing(12);
        dialogButtonLayout->setObjectName("dialogButtonLayout");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        dialogButtonLayout->addItem(horizontalSpacer);

        acceptButton = new QPushButton(rightWidget);
        acceptButton->setObjectName("acceptButton");

        dialogButtonLayout->addWidget(acceptButton);

        cancelButton = new QPushButton(rightWidget);
        cancelButton->setObjectName("cancelButton");

        dialogButtonLayout->addWidget(cancelButton);


        rightLayout->addLayout(dialogButtonLayout);

        splitter->addWidget(rightWidget);

        mainLayout->addWidget(splitter);


        retranslateUi(PinEditorDialog);

        QMetaObject::connectSlotsByName(PinEditorDialog);
    } // setupUi

    void retranslateUi(QDialog *PinEditorDialog)
    {
        PinEditorDialog->setWindowTitle(QCoreApplication::translate("PinEditorDialog", "\345\274\225\350\204\232\347\274\226\350\276\221\345\231\250", nullptr));
        layoutLabel->setText(QCoreApplication::translate("PinEditorDialog", "\345\274\225\350\204\232\345\270\203\345\261\200\351\242\204\350\247\210:", nullptr));
        addPinButton->setText(QCoreApplication::translate("PinEditorDialog", "+ \346\267\273\345\212\240\345\274\225\350\204\232", nullptr));
        removePinButton->setText(QCoreApplication::translate("PinEditorDialog", "- \345\210\240\351\231\244\345\274\225\350\204\232", nullptr));
        saveButton->setText(QCoreApplication::translate("PinEditorDialog", "\344\277\235\345\255\230\345\210\260\346\226\207\344\273\266", nullptr));
        loadButton->setText(QCoreApplication::translate("PinEditorDialog", "\344\273\216\346\226\207\344\273\266\345\212\240\350\275\275", nullptr));
        chipPropertiesGroup->setTitle(QCoreApplication::translate("PinEditorDialog", "\350\212\257\347\211\207\345\261\236\346\200\247", nullptr));
        nameLabel->setText(QCoreApplication::translate("PinEditorDialog", "\350\212\257\347\211\207\345\220\215\347\247\260:", nullptr));
        nameEdit->setPlaceholderText(QCoreApplication::translate("PinEditorDialog", "\350\257\267\350\276\223\345\205\245\350\212\257\347\211\207\345\220\215\347\247\260", nullptr));
        instanceLabel->setText(QCoreApplication::translate("PinEditorDialog", "\345\256\236\344\276\213\345\220\215\347\247\260:", nullptr));
        instanceEdit->setPlaceholderText(QCoreApplication::translate("PinEditorDialog", "\350\257\267\350\276\223\345\205\245\345\256\236\344\276\213\345\220\215\347\247\260", nullptr));
        colorLabel->setText(QCoreApplication::translate("PinEditorDialog", "\346\230\276\347\244\272\351\242\234\350\211\262:", nullptr));
        colorCombo->setItemText(0, QCoreApplication::translate("PinEditorDialog", "\346\265\205\347\201\260\350\211\262", nullptr));
        colorCombo->setItemText(1, QCoreApplication::translate("PinEditorDialog", "\350\223\235\350\211\262", nullptr));
        colorCombo->setItemText(2, QCoreApplication::translate("PinEditorDialog", "\347\273\277\350\211\262", nullptr));
        colorCombo->setItemText(3, QCoreApplication::translate("PinEditorDialog", "\351\273\204\350\211\262", nullptr));
        colorCombo->setItemText(4, QCoreApplication::translate("PinEditorDialog", "\347\272\242\350\211\262", nullptr));
        colorCombo->setItemText(5, QCoreApplication::translate("PinEditorDialog", "\351\235\222\350\211\262", nullptr));
        colorCombo->setItemText(6, QCoreApplication::translate("PinEditorDialog", "\346\264\213\347\272\242\350\211\262", nullptr));

        sizeLabel->setText(QCoreApplication::translate("PinEditorDialog", "\345\260\272\345\257\270:", nullptr));
        xLabel->setText(QCoreApplication::translate("PinEditorDialog", "\303\227", nullptr));
        resizableCheckBox->setText(QCoreApplication::translate("PinEditorDialog", "\345\220\257\347\224\250\345\244\247\345\260\217\350\260\203\346\225\264", nullptr));
        pinListGroup->setTitle(QCoreApplication::translate("PinEditorDialog", "\345\274\225\350\204\232\345\210\227\350\241\250", nullptr));
        pinPropertiesGroup->setTitle(QCoreApplication::translate("PinEditorDialog", "\345\274\225\350\204\232\345\261\236\346\200\247", nullptr));
        pinIdLabel->setText(QCoreApplication::translate("PinEditorDialog", "\345\274\225\350\204\232ID:", nullptr));
        sideLabel->setText(QCoreApplication::translate("PinEditorDialog", "\344\275\215\347\275\256:", nullptr));
        sideCombo->setItemText(0, QCoreApplication::translate("PinEditorDialog", "top", nullptr));
        sideCombo->setItemText(1, QCoreApplication::translate("PinEditorDialog", "bottom", nullptr));
        sideCombo->setItemText(2, QCoreApplication::translate("PinEditorDialog", "left", nullptr));
        sideCombo->setItemText(3, QCoreApplication::translate("PinEditorDialog", "right", nullptr));
        sideCombo->setItemText(4, QCoreApplication::translate("PinEditorDialog", "custom", nullptr));

        percentageLabel->setText(QCoreApplication::translate("PinEditorDialog", "\347\231\276\345\210\206\346\257\224:", nullptr));
        percentageSpin->setSuffix(QCoreApplication::translate("PinEditorDialog", "%", nullptr));
        acceptButton->setText(QCoreApplication::translate("PinEditorDialog", "\347\241\256\345\256\232", nullptr));
        cancelButton->setText(QCoreApplication::translate("PinEditorDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PinEditorDialog: public Ui_PinEditorDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PINEDITORDIALOG_H
