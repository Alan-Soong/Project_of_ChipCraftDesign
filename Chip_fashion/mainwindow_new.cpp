#include "mainwindow_new.h"
#include "ui_mainwindow.h"
#include "canvasscene.h"
#include "canvasview.h"
#include "connectionline.h"
#include "chipmanager.h"
#include "filemanager.h"
#include "pineditordialog.h"
#include "command.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QTimer>
#include <QStatusBar>
#include <QTimer>
#include <QTreeWidget>
#include <QSet>
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_scene(nullptr)
    , m_view(nullptr)
    , m_undoStack(new QUndoStack(this))
    , m_chipManager(nullptr)
    , m_fileManager(nullptr)
    , m_isModified(false)
{
    ui->setupUi(this);
    // 设置应用程序图标
    setWindowIcon(QIcon(":/app_icon.svg"));

    loadStyleSheet();
    setupUI();
    setupManagers();
    connectSignals();

    // 树点击（使用对象名称确保存在）
    if (auto tree = findChild<QTreeWidget*>("netTreeWidget")) {
        connect(tree, &QTreeWidget::itemClicked,
                this, &MainWindow::on_netTreeWidget_itemClicked);
    }

    // 初始构建网络树
    rebuildNetTree();
    updateWindowTitle();
    statusBar()->showMessage("就绪", 2000);

    loadRecentFiles();
    updateRecentFilesMenu();
    // 读取动态模式偏好
    {
        QSettings settings("ChipDesignCompany", "ChipDesignTool");
        m_dynamicNetMode = settings.value("dynamicNetMode", false).toBool();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 设置场景和视图，创建无限大画布
    m_scene = new CanvasScene(this);
    // 不需要在这里设置场景矩形，CanvasScene构造函数已经设置了无限大小
    
    // 设置撤销栈
    m_scene->setUndoStack(m_undoStack);

    m_view = new CanvasView(m_scene);
    m_view->setParent(this);
    setCentralWidget(m_view);

    // 初始视图居中显示，不限制在特定矩形内
    QTimer::singleShot(100, [this]() {
        if (m_view && m_scene) {
            // 将视图中心定位到场景的原点(0,0)
            m_view->centerOn(0, 0);
        }
    });

    // 现在场景已创建，连接线网信号（防止 connectSignals 时 m_scene 为空）
    connect(m_scene, &CanvasScene::netTopologyChanged, this, &MainWindow::rebuildNetTree);
}

void MainWindow::loadStyleSheet()
{
    // 尝试加载样式表文件
    QFile styleFile(":/styles.qss");
    if (!styleFile.exists()) {
        // 如果资源文件不存在，尝试从当前目录加载
        styleFile.setFileName("styles.qss");
    }

    if (styleFile.open(QFile::ReadOnly)) {
        QTextStream stream(&styleFile);
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
        qDebug() << "样式表加载成功";
    } else {
        qDebug() << "样式表加载失败:" << styleFile.errorString();
        // 如果无法加载文件，使用内嵌的基本样式
        setDefaultStyle();
    }
}

// 构建网络树：顶层为“线网”，其下为每个线网；线网下分“芯片”“连线”两个子节点，再列出具体对象
void MainWindow::rebuildNetTree()
{
    auto tree = findChild<QTreeWidget*>("netTreeWidget");
    if (!tree || !m_scene) return;
    tree->clear();
    tree->setHeaderLabel("对象");
    QList<CellItem*> cells = m_scene->getAllCellItems();
    QList<ConnectionLine*> lines = m_scene->getConnectionLines();
    if (cells.isEmpty()) return; // 没有元件不显示

    // 如果存在命名线网且未开启强制动态模式，则使用命名线网结构
    if (!m_dynamicNetMode && m_scene->hasNamedNets()) {
        auto namedNets = m_scene->getNamedNets();
        for (auto it = namedNets.begin(); it != namedNets.end(); ++it) {
            const QString &netName = it.key();
            const auto &netInfo = it.value();
            QTreeWidgetItem *netRoot = new QTreeWidgetItem(tree, QStringList(netName));
            netRoot->setData(0, Qt::UserRole, QVariant("net"));
            // 直接列出引脚（与文件格式类似：Pin C1/P1）
            for (const auto &pr : netInfo.pins) {
                CellItem* c = pr.first;
                if (!c) continue;
                QString instName = c->getInstanceName().isEmpty() ? QStringLiteral("<未命名芯片>") : c->getInstanceName();
                QString pinId = pr.second;
                QString label = QStringLiteral("Pin %1/%2").arg(instName, pinId);
                QTreeWidgetItem *ci = new QTreeWidgetItem(netRoot, QStringList(label));
                ci->setData(0, Qt::UserRole, QVariant("cell_pin"));
                ci->setData(0, Qt::UserRole + 1, QVariant::fromValue(reinterpret_cast<qulonglong>(c)));
            }
            // 如果需要也可以加一个子节点展示连线，但此处按文件风格省略分组
            for (ConnectionLine* l : netInfo.lines) {
                if (!l) continue;
                QString label = QStringLiteral("Line_%1").arg(reinterpret_cast<qulonglong>(l), 0, 16);
                QTreeWidgetItem *li = new QTreeWidgetItem(netRoot, QStringList(label));
                li->setData(0, Qt::UserRole, QVariant("line"));
                li->setData(0, Qt::UserRole + 1, QVariant::fromValue(reinterpret_cast<qulonglong>(l)));
            }
        }
        tree->expandAll();
        return; // 已使用命名线网，退出
    }

    // 建图：节点为芯片，边为连线
    QMap<CellItem*, QList<CellItem*>> adj;
    for (CellItem* c : cells) adj[c];
    for (ConnectionLine* l : lines) {
        if (!l) continue;
        CellItem* a = l->getStartItem();
        CellItem* b = l->getEndItem();
        if (a && b && a != b) {
            adj[a].append(b);
            adj[b].append(a);
        }
    }

    // DFS 求连通分量
    QList<QList<CellItem*>> components;
    QSet<CellItem*> visited;
    for (CellItem* c : cells) {
        if (visited.contains(c)) continue;
        QList<CellItem*> comp;
        QList<CellItem*> stack{c};
        visited.insert(c);
        while (!stack.isEmpty()) {
            CellItem* cur = stack.takeLast();
            comp.append(cur);
            for (CellItem* nxt : adj[cur]) {
                if (!visited.contains(nxt)) {
                    visited.insert(nxt);
                    stack.append(nxt);
                }
            }
        }
        components.append(comp);
    }

    int netIndex = 1;
    for (const QList<CellItem*>& comp : components) {
        if (comp.isEmpty()) continue;
        // 生成该连通分量的稳定键
        QList<qulonglong> ptrs; for (CellItem* c : comp) ptrs.append(reinterpret_cast<qulonglong>(c)); std::sort(ptrs.begin(), ptrs.end());
        QStringList keyParts; for (auto v : ptrs) keyParts << QString::number(v, 16); QString key = keyParts.join("_");
        QString netName = m_dynamicNetNameMap.value(key, QString("Net%1").arg(netIndex));
        netIndex++;
        QTreeWidgetItem *netRoot = new QTreeWidgetItem(tree, QStringList(netName));
        netRoot->setData(0, Qt::UserRole, QVariant("net"));
        netRoot->setData(0, Qt::UserRole + 2, key); // 存储键用于重命名

        QTreeWidgetItem *cellsNode = new QTreeWidgetItem(netRoot, QStringList(QStringLiteral("芯片")));
        cellsNode->setData(0, Qt::UserRole, QVariant("cells_group"));
        for (CellItem* c : comp) {
            QString name = c->getInstanceName().isEmpty() ? QStringLiteral("<未命名芯片>") : c->getInstanceName();
            QTreeWidgetItem *ci = new QTreeWidgetItem(cellsNode, QStringList(name));
            ci->setData(0, Qt::UserRole, QVariant("cell"));
            ci->setData(0, Qt::UserRole + 1, QVariant::fromValue(reinterpret_cast<qulonglong>(c)));
        }

        QTreeWidgetItem *linesNode = new QTreeWidgetItem(netRoot, QStringList(QStringLiteral("连线")));
        linesNode->setData(0, Qt::UserRole, QVariant("lines_group"));
        for (ConnectionLine* l : lines) {
            if (!l) continue;
            CellItem* a = l->getStartItem();
            CellItem* b = l->getEndItem();
            if (comp.contains(a) && comp.contains(b)) {
                QString label = QStringLiteral("Line_%1").arg(reinterpret_cast<qulonglong>(l), 0, 16);
                QTreeWidgetItem *li = new QTreeWidgetItem(linesNode, QStringList(label));
                li->setData(0, Qt::UserRole, QVariant("line"));
                li->setData(0, Qt::UserRole + 1, QVariant::fromValue(reinterpret_cast<qulonglong>(l)));
            }
        }
    }
    tree->expandAll();
}

void MainWindow::selectAndCenterItem(QGraphicsItem* item)
{
    if (!item || !m_scene || !m_view) return;
    m_scene->clearSelection();
    item->setSelected(true);
    m_view->centerOn(item);
}

void MainWindow::on_netTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    if (!item) return;
    QString type = item->data(0, Qt::UserRole).toString();
    if (type == "cell" || type == "line" || type == "cell_pin") {
        qulonglong ptrVal = item->data(0, Qt::UserRole + 1).toULongLong();
        if (ptrVal) {
            if (type == "cell") {
                CellItem* c = reinterpret_cast<CellItem*>(ptrVal);
                selectAndCenterItem(c);
            } else if (type == "cell_pin") {
                CellItem* c = reinterpret_cast<CellItem*>(ptrVal);
                selectAndCenterItem(c);
            } else if (type == "line") {
                ConnectionLine* l = reinterpret_cast<ConnectionLine*>(ptrVal);
                selectAndCenterItem(l);
            }
        }
    }
}

