#include <QApplication>
#include "ISD_QT2.h"
#include <windows.h>
#include <clocale>

int main(int argc,char* argv[])
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    std::setlocale(LC_ALL,"Russian");

    QApplication app(argc,argv);
    ISD_QT2 w;
    w.showMaximized();
    return app.exec();
}
