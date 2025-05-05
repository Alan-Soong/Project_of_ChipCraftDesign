#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QMainWindow>
#include "canvasview.h"
#include "canvasscene.h"

#include <QString>
#include <QMessageBox>
#include <QDir>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_addRectangleButton_clicked();
    void on_selectButton_clicked();
    void on_undoButton_clicked();
    void on_redoButton_clicked();
    void on_deleteButton_clicked();

    // void setupConnections();

private slots:
    void on_actionhideLeftDock_triggered();

    void on_actionsetLeftDock_triggered();

    void on_actionsave_triggered();

private:
    Ui::MainWindow *ui;
    CanvasView *m_canvasView;
    CanvasScene *m_canvasScene;
};
#endif // MAINWINDOW_H

