#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QMainWindow>
#include "canvasview.h"
#include "canvasscene.h"
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

private:
    Ui::MainWindow *ui;
    CanvasView *m_canvasView;
    CanvasScene *m_canvasScene;
};
#endif // MAINWINDOW_H