void MainWindow::setDefaultStyle()
{
    // 设置基本的现代化样式
    QString basicStyle = R"(
        QMainWindow {
            background-color: #f3f3f3;
        }
        QPushButton {
            background-color: #ffffff;
            border: 2px solid #e1dfdd;
            border-radius: 6px;
            padding: 8px 16px;
            font-size: 14px;
            color: #323130;
        }
        QPushButton:hover {
            background-color: #f3f2f1;
            border-color: #c8c6c4;
        }
        QPushButton:pressed {
            background-color: #edebe9;
        }
    )";
    qApp->setStyleSheet(basicStyle);
}

void MainWindow::setupManagers()
{
    // 初始化管理器
    m_chipManager = new ChipManager(this);
    m_fileManager = new FileManager(this);
}

void MainWindow::connectSignals()
{
    // 连接菜单信号
    connect(ui->actionnew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionopen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionsave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionsaveOther, &QAction::triggered, this, &MainWindow::saveFileAs);
    // 退出采用 Qt 自动连接，不在此处手动连接：on_actionexit_triggered()

    // 连接文件管理器信号
    connect(m_fileManager, &FileManager::fileOperationCompleted,
            this, &MainWindow::onFileOperationCompleted);

    // 线网拓扑变化时重建树
    if (m_scene) {
        connect(m_scene, &CanvasScene::netTopologyChanged, this, &MainWindow::rebuildNetTree);
    }

    // 最近文件菜单动作（actionopenRecent 作为占位，将其替换成下拉子菜单）
    if (ui && ui->menufile) {
        QAction* recentPlaceholder = ui->actionopenRecent; // 来自 UI 的动作
        if (recentPlaceholder) {
            if (!m_recentMenu) {
                m_recentMenu = new QMenu(tr("最近打开的文件"), this);
                recentPlaceholder->setMenu(m_recentMenu); // 直接作为可展开项
            }
            updateRecentFilesMenu();
        }
    }

    // 动态模式菜单项（添加到“设置”菜单或“视图”菜单）
    QMenu* targetMenu = nullptr;
    if (ui && ui->menusettings) targetMenu = ui->menusettings; else if (ui && ui->menuview) targetMenu = ui->menuview;
    if (targetMenu && !m_actionDynamicMode) {
        targetMenu->addSeparator();
        m_actionDynamicMode = new QAction(tr("动态线网模式"), this);
        m_actionDynamicMode->setCheckable(true);
        m_actionDynamicMode->setChecked(m_dynamicNetMode);
        targetMenu->addAction(m_actionDynamicMode);
        connect(m_actionDynamicMode, &QAction::triggered, this, &MainWindow::toggleDynamicNetMode);
    }
    // 线网树右键菜单
    if (auto tree = findChild<QTreeWidget*>("netTreeWidget")) {
        tree->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tree, &QTreeWidget::customContextMenuRequested, this, &MainWindow::showNetTreeContextMenu);
    }
}

