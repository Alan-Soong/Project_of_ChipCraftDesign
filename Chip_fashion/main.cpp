#include "mainwindow_new.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("芯片设计工具 (重构版)");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("ChipCraft Design");

    // 创建主窗口
    MainWindow window;
    window.show();

    return app.exec();
}
