#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QUndoStack>

class CanvasScene;
class CanvasView;
class CellItem;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void exportFiles();

    void on_addRectangleButton_clicked();

    void on_selectButton_clicked();

    void on_undoButton_clicked();

    void on_redoButton_clicked();

    void on_deleteButton_clicked();

private:
    Ui::MainWindow *ui;
    CanvasScene *scene;
    CanvasView *view;
    QString currentFilePath;
    QUndoStack *undoStack; // 用于撤销和重做

    bool generateMacroFile(const QString& filePath, const QList<CellItem*>& cellItems);
    bool generateDesignFile(const QString& filePath, const QList<CellItem*>& cellItems);
};




#endif // MAINWINDOW_H