void MainWindow::applyNetModeAndRefresh()
{
    if (m_dynamicNetMode && m_scene && m_scene->hasNamedNets()) {
        m_scene->resetNamedNets(); // 清空以便重新按连通分量生成
    }
    rebuildNetTree();
    if (m_actionDynamicMode) m_actionDynamicMode->setChecked(m_dynamicNetMode);
    QSettings settings("ChipDesignCompany", "ChipDesignTool");
    settings.setValue("dynamicNetMode", m_dynamicNetMode);
}

void MainWindow::toggleDynamicNetMode() { m_dynamicNetMode = !m_dynamicNetMode; applyNetModeAndRefresh(); }

QString MainWindow::computeNetKey(const QList<CellItem*>& comp) const {
    QList<qulonglong> ptrs; for (CellItem* c : comp) ptrs.append(reinterpret_cast<qulonglong>(c)); std::sort(ptrs.begin(), ptrs.end());
    QStringList parts; for (auto v : ptrs) parts << QString::number(v, 16); return parts.join("_"); }

void MainWindow::saveDynamicNetNames() {
    QSettings s("ChipDesignCompany", "ChipDesignTool"); s.beginGroup("DynamicNetNames"); s.remove("");
    for (auto it = m_dynamicNetNameMap.begin(); it != m_dynamicNetNameMap.end(); ++it) s.setValue(it.key(), it.value()); s.endGroup(); }

