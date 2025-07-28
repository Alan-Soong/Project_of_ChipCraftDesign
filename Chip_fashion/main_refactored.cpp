#include "mainwindow_new.h"
#include "chip/chipmanager.h"
#include "pins/pinmanager.h"
#include "file/filemanager.h"
#include "ui/componentsettingsdialog.h"
#include "ui/pineditordialog.h"
#include <QApplication>
#include <QDebug>

/**
 * 重构后代码使用示例
 */
void demonstrateNewStructure()
{
    qDebug() << "=== 芯片设计界面重构后使用示例 ===";

    // 1. 芯片管理示例
    qDebug() << "\n1. 芯片管理:";
    ChipManager chipManager;
    
    // 创建芯片
    CellItem* chip1 = chipManager.createNewChip(QPointF(100, 100));
    CellItem* chip2 = chipManager.createNewChip(QPointF(300, 200), QSizeF(200, 150));
    
    qDebug() << "创建芯片1:" << chip1->getInstanceName() << "宏名:" << chip1->getMacroName();
    qDebug() << "创建芯片2:" << chip2->getInstanceName() << "宏名:" << chip2->getMacroName();

    // 2. 引脚管理示例
    qDebug() << "\n2. 引脚管理:";
    PinManager pinManager;
    
    // 添加引脚
    pinManager.addPin(chip1, "top", 25.0, 10, "pin1");
    pinManager.addPin(chip1, "bottom", 75.0, 10, "pin2");
    pinManager.addPin(chip1, "left", 50.0, 10, "pin3");
    
    qDebug() << "为芯片1添加了3个引脚";
    qDebug() << "芯片1的引脚数量:" << pinManager.getPins(chip1).size();

    // 3. 文件管理示例
    qDebug() << "\n3. 文件管理:";
    FileManager fileManager;
    
    QList<CellItem*> chipList = {chip1, chip2};
    qDebug() << "准备导出" << chipList.size() << "个芯片的宏定义";

    // 4. 组件信息管理示例
    qDebug() << "\n4. 组件信息管理:";
    ComponentInfo info;
    info.name = "测试芯片";
    info.width = 150;
    info.height = 100;
    info.color = "blue";
    
    qDebug() << "组件信息 - 名称:" << info.name 
             << "尺寸:" << info.width << "x" << info.height 
             << "颜色:" << info.color;

    qDebug() << "\n=== 示例完成 ===";
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 演示重构后的代码结构
    demonstrateNewStructure();

    // 创建主窗口
    MainWindow window;
    window.show();

    return app.exec();
}
