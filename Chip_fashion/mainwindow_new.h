#ifndef MAINWINDOW_NEW_H
#define MAINWINDOW_NEW_H

#include <QMainWindow>
#include <QUndoStack>
#include <QString>
#include <QStringList>
#include <QMap>

class QMenu;
class QAction;

// 前向声明，避免头文件直接包含大量 Qt 组件头，减少编译依赖
class QTreeWidget;
class QTreeWidgetItem;
class QGraphicsItem;
class QCloseEvent;

class CanvasScene;
class CanvasView;
class ChipManager;
class FileManager;
class CellItem; // 前向声明，供 computeNetKey 使用

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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

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
    void on_netTreeWidget_itemClicked(QTreeWidgetItem *item, int column); // 处理树点击

    // 退出（与其它 QAction 的触发命名保持一致，使用 Qt 自动连接）
    void on_actionexit_triggered();

private:
    // 初始化 / UI
    void setupUI();
    void loadStyleSheet();
    void setDefaultStyle();
    void setupManagers();
    void connectSignals();

    // 业务逻辑辅助
    void rebuildNetTree();            // 重建网络树
    void selectAndCenterItem(QGraphicsItem* item);
    void updateWindowTitle(const QString& fileName = QString());
    void fitViewToContent();          // 自动调整视图以适应内容

    // 事件
    void closeEvent(QCloseEvent *event) override; // 退出确认
    // （保留扩展点：设计浏览器相关函数已去除未实现声明，后续需要再添加实现）

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

    // 最近文件
    QStringList m_recentFiles;              // 最近文件路径
    QMenu* m_recentMenu = nullptr;          // 菜单容器
    static const int MAX_RECENT_FILES = 10; // 最大数量
    void loadRecentFiles();                 // 从 QSettings 读取
    void saveRecentFiles();                 // 保存到 QSettings
    void updateRecentFilesMenu();           // 重建菜单项
    void addToRecentFiles(const QString& path); // 添加并更新
    void openRecentFileInternal(const QString& path); // 打开指定文件
    void clearRecentFiles();                // 清空

    // 动态线网模式
    bool m_dynamicNetMode = false;          // 是否强制使用动态连通分量模式
    void applyNetModeAndRefresh();          // 根据模式刷新树

    QAction* m_actionDynamicMode = nullptr; // 切换动作指针
    // 动态模式下的自定义名称映射 key=组件指针排序生成的键, value=自定义名称
    QMap<QString, QString> m_dynamicNetNameMap;
    QString computeNetKey(const QList<CellItem*>& comp) const; // 生成动态线网键
    void saveDynamicNetNames();
    void loadDynamicNetNames();
    void renameDynamicNet(const QString& key, const QString& newName);

private slots:
    void onRecentFileTriggered();           // 动态动作槽
    void onClearRecentTriggered();          // 清空动作槽
    void toggleDynamicNetMode();            // 切换动态模式槽
    void showNetTreeContextMenu(const QPoint& pos); // 右键菜单
    void renameSelectedNet();               // 执行重命名

    // （网络树直接通过 objectName 查找，不在此保存指针）
};

#endif // MAINWINDOW_NEW_H