void MainWindow::loadDynamicNetNames() {
    m_dynamicNetNameMap.clear(); QSettings s("ChipDesignCompany", "ChipDesignTool"); s.beginGroup("DynamicNetNames");
    for (const QString &k : s.childKeys()) m_dynamicNetNameMap[k] = s.value(k).toString(); s.endGroup(); }

void MainWindow::renameDynamicNet(const QString& key, const QString& newName) {
    if (key.isEmpty()) return; QString n = newName.trimmed(); if (n.isEmpty()) return;
    m_dynamicNetNameMap[key] = n; saveDynamicNetNames(); rebuildNetTree(); }

void MainWindow::showNetTreeContextMenu(const QPoint& pos) {
    auto tree = qobject_cast<QTreeWidget*>(sender()); if (!tree) return; auto item = tree->itemAt(pos); if (!item) return;
    if (item->data(0, Qt::UserRole).toString() != "net") return; QMenu menu(this);
    QAction *renameAct = menu.addAction(tr("重命名线网")); QAction *act = menu.exec(tree->viewport()->mapToGlobal(pos));
    if (act == renameAct) { tree->setCurrentItem(item); renameSelectedNet(); } }

void MainWindow::renameSelectedNet() {
    if (!m_dynamicNetMode) { QMessageBox::information(this, tr("提示"), tr("请先启用动态线网模式。")); return; }
    auto tree = findChild<QTreeWidget*>("netTreeWidget"); if (!tree) return; auto item = tree->currentItem(); if (!item) return;
    if (item->data(0, Qt::UserRole).toString() != "net") return; QString key = item->data(0, Qt::UserRole + 2).toString();
    QString oldName = item->text(0); bool ok=false; QString newName = QInputDialog::getText(this, tr("重命名线网"), tr("新名称:"), QLineEdit::Normal, oldName, &ok);
    if (!ok) return; renameDynamicNet(key, newName); }

