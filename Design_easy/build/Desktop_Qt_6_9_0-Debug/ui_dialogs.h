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
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialogs
{
public:
    QFormLayout *formLayout_2;
    QFormLayout *formLayout;
    QLabel *name;
    QLineEdit *nameEdit;
    QLabel *color;
    QComboBox *colorCombo;
    QLabel *label;
    QSpinBox *portSpin;
    QCheckBox *checkBox;
    QHBoxLayout *horizontalLayout;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    QListView *listView;
    QVBoxLayout *verticalLayout;
    QGraphicsView *pinGraphicsView;
    QComboBox *sideCombo;
    QDoubleSpinBox *percentageSpin;
    QPushButton *addPinButton;
    QPushButton *removePinButton;

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
        name = new QLabel(Dialogs);
        name->setObjectName("name");
        sizePolicy.setHeightForWidth(name->sizePolicy().hasHeightForWidth());
        name->setSizePolicy(sizePolicy);

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, name);

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


        formLayout->setLayout(5, QFormLayout::ItemRole::SpanningRole, horizontalLayout);

        listView = new QListView(Dialogs);
        listView->setObjectName("listView");

        formLayout->setWidget(4, QFormLayout::ItemRole::SpanningRole, listView);


        formLayout_2->setLayout(0, QFormLayout::ItemRole::LabelRole, formLayout);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        pinGraphicsView = new QGraphicsView(Dialogs);
        pinGraphicsView->setObjectName("pinGraphicsView");

        verticalLayout->addWidget(pinGraphicsView);

        sideCombo = new QComboBox(Dialogs);
        sideCombo->setObjectName("sideCombo");
        sizePolicy.setHeightForWidth(sideCombo->sizePolicy().hasHeightForWidth());
        sideCombo->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(sideCombo);

        percentageSpin = new QDoubleSpinBox(Dialogs);
        percentageSpin->setObjectName("percentageSpin");
        sizePolicy.setHeightForWidth(percentageSpin->sizePolicy().hasHeightForWidth());
        percentageSpin->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(percentageSpin);

        addPinButton = new QPushButton(Dialogs);
        addPinButton->setObjectName("addPinButton");

        verticalLayout->addWidget(addPinButton);

        removePinButton = new QPushButton(Dialogs);
        removePinButton->setObjectName("removePinButton");

        verticalLayout->addWidget(removePinButton);


        formLayout_2->setLayout(0, QFormLayout::ItemRole::FieldRole, verticalLayout);


        retranslateUi(Dialogs);

        QMetaObject::connectSlotsByName(Dialogs);
    } // setupUi

    void retranslateUi(QDialog *Dialogs)
    {
        Dialogs->setWindowTitle(QCoreApplication::translate("Dialogs", "Dialogs", nullptr));
        name->setText(QCoreApplication::translate("Dialogs", "\345\220\215\347\247\260:", nullptr));
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
        addPinButton->setText(QCoreApplication::translate("Dialogs", "\346\267\273\345\212\240\347\253\257\345\217\243", nullptr));
        removePinButton->setText(QCoreApplication::translate("Dialogs", "\347\247\273\351\231\244\345\274\225\350\204\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialogs: public Ui_Dialogs {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGS_H
