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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialogs
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *nameEdit;
    QLabel *label_2;
    QComboBox *colorCombo;
    QLabel *label_3;
    QComboBox *sideCombo;
    QLabel *label_4;
    QDoubleSpinBox *percentageSpin;
    QLabel *label_5;
    QDoubleSpinBox *xSpinBox;
    QLabel *label_6;
    QDoubleSpinBox *ySpinBox;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *addPinButton;
    QPushButton *removePinButton;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QSpacerItem *verticalSpacer;
    QGraphicsView *pinGraphicsView;

    void setupUi(QDialog *Dialogs)
    {
        if (Dialogs->objectName().isEmpty())
            Dialogs->setObjectName("Dialogs");
        Dialogs->resize(918, 518);
        horizontalLayout = new QHBoxLayout(Dialogs);
        horizontalLayout->setObjectName("horizontalLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        label = new QLabel(Dialogs);
        label->setObjectName("label");

        verticalLayout->addWidget(label);

        nameEdit = new QLineEdit(Dialogs);
        nameEdit->setObjectName("nameEdit");

        verticalLayout->addWidget(nameEdit);

        label_2 = new QLabel(Dialogs);
        label_2->setObjectName("label_2");

        verticalLayout->addWidget(label_2);

        colorCombo = new QComboBox(Dialogs);
        colorCombo->setObjectName("colorCombo");

        verticalLayout->addWidget(colorCombo);

        label_3 = new QLabel(Dialogs);
        label_3->setObjectName("label_3");

        verticalLayout->addWidget(label_3);

        sideCombo = new QComboBox(Dialogs);
        sideCombo->setObjectName("sideCombo");

        verticalLayout->addWidget(sideCombo);

        label_4 = new QLabel(Dialogs);
        label_4->setObjectName("label_4");

        verticalLayout->addWidget(label_4);

        percentageSpin = new QDoubleSpinBox(Dialogs);
        percentageSpin->setObjectName("percentageSpin");

        verticalLayout->addWidget(percentageSpin);

        label_5 = new QLabel(Dialogs);
        label_5->setObjectName("label_5");

        verticalLayout->addWidget(label_5);

        xSpinBox = new QDoubleSpinBox(Dialogs);
        xSpinBox->setObjectName("xSpinBox");

        verticalLayout->addWidget(xSpinBox);

        label_6 = new QLabel(Dialogs);
        label_6->setObjectName("label_6");

        verticalLayout->addWidget(label_6);

        ySpinBox = new QDoubleSpinBox(Dialogs);
        ySpinBox->setObjectName("ySpinBox");

        verticalLayout->addWidget(ySpinBox);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        addPinButton = new QPushButton(Dialogs);
        addPinButton->setObjectName("addPinButton");

        horizontalLayout_2->addWidget(addPinButton);

        removePinButton = new QPushButton(Dialogs);
        removePinButton->setObjectName("removePinButton");

        horizontalLayout_2->addWidget(removePinButton);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        saveButton = new QPushButton(Dialogs);
        saveButton->setObjectName("saveButton");

        horizontalLayout_3->addWidget(saveButton);

        loadButton = new QPushButton(Dialogs);
        loadButton->setObjectName("loadButton");

        horizontalLayout_3->addWidget(loadButton);


        verticalLayout->addLayout(horizontalLayout_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);

        pinGraphicsView = new QGraphicsView(Dialogs);
        pinGraphicsView->setObjectName("pinGraphicsView");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(pinGraphicsView->sizePolicy().hasHeightForWidth());
        pinGraphicsView->setSizePolicy(sizePolicy);
        pinGraphicsView->setMinimumSize(QSize(500, 500));

        horizontalLayout->addWidget(pinGraphicsView);


        retranslateUi(Dialogs);

        QMetaObject::connectSlotsByName(Dialogs);
    } // setupUi

    void retranslateUi(QDialog *Dialogs)
    {
        Dialogs->setWindowTitle(QCoreApplication::translate("Dialogs", "\350\256\276\347\275\256", nullptr));
        label->setText(QCoreApplication::translate("Dialogs", "\345\220\215\347\247\260\357\274\232", nullptr));
        label_2->setText(QCoreApplication::translate("Dialogs", "\351\242\234\350\211\262\357\274\232", nullptr));
        label_3->setText(QCoreApplication::translate("Dialogs", "\344\275\215\347\275\256\357\274\232", nullptr));
        label_4->setText(QCoreApplication::translate("Dialogs", "\347\231\276\345\210\206\346\257\224\357\274\232", nullptr));
        percentageSpin->setSuffix(QCoreApplication::translate("Dialogs", "%", nullptr));
        label_5->setText(QCoreApplication::translate("Dialogs", "X\345\235\220\346\240\207\357\274\232", nullptr));
        label_6->setText(QCoreApplication::translate("Dialogs", "Y\345\235\220\346\240\207\357\274\232", nullptr));
        addPinButton->setText(QCoreApplication::translate("Dialogs", "\346\267\273\345\212\240\347\253\257\345\217\243", nullptr));
        removePinButton->setText(QCoreApplication::translate("Dialogs", "\345\210\240\351\231\244\347\253\257\345\217\243", nullptr));
        saveButton->setText(QCoreApplication::translate("Dialogs", "\344\277\235\345\255\230", nullptr));
        loadButton->setText(QCoreApplication::translate("Dialogs", "\345\212\240\350\275\275", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialogs: public Ui_Dialogs {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGS_H