void MainWindow::newFile()
{
    qDebug() << "========== 开始新建文件 ==========";

    // 检查是否已经打开了文件
    if (!m_currentFilePath.isEmpty()) {
        qDebug() << "当前已有文件:" << m_currentFilePath;

        // 创建确认对话框
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("新建文件确认");
        msgBox.setText("当前已经打开了一个设计文件。");
        msgBox.setInformativeText("请选择您希望如何处理当前工作：\n\n"
                                  "• 直接新建：清空当前内容，创建新的空白文件\n"
                                  "• 保存后新建：先保存当前文件，然后创建新文件\n"
                                  "• 取消：保持当前状态，不创建新文件");
        msgBox.setIcon(QMessageBox::Question);

        // 添加自定义按钮
        QPushButton *directNewButton = msgBox.addButton("直接新建", QMessageBox::ActionRole);
        QPushButton *saveAndNewButton = msgBox.addButton("保存后新建", QMessageBox::ActionRole);
        QPushButton *cancelButton = msgBox.addButton("取消", QMessageBox::RejectRole);

        msgBox.setDefaultButton(saveAndNewButton);

        msgBox.exec();

        if (msgBox.clickedButton() == cancelButton) {
            qDebug() << "用户取消新建文件";
            return; // 用户选择取消
        } else if (msgBox.clickedButton() == saveAndNewButton) {
            // 用户选择保存后新建，先保存当前文件
            qDebug() << "用户选择保存后新建文件";
            saveFile(); // 保存当前文件
        }
        // 如果选择直接新建，继续执行新建文件的逻辑
    }

    // 清理当前项目，创建新的空白文件
    m_scene->clear();
    m_currentFilePath.clear();
    m_chipManager->resetChipCounter();
    m_isModified = false;
    rebuildNetTree();

    updateWindowTitle("新文件");

    // 不自动创建芯片，让用户手动添加或通过文件加载
    // 这样与打开文件的逻辑保持一致
    
    qDebug() << "========== 新建文件完成 ==========";
}

void MainWindow::newProject()
{
    // 清理当前项目，准备新建项目或打开文件
    m_scene->clear();
    m_currentFilePath.clear();
    m_chipManager->resetChipCounter();
    m_isModified = false;
    rebuildNetTree();

    updateWindowTitle("新项目");

    // 注意：newProject 不自动创建芯片，这样打开文件时场景是干净的
}

void MainWindow::openFile()
{
    qDebug() << "========== 开始打开文件 ==========";

    // 检查是否已经打开了文件
    if (!m_currentFilePath.isEmpty()) {
        qDebug() << "当前已有文件:" << m_currentFilePath;

        // 创建确认对话框
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("打开文件确认");
        msgBox.setText("当前已经打开了一个设计文件。");
        msgBox.setInformativeText("请选择您希望如何处理当前工作：\n\n"
                                  "• 新建项目：清空当前内容，创建空白项目后再打开新文件\n"
                                  "• 重新打开：直接用新文件替换当前内容\n"
                                  "• 取消：保持当前状态，不打开新文件");
        msgBox.setIcon(QMessageBox::Question);

        // 添加自定义按钮
        QPushButton *newButton = msgBox.addButton("新建项目", QMessageBox::ActionRole);
        QPushButton *reopenButton = msgBox.addButton("重新打开", QMessageBox::ActionRole);
        QPushButton *cancelButton = msgBox.addButton("取消", QMessageBox::RejectRole);

        msgBox.setDefaultButton(reopenButton);

        msgBox.exec();

        if (msgBox.clickedButton() == cancelButton) {
            qDebug() << "用户取消打开文件";
            return; // 用户选择取消
        } else if (msgBox.clickedButton() == newButton) {
            // 用户选择新建项目，先清理当前场景
            qDebug() << "用户选择新建项目，先清理场景";
            newProject();
        }
        // 如果选择重新打开，继续执行打开文件的逻辑
    }

    QString filePath = QFileDialog::getOpenFileName(this, "打开文件", "", "设计文件 (*.place *.txt);;Place文件 (*.place);;文本文件 (*.txt);;所有文件 (*)");
    if (filePath.isEmpty()) {
        qDebug() << "用户未选择文件";
        return;
    }

    qDebug() << "选择的文件:" << filePath;
    qDebug() << "当前画布单位:" << m_scene->get_unit();
    qDebug() << "打开前场景中芯片数量:" << m_scene->getAllCellItems().size();

    if (m_fileManager->openDesignFile(filePath, m_scene)) {
        qDebug() << "文件加载成功！";
        m_currentFilePath = filePath;
    addToRecentFiles(filePath);
        m_isModified = false;
        updateWindowTitle(QFileInfo(filePath).fileName());

        qDebug() << "打开后场景中芯片数量:" << m_scene->getAllCellItems().size();
        qDebug() << "场景矩形:" << m_scene->sceneRect();

        // 使用QTimer延迟调整视图，确保所有芯片都已渲染完成
        QTimer::singleShot(100, this, &MainWindow::fitViewToContent);
    rebuildNetTree();
    } else {
        qDebug() << "文件加载失败:" << m_fileManager->getLastError();
        QMessageBox::warning(this, "错误", "无法打开文件：" + m_fileManager->getLastError());
    }

    qDebug() << "========== 打开文件完成 ==========";
}

