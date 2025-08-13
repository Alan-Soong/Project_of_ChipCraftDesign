#ifndef MAINWINDOW_NEW_H
#define MAINWINDOW_NEW_H

#include <QMainWindow>
#include <QUndoStack>

class CanvasScene;
class CanvasView;
class ChipManager;
class FileManager;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * 主窗口类 - 重构后的版本，职责更加清晰
 * 主要负责：
 * 1. 界面管理和用户交互
 * 2. 菜单和工具栏操作
 * 3. 各个管理器的协调
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 文件操作
    void newFile();
    void newProject();  // 新建项目方法
    void openFile();
    void saveFile();
    void saveFileAs();
    void exportFiles();

    // 编辑操作
    void on_addRectangleButton_clicked();
    void on_selectButton_clicked();
    void on_undoButton_clicked();
    void on_redoButton_clicked();
    void on_deleteButton_clicked();

    // 事件处理
    void onFileOperationCompleted(const QString& operation, bool success);
    void onComponentSettingsSaved();

    void on_actionmm_triggered();

    void on_actioncm_triggered();

    void on_actiondm_triggered();

private:
    void setupUI();
    void loadStyleSheet();
    void setDefaultStyle();
    void setupManagers();
    void connectSignals();
    void updateWindowTitle(const QString& fileName = QString());
    void fitViewToContent(); // 新增：自动调整视图以适应内容

    // UI组件
    Ui::MainWindow *ui;
    CanvasScene *m_scene;
    CanvasView *m_view;
    QUndoStack *m_undoStack;

    // 管理器
    ChipManager *m_chipManager;
    FileManager *m_fileManager;

    // 状态
    QString m_currentFilePath;
    bool m_isModified;
};

#endif // MAINWINDOW_NEW_H
