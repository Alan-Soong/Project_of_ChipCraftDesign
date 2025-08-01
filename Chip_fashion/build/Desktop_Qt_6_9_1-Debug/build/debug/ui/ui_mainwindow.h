/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
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
    QMenu *menufile;
    QMenu *menuedit;
    QMenu *menusettings;
    QMenu *menu;
    QMenu *menuview;
    QMenu *menudocument;
    QMenu *menuhelp;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_main;
    QLabel *toolboxLabel;
    QPushButton *addRectangleButton;
    QFrame *line_1;
    QLabel *editLabel;
    QPushButton *selectButton;
    QPushButton *undoButton;
    QPushButton *redoButton;
    QPushButton *deleteButton;
    QSpacerItem *verticalSpacer;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1200, 800);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setStyleSheet(QString::fromUtf8("/* \344\270\273\347\252\227\345\217\243\346\240\267\345\274\217 */\n"
"QMainWindow {\n"
"    background-color: #f3f3f3;\n"
"    color: #323130;\n"
"}\n"
"\n"
"/* \350\217\234\345\215\225\346\240\217\346\240\267\345\274\217 */\n"
"QMenuBar {\n"
"    background-color: #ffffff;\n"
"    border-bottom: 1px solid #e1dfdd;\n"
"    font-size: 14px;\n"
"    padding: 4px;\n"
"}\n"
"\n"
"QMenuBar::item {\n"
"    background-color: transparent;\n"
"    padding: 8px 16px;\n"
"    margin: 0px 2px;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QMenuBar::item:selected {\n"
"    background-color: #f3f2f1;\n"
"    color: #323130;\n"
"}\n"
"\n"
"QMenuBar::item:pressed {\n"
"    background-color: #edebe9;\n"
"}\n"
"\n"
"/* \350\217\234\345\215\225\346\240\267\345\274\217 */\n"
"QMenu {\n"
"    background-color: #ffffff;\n"
"    border: 1px solid #e1dfdd;\n"
"    border-radius: 8px;\n"
"    padding: 8px 0px;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"QMenu::item {\n"
"    padding: 8px 24px;\n"
"    margin: 2px 8px;\n"
"    border-radius:"
                        " 4px;\n"
"}\n"
"\n"
"QMenu::item:selected {\n"
"    background-color: #f3f2f1;\n"
"    color: #323130;\n"
"}\n"
"\n"
"/* \345\201\234\351\235\240\347\252\227\345\217\243\346\240\267\345\274\217 */\n"
"QDockWidget {\n"
"    background-color: #faf9f8;\n"
"    border: 1px solid #e1dfdd;\n"
"    border-radius: 8px;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"QDockWidget::title {\n"
"    background-color: #ffffff;\n"
"    border-bottom: 1px solid #e1dfdd;\n"
"    padding: 12px;\n"
"    font-weight: 600;\n"
"    color: #323130;\n"
"}"));
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
        centralwidget->setStyleSheet(QString::fromUtf8("QWidget {\n"
"    background-color: #ffffff;\n"
"    border-radius: 8px;\n"
"}"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setSpacing(8);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(8, 8, 8, 8);
        canvasView = new QGraphicsView(centralwidget);
        canvasView->setObjectName("canvasView");
        canvasView->setStyleSheet(QString::fromUtf8("QGraphicsView {\n"
"    background-color: #ffffff;\n"
"    border: 2px solid #e1dfdd;\n"
"    border-radius: 8px;\n"
"}\n"
"\n"
"QGraphicsView:focus {\n"
"    border-color: #0078d4;\n"
"}"));
        canvasView->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        canvasView->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

        horizontalLayout->addWidget(canvasView);

        MainWindow->setCentralWidget(centralwidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1200, 45));
        menufile = new QMenu(menuBar);
        menufile->setObjectName("menufile");
        menuedit = new QMenu(menuBar);
        menuedit->setObjectName("menuedit");
        menusettings = new QMenu(menuBar);
        menusettings->setObjectName("menusettings");
        menu = new QMenu(menusettings);
        menu->setObjectName("menu");
        menuview = new QMenu(menuBar);
        menuview->setObjectName("menuview");
        menudocument = new QMenu(menuBar);
        menudocument->setObjectName("menudocument");
        menuhelp = new QMenu(menuBar);
        menuhelp->setObjectName("menuhelp");
        MainWindow->setMenuBar(menuBar);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName("dockWidget");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(dockWidget->sizePolicy().hasHeightForWidth());
        dockWidget->setSizePolicy(sizePolicy1);
        dockWidget->setMinimumSize(QSize(200, 540));
        dockWidget->setMaximumSize(QSize(280, 524287));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName("dockWidgetContents");
        sizePolicy1.setHeightForWidth(dockWidgetContents->sizePolicy().hasHeightForWidth());
        dockWidgetContents->setSizePolicy(sizePolicy1);
        dockWidgetContents->setStyleSheet(QString::fromUtf8("/* \345\267\245\345\205\267\347\256\261\346\214\211\351\222\256\346\240\267\345\274\217 */\n"
"QPushButton {\n"
"    background-color: #ffffff;\n"
"    border: 2px solid #e1dfdd;\n"
"    border-radius: 8px;\n"
"    padding: 12px 16px;\n"
"    font-size: 14px;\n"
"    font-weight: 500;\n"
"    color: #323130;\n"
"    text-align: left;\n"
"    min-height: 32px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #f3f2f1;\n"
"    border-color: #c8c6c4;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #edebe9;\n"
"    border-color: #a19f9d;\n"
"}\n"
"\n"
"QPushButton:focus {\n"
"    border-color: #0078d4;\n"
"    outline: none;\n"
"}\n"
"\n"
"/* \347\211\271\346\256\212\346\214\211\351\222\256\351\242\234\350\211\262 */\n"
"QPushButton#addRectangleButton {\n"
"    background-color: #0078d4;\n"
"    color: #ffffff;\n"
"    border-color: #0078d4;\n"
"}\n"
"\n"
"QPushButton#addRectangleButton:hover {\n"
"    background-color: #106ebe;\n"
"    border-color: #106ebe;\n"
"}\n"
"\n"
"QPushButton#de"
                        "leteButton {\n"
"    background-color: #d13438;\n"
"    color: #ffffff;\n"
"    border-color: #d13438;\n"
"}\n"
"\n"
"QPushButton#deleteButton:hover {\n"
"    background-color: #b4282c;\n"
"    border-color: #b4282c;\n"
"}"));
        verticalLayout_main = new QVBoxLayout(dockWidgetContents);
        verticalLayout_main->setSpacing(12);
        verticalLayout_main->setObjectName("verticalLayout_main");
        verticalLayout_main->setContentsMargins(16, 16, 16, 16);
        toolboxLabel = new QLabel(dockWidgetContents);
        toolboxLabel->setObjectName("toolboxLabel");
        toolboxLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    font-size: 16px;\n"
"    font-weight: 600;\n"
"    color: #323130;\n"
"    padding: 8px 0px;\n"
"    border: none;\n"
"    background: transparent;\n"
"}"));

        verticalLayout_main->addWidget(toolboxLabel);

        addRectangleButton = new QPushButton(dockWidgetContents);
        addRectangleButton->setObjectName("addRectangleButton");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(addRectangleButton->sizePolicy().hasHeightForWidth());
        addRectangleButton->setSizePolicy(sizePolicy2);

        verticalLayout_main->addWidget(addRectangleButton);

        line_1 = new QFrame(dockWidgetContents);
        line_1->setObjectName("line_1");
        line_1->setStyleSheet(QString::fromUtf8("QFrame {\n"
"    background-color: #e1dfdd;\n"
"    border: none;\n"
"}"));
        line_1->setFrameShape(QFrame::Shape::HLine);
        line_1->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_main->addWidget(line_1);

        editLabel = new QLabel(dockWidgetContents);
        editLabel->setObjectName("editLabel");
        editLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    font-size: 14px;\n"
"    font-weight: 600;\n"
"    color: #605e5c;\n"
"    padding: 4px 0px;\n"
"    border: none;\n"
"    background: transparent;\n"
"}"));

        verticalLayout_main->addWidget(editLabel);

        selectButton = new QPushButton(dockWidgetContents);
        selectButton->setObjectName("selectButton");
        sizePolicy2.setHeightForWidth(selectButton->sizePolicy().hasHeightForWidth());
        selectButton->setSizePolicy(sizePolicy2);

        verticalLayout_main->addWidget(selectButton);

        undoButton = new QPushButton(dockWidgetContents);
        undoButton->setObjectName("undoButton");
        sizePolicy2.setHeightForWidth(undoButton->sizePolicy().hasHeightForWidth());
        undoButton->setSizePolicy(sizePolicy2);

        verticalLayout_main->addWidget(undoButton);

        redoButton = new QPushButton(dockWidgetContents);
        redoButton->setObjectName("redoButton");
        sizePolicy2.setHeightForWidth(redoButton->sizePolicy().hasHeightForWidth());
        redoButton->setSizePolicy(sizePolicy2);

        verticalLayout_main->addWidget(redoButton);

        deleteButton = new QPushButton(dockWidgetContents);
        deleteButton->setObjectName("deleteButton");
        sizePolicy2.setHeightForWidth(deleteButton->sizePolicy().hasHeightForWidth());
        deleteButton->setSizePolicy(sizePolicy2);

        verticalLayout_main->addWidget(deleteButton);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_main->addItem(verticalSpacer);

        dockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWidget);
        dockWidget->raise();

        menuBar->addAction(menufile->menuAction());
        menuBar->addAction(menuedit->menuAction());
        menuBar->addAction(menusettings->menuAction());
        menuBar->addAction(menuview->menuAction());
        menuBar->addAction(menudocument->menuAction());
        menuBar->addAction(menuhelp->menuAction());
        menufile->addAction(actionnew);
        menufile->addAction(actionopen);
        menufile->addAction(actionopenRecent);
        menufile->addSeparator();
        menufile->addAction(actionsave);
        menufile->addAction(actionsaveOther);
        menufile->addSeparator();
        menufile->addAction(actionexit);
        menuedit->addAction(actionaddRect);
        menusettings->addAction(actionsetLeftDock);
        menusettings->addAction(actionhideLeftDock);
        menusettings->addAction(menu->menuAction());
        menu->addAction(actionmm);
        menu->addAction(actioncm);
        menu->addAction(actiondm);
        menuview->addAction(actionsetting_original);
        menuhelp->addAction(actionaboutUs);
        menuhelp->addAction(actionhelpDocument);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "ChipCraft Design Studio", nullptr));
        actionsetting_original->setText(QCoreApplication::translate("MainWindow", "\351\246\226\351\200\211\351\241\271...", nullptr));
        actionsetLeftDock->setText(QCoreApplication::translate("MainWindow", "\346\230\276\347\244\272\345\267\245\345\205\267\347\256\261", nullptr));
        actionhideLeftDock->setText(QCoreApplication::translate("MainWindow", "\351\232\220\350\227\217\345\267\245\345\205\267\347\256\261", nullptr));
        actionnew->setText(QCoreApplication::translate("MainWindow", "\346\226\260\345\273\272(&N)", nullptr));