// ================= 最近文件功能实现 =================
void MainWindow::loadRecentFiles()
{
    QSettings settings("ChipDesignCompany", "ChipDesignTool");
    m_recentFiles = settings.value("recentFiles").toStringList();
}

void MainWindow::saveRecentFiles()
{
    QSettings settings("ChipDesignCompany", "ChipDesignTool");
    settings.setValue("recentFiles", m_recentFiles);
}

void MainWindow::addToRecentFiles(const QString& path)
{
    if (path.isEmpty()) return;
    m_recentFiles.removeAll(path); // 去重
    m_recentFiles.prepend(path);
    while (m_recentFiles.size() > MAX_RECENT_FILES) m_recentFiles.removeLast();
    saveRecentFiles();
    updateRecentFilesMenu();
}

void MainWindow::updateRecentFilesMenu()
{
    if (!m_recentMenu) return;
    m_recentMenu->clear();
    if (m_recentFiles.isEmpty()) {
        QAction* emptyAct = m_recentMenu->addAction(tr("(无)"));
        emptyAct->setEnabled(false);
    } else {
        int index = 1;
        for (const QString& path : m_recentFiles) {
            QString text = QString::number(index) + ". " + QFileInfo(path).fileName();
            QAction* act = m_recentMenu->addAction(text);
            act->setData(path);
            connect(act, &QAction::triggered, this, &MainWindow::onRecentFileTriggered);
            ++index;
        }
        m_recentMenu->addSeparator();
        QAction* clearAct = m_recentMenu->addAction(tr("清空列表"));
        connect(clearAct, &QAction::triggered, this, &MainWindow::onClearRecentTriggered);
    }
}

void MainWindow::openRecentFileInternal(const QString& path)
{
    if (path.isEmpty()) return;
    QFileInfo fi(path);
    if (!fi.exists()) {
        QMessageBox::warning(this, tr("文件不存在"), tr("文件已被移动或删除:\n%1").arg(path));
        m_recentFiles.removeAll(path);
        saveRecentFiles();
        updateRecentFilesMenu();
        return;
    }
    // 模拟重新打开逻辑：清理（保留 undo 栈行为与 openFile 一致）
    if (!m_currentFilePath.isEmpty()) {
        newProject();
    }
    if (m_fileManager->openDesignFile(path, m_scene)) {
        m_currentFilePath = path;
        m_isModified = false;
        addToRecentFiles(path); // 放到最前
        updateWindowTitle(QFileInfo(path).fileName());
        QTimer::singleShot(100, this, &MainWindow::fitViewToContent);
        rebuildNetTree();
    } else {
        QMessageBox::warning(this, tr("打开失败"), m_fileManager->getLastError());
    }
}

