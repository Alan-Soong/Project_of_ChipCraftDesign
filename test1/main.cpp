#include "mytestproject.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyTestProject w;
    w.show();
    return a.exec();
}
