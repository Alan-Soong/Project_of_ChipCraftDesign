/********************************************************************************
** Form generated from reading UI file 'dialogs.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGS_H
#define UI_DIALOGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_Dialogs
{
public:
    QFormLayout *formLayout_2;
    QFormLayout *formLayout;
    QLineEdit *nameEdit;
    QLabel *color;
    QComboBox *colorCombo;
    QLabel *label;
    QSpinBox *portSpin;
    QCheckBox *checkBox;
    QHBoxLayout *horizontalLayout;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    QLabel *name;
    QListView *listView;
    QListView *listView_2;

    void setupUi(QDialog *Dialogs)
    {
        if (Dialogs->objectName().isEmpty())
            Dialogs->setObjectName("Dialogs");
        Dialogs->resize(621, 388);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Dialogs->sizePolicy().hasHeightForWidth());
        Dialogs->setSizePolicy(sizePolicy);
        formLayout_2 = new QFormLayout(Dialogs);
        formLayout_2->setObjectName("formLayout_2");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        formLayout->setFieldGrowthPolicy(QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow);
        formLayout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::DontWrapRows);
        formLayout->setLabelAlignment(Qt::AlignmentFlag::AlignCenter);
        formLayout->setFormAlignment(Qt::AlignmentFlag::AlignCenter);
        nameEdit = new QLineEdit(Dialogs);
        nameEdit->setObjectName("nameEdit");
        sizePolicy.setHeightForWidth(nameEdit->sizePolicy().hasHeightForWidth());
        nameEdit->setSizePolicy(sizePolicy);

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, nameEdit);

        color = new QLabel(Dialogs);
        color->setObjectName("color");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, color);

        colorCombo = new QComboBox(Dialogs);
        colorCombo->addItem(QString());
        colorCombo->addItem(QString());
        colorCombo->addItem(QString());
        colorCombo->addItem(QString());
        colorCombo->addItem(QString());
        colorCombo->setObjectName("colorCombo");
        sizePolicy.setHeightForWidth(colorCombo->sizePolicy().hasHeightForWidth());
        colorCombo->setSizePolicy(sizePolicy);

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, colorCombo);

        label = new QLabel(Dialogs);
        label->setObjectName("label");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label);

        portSpin = new QSpinBox(Dialogs);
        portSpin->setObjectName("portSpin");
        sizePolicy.setHeightForWidth(portSpin->sizePolicy().hasHeightForWidth());
        portSpin->setSizePolicy(sizePolicy);

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, portSpin);

        checkBox = new QCheckBox(Dialogs);
        checkBox->setObjectName("checkBox");
        sizePolicy.setHeightForWidth(checkBox->sizePolicy().hasHeightForWidth());
        checkBox->setSizePolicy(sizePolicy);

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, checkBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        saveButton = new QPushButton(Dialogs);
        saveButton->setObjectName("saveButton");
        sizePolicy.setHeightForWidth(saveButton->sizePolicy().hasHeightForWidth());
        saveButton->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(saveButton);

        cancelButton = new QPushButton(Dialogs);
        cancelButton->setObjectName("cancelButton");
        sizePolicy.setHeightForWidth(cancelButton->sizePolicy().hasHeightForWidth());
        cancelButton->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(cancelButton);


        formLayout->setLayout(4, QFormLayout::ItemRole::SpanningRole, horizontalLayout);

        name = new QLabel(Dialogs);
        name->setObjectName("name");
        sizePolicy.setHeightForWidth(name->sizePolicy().hasHeightForWidth());
        name->setSizePolicy(sizePolicy);

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, name);


        formLayout_2->setLayout(0, QFormLayout::ItemRole::LabelRole, formLayout);

        listView = new QListView(Dialogs);
        listView->setObjectName("listView");
        sizePolicy.setHeightForWidth(listView->sizePolicy().hasHeightForWidth());
        listView->setSizePolicy(sizePolicy);

        formLayout_2->setWidget(0, QFormLayout::ItemRole::FieldRole, listView);

        listView_2 = new QListView(Dialogs);
        listView_2->setObjectName("listView_2");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::SpanningRole, listView_2);


        retranslateUi(Dialogs);

        QMetaObject::connectSlotsByName(Dialogs);
    } // setupUi

    void retranslateUi(QDialog *Dialogs)
    {
        Dialogs->setWindowTitle(QCoreApplication::translate("Dialogs", "Dialogs", nullptr));
        color->setText(QCoreApplication::translate("Dialogs", "\351\242\234\350\211\262:", nullptr));
        colorCombo->setItemText(0, QString());
        colorCombo->setItemText(1, QCoreApplication::translate("Dialogs", "blue", nullptr));
        colorCombo->setItemText(2, QCoreApplication::translate("Dialogs", "red", nullptr));
        colorCombo->setItemText(3, QCoreApplication::translate("Dialogs", "yellow", nullptr));
        colorCombo->setItemText(4, QCoreApplication::translate("Dialogs", "gray", nullptr));

        label->setText(QCoreApplication::translate("Dialogs", "\346\216\245\345\217\243\346\225\260\351\207\217:", nullptr));
        checkBox->setText(QCoreApplication::translate("Dialogs", "\345\220\257\347\224\250\350\260\203\346\225\264\345\244\247\345\260\217", nullptr));
        saveButton->setText(QCoreApplication::translate("Dialogs", "\344\277\235\345\255\230", nullptr));
        cancelButton->setText(QCoreApplication::translate("Dialogs", "\345\217\226\346\266\210", nullptr));
        name->setText(QCoreApplication::translate("Dialogs", "\345\220\215\347\247\260:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialogs: public Ui_Dialogs {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGS_H