void MainWindow::clearRecentFiles()
{
    m_recentFiles.clear();
    saveRecentFiles();
    updateRecentFilesMenu();
}

void MainWindow::onRecentFileTriggered()
{
    QAction* act = qobject_cast<QAction*>(sender());
    if (!act) return;
    QString path = act->data().toString();
    openRecentFileInternal(path);
}

void MainWindow::onClearRecentTriggered()
{
    clearRecentFiles();
}

void MainWindow::saveFile()
{
    if (m_currentFilePath.isEmpty()) {
        saveFileAs();
        return;
    }

    QList<CellItem*> cellItems = m_scene->getAllCellItems();
    if (m_fileManager->saveDesignFile(m_currentFilePath, cellItems, m_scene)) {
        m_isModified = false;
        updateWindowTitle(QFileInfo(m_currentFilePath).fileName());
    } else {
        QMessageBox::warning(this, "错误", "无法保存文件：" + m_fileManager->getLastError());
    }
}

void MainWindow::saveFileAs()
{
    QString filePath = QFileDialog::getSaveFileName(this, "另存为", "", "设计文件 (*.place *.txt);;Place文件 (*.place);;文本文件 (*.txt);;所有文件 (*)");
    if (filePath.isEmpty()) return;

    QList<CellItem*> cellItems = m_scene->getAllCellItems();
    if (m_fileManager->saveDesignFile(filePath, cellItems, m_scene)) {
        m_currentFilePath = filePath;
        m_isModified = false;
        updateWindowTitle(QFileInfo(filePath).fileName());
    } else {
        QMessageBox::warning(this, "错误", "无法保存文件：" + m_fileManager->getLastError());
    }
}

void MainWindow::exportFiles()
{
    QString filePath = QFileDialog::getSaveFileName(this, "导出宏文件", "", "宏文件 (*.macro);;Place文件 (*.place);;文本文件 (*.txt);;所有文件 (*)");
    if (filePath.isEmpty()) return;

    QList<CellItem*> cellItems = m_scene->getAllCellItems();
    if (m_fileManager->exportMacroFile(filePath, cellItems)) {
        QMessageBox::information(this, "成功", "宏文件导出成功");
    } else {
        QMessageBox::warning(this, "错误", "无法导出宏文件：" + m_fileManager->getLastError());
    }
}

