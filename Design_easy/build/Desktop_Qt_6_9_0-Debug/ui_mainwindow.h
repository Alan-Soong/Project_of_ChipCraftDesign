/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionsetting_original;
    QAction *actionsetLeftDock;
    QAction *actionhideLeftDock;
    QAction *actionnew;
    QAction *actionsave;
    QAction *actionopen;
    QAction *actionopenRecent;
    QAction *actionexit;
    QAction *actionsaveOther;
    QAction *actionaddRect;
    QAction *actionaboutUs;
    QAction *actionhelpDocument;
    QAction *actionmm;
    QAction *actioncm;
    QAction *actiondm;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QGraphicsView *canvasView;
    QMenuBar *menuBar;
    QMenu *menudocument;
    QMenu *menuedit;
    QMenu *menuview;
    QMenu *menuswich_unit;
    QMenu *menusettings;
    QMenu *menufile;
    QMenu *menuhelp;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *addRectangleButton;
    QSpacerItem *verticalSpacer;
    QPushButton *selectButton;
    QSpacerItem *verticalSpacer_2;
    QPushButton *undoButton;
    QSpacerItem *verticalSpacer_3;
    QPushButton *redoButton;
    QSpacerItem *verticalSpacer_4;
    QPushButton *deleteButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1004, 606);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        actionsetting_original = new QAction(MainWindow);
        actionsetting_original->setObjectName("actionsetting_original");
        actionsetLeftDock = new QAction(MainWindow);
        actionsetLeftDock->setObjectName("actionsetLeftDock");
        actionhideLeftDock = new QAction(MainWindow);
        actionhideLeftDock->setObjectName("actionhideLeftDock");
        actionnew = new QAction(MainWindow);
        actionnew->setObjectName("actionnew");
        actionsave = new QAction(MainWindow);
        actionsave->setObjectName("actionsave");
        actionopen = new QAction(MainWindow);
        actionopen->setObjectName("actionopen");
        actionopenRecent = new QAction(MainWindow);
        actionopenRecent->setObjectName("actionopenRecent");
        actionexit = new QAction(MainWindow);
        actionexit->setObjectName("actionexit");
        actionsaveOther = new QAction(MainWindow);
        actionsaveOther->setObjectName("actionsaveOther");
        actionaddRect = new QAction(MainWindow);
        actionaddRect->setObjectName("actionaddRect");
        actionaboutUs = new QAction(MainWindow);
        actionaboutUs->setObjectName("actionaboutUs");
        actionhelpDocument = new QAction(MainWindow);
        actionhelpDocument->setObjectName("actionhelpDocument");
        actionmm = new QAction(MainWindow);
        actionmm->setObjectName("actionmm");
        actioncm = new QAction(MainWindow);
        actioncm->setObjectName("actioncm");
        actiondm = new QAction(MainWindow);
        actiondm->setObjectName("actiondm");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        canvasView = new QGraphicsView(centralwidget);
        canvasView->setObjectName("canvasView");
        canvasView->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        canvasView->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

        horizontalLayout->addWidget(canvasView);

        MainWindow->setCentralWidget(centralwidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1004, 27));
        menudocument = new QMenu(menuBar);
        menudocument->setObjectName("menudocument");
        menuedit = new QMenu(menuBar);
        menuedit->setObjectName("menuedit");
        menuview = new QMenu(menuBar);
        menuview->setObjectName("menuview");
        menuswich_unit = new QMenu(menuview);
        menuswich_unit->setObjectName("menuswich_unit");
        menusettings = new QMenu(menuBar);
        menusettings->setObjectName("menusettings");
        menufile = new QMenu(menuBar);
        menufile->setObjectName("menufile");
        menuhelp = new QMenu(menuBar);
        menuhelp->setObjectName("menuhelp");
        MainWindow->setMenuBar(menuBar);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName("dockWidget");
        sizePolicy.setHeightForWidth(dockWidget->sizePolicy().hasHeightForWidth());
        dockWidget->setSizePolicy(sizePolicy);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName("dockWidgetContents");
        sizePolicy.setHeightForWidth(dockWidgetContents->sizePolicy().hasHeightForWidth());
        dockWidgetContents->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setObjectName("gridLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        addRectangleButton = new QPushButton(dockWidgetContents);
        addRectangleButton->setObjectName("addRectangleButton");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(addRectangleButton->sizePolicy().hasHeightForWidth());
        addRectangleButton->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(addRectangleButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        selectButton = new QPushButton(dockWidgetContents);
        selectButton->setObjectName("selectButton");
        sizePolicy1.setHeightForWidth(selectButton->sizePolicy().hasHeightForWidth());
        selectButton->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(selectButton);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        undoButton = new QPushButton(dockWidgetContents);
        undoButton->setObjectName("undoButton");
        sizePolicy1.setHeightForWidth(undoButton->sizePolicy().hasHeightForWidth());
        undoButton->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(undoButton);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        redoButton = new QPushButton(dockWidgetContents);
        redoButton->setObjectName("redoButton");
        sizePolicy1.setHeightForWidth(redoButton->sizePolicy().hasHeightForWidth());
        redoButton->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(redoButton);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer_4);

        deleteButton = new QPushButton(dockWidgetContents);
        deleteButton->setObjectName("deleteButton");
        sizePolicy1.setHeightForWidth(deleteButton->sizePolicy().hasHeightForWidth());
        deleteButton->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(deleteButton);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

        dockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWidget);
        dockWidget->raise();

        menuBar->addAction(menufile->menuAction());
        menuBar->addAction(menuedit->menuAction());
        menuBar->addAction(menusettings->menuAction());
        menuBar->addAction(menuview->menuAction());
        menuBar->addAction(menudocument->menuAction());
        menuBar->addAction(menuhelp->menuAction());
        menuedit->addAction(actionaddRect);
        menuview->addAction(actionsetting_original);
        menuview->addAction(menuswich_unit->menuAction());
        menuswich_unit->addAction(actionmm);
        menuswich_unit->addAction(actioncm);
        menuswich_unit->addAction(actiondm);
        menusettings->addAction(actionsetLeftDock);
        menusettings->addAction(actionhideLeftDock);
        menufile->addAction(actionnew);
        menufile->addAction(actionopen);
        menufile->addAction(actionopenRecent);
        menufile->addAction(actionsave);
        menufile->addAction(actionsaveOther);
        menufile->addAction(actionexit);
        menuhelp->addAction(actionaboutUs);
        menuhelp->addAction(actionhelpDocument);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Chip Layout Editor", nullptr));
        actionsetting_original->setText(QCoreApplication::translate("MainWindow", "setting_original", nullptr));
        actionsetLeftDock->setText(QCoreApplication::translate("MainWindow", "setLeftDock", nullptr));
        actionhideLeftDock->setText(QCoreApplication::translate("MainWindow", "hideLeftDock", nullptr));
        actionnew->setText(QCoreApplication::translate("MainWindow", "new", nullptr));
        actionsave->setText(QCoreApplication::translate("MainWindow", "save", nullptr));
        actionopen->setText(QCoreApplication::translate("MainWindow", "open", nullptr));
        actionopenRecent->setText(QCoreApplication::translate("MainWindow", "openRecent", nullptr));
        actionexit->setText(QCoreApplication::translate("MainWindow", "exit", nullptr));
        actionsaveOther->setText(QCoreApplication::translate("MainWindow", "saveOther", nullptr));
        actionaddRect->setText(QCoreApplication::translate("MainWindow", "addRect", nullptr));
        actionaboutUs->setText(QCoreApplication::translate("MainWindow", "aboutUs", nullptr));
        actionhelpDocument->setText(QCoreApplication::translate("MainWindow", "helpDocument", nullptr));
        actionmm->setText(QCoreApplication::translate("MainWindow", "mm", nullptr));
        actioncm->setText(QCoreApplication::translate("MainWindow", "cm", nullptr));
        actiondm->setText(QCoreApplication::translate("MainWindow", "dm", nullptr));
        menudocument->setTitle(QCoreApplication::translate("MainWindow", "document", nullptr));
        menuedit->setTitle(QCoreApplication::translate("MainWindow", "edit", nullptr));
        menuview->setTitle(QCoreApplication::translate("MainWindow", "setting", nullptr));
        menuswich_unit->setTitle(QCoreApplication::translate("MainWindow", "swich unit", nullptr));
        menusettings->setTitle(QCoreApplication::translate("MainWindow", "view", nullptr));
        menufile->setTitle(QCoreApplication::translate("MainWindow", "file", nullptr));
        menuhelp->setTitle(QCoreApplication::translate("MainWindow", "help", nullptr));
        addRectangleButton->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240", nullptr));
        selectButton->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251", nullptr));
        undoButton->setText(QCoreApplication::translate("MainWindow", "\346\222\244\351\224\200", nullptr));
        redoButton->setText(QCoreApplication::translate("MainWindow", "\351\207\215\345\201\232", nullptr));
        deleteButton->setText(QCoreApplication::translate("MainWindow", "\345\210\240\351\231\244", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
