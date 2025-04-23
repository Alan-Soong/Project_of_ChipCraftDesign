/********************************************************************************
** Form generated from reading UI file 'mytestproject.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYTESTPROJECT_H
#define UI_MYTESTPROJECT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyTestProject
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QWidget *leftPanel;
    QVBoxLayout *leftLayout;
    QTableWidget *twTemplates;
    QHBoxLayout *templateBtnLayout;
    QPushButton *btnAddTemplate;
    QPushButton *btnRemoveTemplate;
    QPushButton *btnEditTemplate;
    QGroupBox *grpTemplate;
    QFormLayout *formTemp;
    QLabel *lblTemplateName;
    QLineEdit *leTemplateName;
    QLabel *lblTemplateWidth;
    QSpinBox *sbTemplateWidth;
    QLabel *lblTemplateHeight;
    QSpinBox *sbTemplateHeight;
    QHBoxLayout *saveTempLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSaveTemplate;
    QWidget *rightPanel;
    QVBoxLayout *rightLayout;
    QGraphicsView *graphicsView;
    QGroupBox *grpPort;
    QFormLayout *formPort;
    QLabel *lblPortGroupName;
    QLineEdit *lePortName;
    QLabel *lblPortX;
    QSpinBox *sbPortX;
    QLabel *lblPortY;
    QSpinBox *sbPortY;
    QLabel *lblPortWidth;
    QSpinBox *sbPortWidth;
    QLabel *lblPortHeight;
    QSpinBox *sbPortHeight;
    QHBoxLayout *portBtnLayout;
    QPushButton *btnAddPortGroup;
    QPushButton *btnRemovePortGroup;
    QSpacerItem *horizontalSpacer2;
    QTableWidget *twPortGroups;
    QMenuBar *menubar;
    QMenu *menuDesign;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MyTestProject)
    {
        if (MyTestProject->objectName().isEmpty())
            MyTestProject->setObjectName("MyTestProject");
        MyTestProject->resize(1599, 966);
        centralwidget = new QWidget(MyTestProject);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Orientation::Horizontal);
        leftPanel = new QWidget(splitter);
        leftPanel->setObjectName("leftPanel");
        leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setObjectName("leftLayout");
        leftLayout->setContentsMargins(0, 0, 0, 0);
        twTemplates = new QTableWidget(leftPanel);
        if (twTemplates->columnCount() < 3)
            twTemplates->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        twTemplates->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        twTemplates->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        twTemplates->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        twTemplates->setObjectName("twTemplates");

        leftLayout->addWidget(twTemplates);

        templateBtnLayout = new QHBoxLayout();
        templateBtnLayout->setObjectName("templateBtnLayout");
        btnAddTemplate = new QPushButton(leftPanel);
        btnAddTemplate->setObjectName("btnAddTemplate");

        templateBtnLayout->addWidget(btnAddTemplate);

        btnRemoveTemplate = new QPushButton(leftPanel);
        btnRemoveTemplate->setObjectName("btnRemoveTemplate");

        templateBtnLayout->addWidget(btnRemoveTemplate);

        btnEditTemplate = new QPushButton(leftPanel);
        btnEditTemplate->setObjectName("btnEditTemplate");

        templateBtnLayout->addWidget(btnEditTemplate);


        leftLayout->addLayout(templateBtnLayout);

        grpTemplate = new QGroupBox(leftPanel);
        grpTemplate->setObjectName("grpTemplate");
        formTemp = new QFormLayout(grpTemplate);
        formTemp->setObjectName("formTemp");
        lblTemplateName = new QLabel(grpTemplate);
        lblTemplateName->setObjectName("lblTemplateName");

        formTemp->setWidget(0, QFormLayout::ItemRole::LabelRole, lblTemplateName);

        leTemplateName = new QLineEdit(grpTemplate);
        leTemplateName->setObjectName("leTemplateName");

        formTemp->setWidget(0, QFormLayout::ItemRole::FieldRole, leTemplateName);

        lblTemplateWidth = new QLabel(grpTemplate);
        lblTemplateWidth->setObjectName("lblTemplateWidth");

        formTemp->setWidget(1, QFormLayout::ItemRole::LabelRole, lblTemplateWidth);

        sbTemplateWidth = new QSpinBox(grpTemplate);
        sbTemplateWidth->setObjectName("sbTemplateWidth");
        sbTemplateWidth->setMinimum(1);
        sbTemplateWidth->setMaximum(10000);

        formTemp->setWidget(1, QFormLayout::ItemRole::FieldRole, sbTemplateWidth);

        lblTemplateHeight = new QLabel(grpTemplate);
        lblTemplateHeight->setObjectName("lblTemplateHeight");

        formTemp->setWidget(2, QFormLayout::ItemRole::LabelRole, lblTemplateHeight);

        sbTemplateHeight = new QSpinBox(grpTemplate);
        sbTemplateHeight->setObjectName("sbTemplateHeight");
        sbTemplateHeight->setMinimum(1);
        sbTemplateHeight->setMaximum(10000);

        formTemp->setWidget(2, QFormLayout::ItemRole::FieldRole, sbTemplateHeight);

        saveTempLayout = new QHBoxLayout();
        saveTempLayout->setObjectName("saveTempLayout");
        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        saveTempLayout->addItem(horizontalSpacer);

        btnSaveTemplate = new QPushButton(grpTemplate);
        btnSaveTemplate->setObjectName("btnSaveTemplate");

        saveTempLayout->addWidget(btnSaveTemplate);


        formTemp->setLayout(3, QFormLayout::ItemRole::FieldRole, saveTempLayout);


        leftLayout->addWidget(grpTemplate);

        splitter->addWidget(leftPanel);
        rightPanel = new QWidget(splitter);
        rightPanel->setObjectName("rightPanel");
        rightLayout = new QVBoxLayout(rightPanel);
        rightLayout->setObjectName("rightLayout");
        rightLayout->setContentsMargins(0, 0, 0, 0);
        graphicsView = new QGraphicsView(rightPanel);
        graphicsView->setObjectName("graphicsView");

        rightLayout->addWidget(graphicsView);

        grpPort = new QGroupBox(rightPanel);
        grpPort->setObjectName("grpPort");
        formPort = new QFormLayout(grpPort);
        formPort->setObjectName("formPort");
        lblPortGroupName = new QLabel(grpPort);
        lblPortGroupName->setObjectName("lblPortGroupName");

        formPort->setWidget(0, QFormLayout::ItemRole::LabelRole, lblPortGroupName);

        lePortName = new QLineEdit(grpPort);
        lePortName->setObjectName("lePortName");

        formPort->setWidget(0, QFormLayout::ItemRole::FieldRole, lePortName);

        lblPortX = new QLabel(grpPort);
        lblPortX->setObjectName("lblPortX");

        formPort->setWidget(1, QFormLayout::ItemRole::LabelRole, lblPortX);

        sbPortX = new QSpinBox(grpPort);
        sbPortX->setObjectName("sbPortX");
        sbPortX->setMaximum(10000);

        formPort->setWidget(1, QFormLayout::ItemRole::FieldRole, sbPortX);

        lblPortY = new QLabel(grpPort);
        lblPortY->setObjectName("lblPortY");

        formPort->setWidget(2, QFormLayout::ItemRole::LabelRole, lblPortY);

        sbPortY = new QSpinBox(grpPort);
        sbPortY->setObjectName("sbPortY");
        sbPortY->setMaximum(10000);

        formPort->setWidget(2, QFormLayout::ItemRole::FieldRole, sbPortY);

        lblPortWidth = new QLabel(grpPort);
        lblPortWidth->setObjectName("lblPortWidth");

        formPort->setWidget(3, QFormLayout::ItemRole::LabelRole, lblPortWidth);

        sbPortWidth = new QSpinBox(grpPort);
        sbPortWidth->setObjectName("sbPortWidth");
        sbPortWidth->setMinimum(1);
        sbPortWidth->setMaximum(10000);

        formPort->setWidget(3, QFormLayout::ItemRole::FieldRole, sbPortWidth);

        lblPortHeight = new QLabel(grpPort);
        lblPortHeight->setObjectName("lblPortHeight");

        formPort->setWidget(4, QFormLayout::ItemRole::LabelRole, lblPortHeight);

        sbPortHeight = new QSpinBox(grpPort);
        sbPortHeight->setObjectName("sbPortHeight");
        sbPortHeight->setMinimum(1);
        sbPortHeight->setMaximum(10000);

        formPort->setWidget(4, QFormLayout::ItemRole::FieldRole, sbPortHeight);

        portBtnLayout = new QHBoxLayout();
        portBtnLayout->setObjectName("portBtnLayout");
        btnAddPortGroup = new QPushButton(grpPort);
        btnAddPortGroup->setObjectName("btnAddPortGroup");

        portBtnLayout->addWidget(btnAddPortGroup);

        btnRemovePortGroup = new QPushButton(grpPort);
        btnRemovePortGroup->setObjectName("btnRemovePortGroup");

        portBtnLayout->addWidget(btnRemovePortGroup);

        horizontalSpacer2 = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        portBtnLayout->addItem(horizontalSpacer2);


        formPort->setLayout(5, QFormLayout::ItemRole::FieldRole, portBtnLayout);


        rightLayout->addWidget(grpPort);

        twPortGroups = new QTableWidget(rightPanel);
        if (twPortGroups->columnCount() < 5)
            twPortGroups->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        twPortGroups->setHorizontalHeaderItem(0, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        twPortGroups->setHorizontalHeaderItem(1, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        twPortGroups->setHorizontalHeaderItem(2, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        twPortGroups->setHorizontalHeaderItem(3, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        twPortGroups->setHorizontalHeaderItem(4, __qtablewidgetitem7);
        twPortGroups->setObjectName("twPortGroups");

        rightLayout->addWidget(twPortGroups);

        splitter->addWidget(rightPanel);

        horizontalLayout->addWidget(splitter);

        MyTestProject->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MyTestProject);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1599, 27));
        menuDesign = new QMenu(menubar);
        menuDesign->setObjectName("menuDesign");
        MyTestProject->setMenuBar(menubar);
        statusbar = new QStatusBar(MyTestProject);
        statusbar->setObjectName("statusbar");
        MyTestProject->setStatusBar(statusbar);

        menubar->addAction(menuDesign->menuAction());

        retranslateUi(MyTestProject);

        QMetaObject::connectSlotsByName(MyTestProject);
    } // setupUi

    void retranslateUi(QMainWindow *MyTestProject)
    {
        QTableWidgetItem *___qtablewidgetitem = twTemplates->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MyTestProject", "\346\250\241\346\235\277\345\220\215\347\247\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = twTemplates->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MyTestProject", "\345\256\275\345\272\246", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = twTemplates->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MyTestProject", "\351\253\230\345\272\246", nullptr));
        btnAddTemplate->setText(QCoreApplication::translate("MyTestProject", "\346\226\260\345\242\236\346\250\241\346\235\277", nullptr));
        btnRemoveTemplate->setText(QCoreApplication::translate("MyTestProject", "\345\210\240\351\231\244\346\250\241\346\235\277", nullptr));
        btnEditTemplate->setText(QCoreApplication::translate("MyTestProject", "\344\277\256\346\224\271\346\250\241\346\235\277", nullptr));
        grpTemplate->setTitle(QCoreApplication::translate("MyTestProject", "\346\250\241\346\235\277\347\274\226\350\276\221", nullptr));
        lblTemplateName->setText(QCoreApplication::translate("MyTestProject", "\345\220\215\347\247\260", nullptr));
        lblTemplateWidth->setText(QCoreApplication::translate("MyTestProject", "\345\256\275\345\272\246", nullptr));
        lblTemplateHeight->setText(QCoreApplication::translate("MyTestProject", "\351\253\230\345\272\246", nullptr));
        btnSaveTemplate->setText(QCoreApplication::translate("MyTestProject", "\344\277\235\345\255\230\346\250\241\346\235\277", nullptr));
        grpPort->setTitle(QCoreApplication::translate("MyTestProject", "\347\253\257\345\217\243\347\273\204\347\256\241\347\220\206", nullptr));
        lblPortGroupName->setText(QCoreApplication::translate("MyTestProject", "\347\253\257\345\217\243\347\273\204\345\220\215\347\247\260", nullptr));
        lblPortX->setText(QCoreApplication::translate("MyTestProject", "X", nullptr));
        lblPortY->setText(QCoreApplication::translate("MyTestProject", "Y", nullptr));
        lblPortWidth->setText(QCoreApplication::translate("MyTestProject", "\345\256\275\345\272\246", nullptr));
        lblPortHeight->setText(QCoreApplication::translate("MyTestProject", "\351\253\230\345\272\246", nullptr));
        btnAddPortGroup->setText(QCoreApplication::translate("MyTestProject", "\346\267\273\345\212\240\347\253\257\345\217\243\347\273\204", nullptr));
        btnRemovePortGroup->setText(QCoreApplication::translate("MyTestProject", "\345\210\240\351\231\244\351\200\211\344\270\255\347\253\257\345\217\243\347\273\204", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = twPortGroups->horizontalHeaderItem(0);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MyTestProject", "\347\253\257\345\217\243\347\273\204\345\220\215\347\247\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = twPortGroups->horizontalHeaderItem(1);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MyTestProject", "X", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = twPortGroups->horizontalHeaderItem(2);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MyTestProject", "Y", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = twPortGroups->horizontalHeaderItem(3);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MyTestProject", "\345\256\275\345\272\246", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = twPortGroups->horizontalHeaderItem(4);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MyTestProject", "\351\253\230\345\272\246", nullptr));
        menuDesign->setTitle(QCoreApplication::translate("MyTestProject", "Design", nullptr));
        (void)MyTestProject;
    } // retranslateUi

};

namespace Ui {
    class MyTestProject: public Ui_MyTestProject {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYTESTPROJECT_H
