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
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *addRectangleButton;
    QPushButton *selectButton;
    QPushButton *undoButton;
    QPushButton *redoButton;
    QPushButton *deleteButton;
    QGraphicsView *canvasView;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1000, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName("horizontalLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        addRectangleButton = new QPushButton(centralwidget);
        addRectangleButton->setObjectName("addRectangleButton");

        verticalLayout->addWidget(addRectangleButton);

        selectButton = new QPushButton(centralwidget);
        selectButton->setObjectName("selectButton");

        verticalLayout->addWidget(selectButton);

        undoButton = new QPushButton(centralwidget);
        undoButton->setObjectName("undoButton");

        verticalLayout->addWidget(undoButton);

        redoButton = new QPushButton(centralwidget);
        redoButton->setObjectName("redoButton");

        verticalLayout->addWidget(redoButton);

        deleteButton = new QPushButton(centralwidget);
        deleteButton->setObjectName("deleteButton");

        verticalLayout->addWidget(deleteButton);


        horizontalLayout->addLayout(verticalLayout);

        canvasView = new QGraphicsView(centralwidget);
        canvasView->setObjectName("canvasView");
        canvasView->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        canvasView->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

        horizontalLayout->addWidget(canvasView);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Chip Layout Editor", nullptr));
        addRectangleButton->setText(QCoreApplication::translate("MainWindow", "\346\267\273\345\212\240\347\237\251\345\275\242", nullptr));
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
