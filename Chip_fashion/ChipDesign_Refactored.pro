QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 项目名称
TARGET = ChipDesign_Refactored

# 定义宏
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# QMAKE_POST_LINK += make clean

# 源文件 - 按模块组织
SOURCES += \
    main.cpp \
    # 核心窗口
    mainwindow_new.cpp \
    canvasscene.cpp \
    canvasview.cpp \
    # 芯片相关
    chipmanager.cpp \
    cellitem_new.cpp \
    connectionline.cpp \
    # 引脚相关
    pinmanager.cpp \
    pinitem.cpp \
    # 文件管理
    filemanager.cpp \
    # 用户界面
    # componentsettingsdialog.cpp \
    pineditordialog.cpp

# 头文件 - 按模块组织
HEADERS += \
    # 核心窗口
    mainwindow_new.h \
    canvasscene.h \
    canvasview.h \
    # 芯片相关
    chipmanager.h \
    cellitem_new.h \
    connectionline.h \
    # 引脚相关
    pinmanager.h \
    pinitem.h \
    # 文件管理
    filemanager.h \
    # 用户界面
    # componentsettingsdialog.h \
    pineditordialog.h \
    # 命令模式
    command.h

# UI文件
FORMS += \
    mainwindow.ui \
    pineditordialog.ui

# 资源文件
# RESOURCES += resources.qrc

# 默认部署规则
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 编译器设置
win32 {
    CONFIG += console
}

# 资源文件
RESOURCES += \
    resources.qrc

# 输出目录设置
CONFIG(debug, debug|release) {
    DESTDIR = build/debug
    OBJECTS_DIR = build/debug/obj
    MOC_DIR = build/debug/moc
    RCC_DIR = build/debug/rcc
    UI_DIR = build/debug/ui
}

CONFIG(release, debug|release) {
    DESTDIR = build/release
    OBJECTS_DIR = build/release/obj
    MOC_DIR = build/release/moc
    RCC_DIR = build/release/rcc
    UI_DIR = build/release/ui
}