void MainWindow::on_actionexit_triggered()
{
    // 与窗口 X 按钮统一：走 close()，由 closeEvent 处理确认
    close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    auto ret = QMessageBox::question(
        this,
        "退出确认",
        "确定要退出应用程序吗？",
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if (ret == QMessageBox::Ok) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::on_addRectangleButton_clicked()
{
    // 获取视图中心在场景坐标系中的位置
    QPointF viewCenter = m_view->rect().center();
    QPointF scenePos = m_view->mapToScene(viewCenter.toPoint());

    qDebug() << "视图中心:" << viewCenter << "对应场景坐标:" << scenePos;

    // 使用命令模式支持撤销/重做，传入ChipManager而不是计数器
    m_undoStack->push(new AddRectangleCommand(m_scene, scenePos, m_chipManager, m_undoStack));

    m_isModified = true;
    updateWindowTitle();
    rebuildNetTree();

    qDebug() << "添加芯片到视图中心位置" << scenePos;
}

void MainWindow::on_selectButton_clicked()
{
    // 选择第一个未选中的芯片
    QList<CellItem*> cellItems = m_scene->getAllCellItems();
    for (CellItem* cellItem : cellItems) {
        if (!cellItem->isSelected()) {
            m_scene->clearSelection();
            cellItem->setSelected(true);
            qDebug() << "选中芯片" << cellItem->getInstanceName() << "位置:" << cellItem->pos();
            return;
        }
    }
    QMessageBox::information(this, "提示", "没有可选择的芯片");
}

void MainWindow::on_undoButton_clicked()
{
    if (m_undoStack->canUndo()) {
        m_undoStack->undo();
        m_isModified = true;
        updateWindowTitle();
    }
}

void MainWindow::on_redoButton_clicked()
{
    if (m_undoStack->canRedo()) {
        m_undoStack->redo();
        m_isModified = true;
        updateWindowTitle();
    }
}

void MainWindow::on_deleteButton_clicked()
{
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选择要删除的项目");
        return;
    }

    int cellCount = 0;
    int lineCount = 0;

    // 统计选中的项目类型
    for (QGraphicsItem* item : selectedItems) {
        if (dynamic_cast<CellItem*>(item)) {
            cellCount++;
        } else if (dynamic_cast<ConnectionLine*>(item)) {
            lineCount++;
        }
    }

    // 显示确认对话框
    QString message;
    if (cellCount > 0 && lineCount > 0) {
        message = QString("确定要删除 %1 个芯片和 %2 条连线吗？").arg(cellCount).arg(lineCount);
    } else if (cellCount > 0) {
        message = QString("确定要删除 %1 个芯片吗？").arg(cellCount);
    } else if (lineCount > 0) {
        message = QString("确定要删除 %1 条连线吗？").arg(lineCount);
    } else {
        QMessageBox::information(this, "提示", "没有可删除的项目");
        return;
    }

    int ret = QMessageBox::question(this, "确认删除", message,
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return;
    }

    // 执行删除操作
    for (QGraphicsItem* item : selectedItems) {
        if (CellItem* cellItem = dynamic_cast<CellItem*>(item)) {
            // 删除芯片（使用撤销命令）
            m_undoStack->push(new DeleteRectangleCommand(m_scene, cellItem, m_undoStack));
        } else if (ConnectionLine* line = dynamic_cast<ConnectionLine*>(item)) {
            // 删除连线（直接删除，也可以考虑添加撤销命令）
            m_scene->removeConnectionLine(line);
        }
    }

    m_isModified = true;
    updateWindowTitle();
    rebuildNetTree();
}

void MainWindow::onFileOperationCompleted(const QString& operation, bool success)
{
    if (success) {
        qDebug() << "文件操作完成:" << operation;
    } else {
        QMessageBox::warning(this, "错误", "文件操作失败: " + operation);
    }
}

void MainWindow::onComponentSettingsSaved()
{
    m_isModified = true;
    updateWindowTitle();
}

void MainWindow::updateWindowTitle(const QString& fileName)
{
    QString title = "芯片设计工具";

    if (!fileName.isEmpty()) {
        title += " - " + fileName;
    } else if (!m_currentFilePath.isEmpty()) {
        title += " - " + QFileInfo(m_currentFilePath).fileName();
    } else {
        title += " - 新文件";
    }

    if (m_isModified) {
        title += " *";
    }

    setWindowTitle(title);
}

void MainWindow::fitViewToContent()
{
    if (!m_scene || !m_view) return;

    // 使用CanvasScene的新的fitToWindow方法
    m_scene->fitToWindow();
    
    // 缩放后更新所有连线的线宽
    if (CanvasView* canvasView = qobject_cast<CanvasView*>(m_view)) {
        QTimer::singleShot(10, [canvasView]() {
            QList<QGraphicsItem*> items = canvasView->scene()->items();
            for (QGraphicsItem* item : items) {
                ConnectionLine* connectionLine = dynamic_cast<ConnectionLine*>(item);
                if (connectionLine) {
                    connectionLine->updateLineWidth();
                }
            }
        });
    }
    
    qDebug() << "视图已适配到内容，最终缩放因子:" << m_view->transform().m11();
}

//将背景画布网格进行调整
void MainWindow::on_actionmm_triggered()
{
    m_view->setGridSize(10);
    m_scene->set_unit("mm");
}


void MainWindow::on_actioncm_triggered()
{
    m_view->setGridSize(100);
    m_scene->set_unit("cm");
}


void MainWindow::on_actiondm_triggered()
{
    m_view->setGridSize(1000);
    m_scene->set_unit("dm");
}