#if QT_CONFIG(shortcut)
        actionnew->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionsave->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230(&S)", nullptr));
#if QT_CONFIG(shortcut)
        actionsave->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionopen->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200(&O)", nullptr));
#if QT_CONFIG(shortcut)
        actionopen->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionopenRecent->setText(QCoreApplication::translate("MainWindow", "\346\234\200\350\277\221\346\211\223\345\274\200\347\232\204\346\226\207\344\273\266", nullptr));
        actionexit->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272(&X)", nullptr));
#if QT_CONFIG(shortcut)
        actionexit->setShortcut(QCoreApplication::translate("MainWindow", "Alt+F4", nullptr));
#endif // QT_CONFIG(shortcut)
        actionsaveOther->setText(QCoreApplication::translate("MainWindow", "\345\217\246\345\255\230\344\270\272...", nullptr));
#if QT_CONFIG(shortcut)
        actionsaveOther->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionaddRect->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\350\212\257\347\211\207", nullptr));
#if QT_CONFIG(shortcut)
        actionaddRect->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+A", nullptr));
#endif // QT_CONFIG(shortcut)
        actionaboutUs->setText(QCoreApplication::translate("MainWindow", "\345\205\263\344\272\216 ChipCraft", nullptr));
        actionhelpDocument->setText(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251\346\226\207\346\241\243", nullptr));
#if QT_CONFIG(shortcut)
        actionhelpDocument->setShortcut(QCoreApplication::translate("MainWindow", "F1", nullptr));
#endif // QT_CONFIG(shortcut)
        actionmm->setText(QCoreApplication::translate("MainWindow", "\346\257\253\347\261\263", nullptr));
        actioncm->setText(QCoreApplication::translate("MainWindow", "\345\216\230\347\261\263", nullptr));
        actiondm->setText(QCoreApplication::translate("MainWindow", "\345\210\206\347\261\263", nullptr));
        menufile->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266(&F)", nullptr));
        menuedit->setTitle(QCoreApplication::translate("MainWindow", "\347\274\226\350\276\221(&E)", nullptr));
        menusettings->setTitle(QCoreApplication::translate("MainWindow", "\350\247\206\345\233\276(&V)", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\345\210\207\346\215\242\350\247\206\345\233\276\345\215\225\344\275\215", nullptr));
        menuview->setTitle(QCoreApplication::translate("MainWindow", "\350\256\276\347\275\256(&S)", nullptr));
        menudocument->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\346\241\243(&D)", nullptr));
        menuhelp->setTitle(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251(&H)", nullptr));
        dockWidget->setWindowTitle(QCoreApplication::translate("MainWindow", "\345\267\245\345\205\267\347\256\261", nullptr));
        toolboxLabel->setText(QCoreApplication::translate("MainWindow", "\350\256\276\350\256\241\345\267\245\345\205\267", nullptr));
        addRectangleButton->setText(QCoreApplication::translate("MainWindow", "+ \346\267\273\345\212\240\350\212\257\347\211\207", nullptr));
        editLabel->setText(QCoreApplication::translate("MainWindow", "\347\274\226\350\276\221\346\223\215\344\275\234", nullptr));
        selectButton->setText(QCoreApplication::translate("MainWindow", "\360\237\224\215 \351\200\211\346\213\251\345\267\245\345\205\267", nullptr));
        undoButton->setText(QCoreApplication::translate("MainWindow", "\342\206\266 \346\222\244\351\224\200", nullptr));
        redoButton->setText(QCoreApplication::translate("MainWindow", "\342\206\267 \351\207\215\345\201\232", nullptr));
        deleteButton->setText(QCoreApplication::translate("MainWindow", "\360\237\227\221 \345\210\240\351\231\244", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
