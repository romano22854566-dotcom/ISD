#include <QApplication>
#include "ISD_QT2.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#include <clocale>

int main(int argc,char* argv[])
{
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
    std::setlocale(LC_ALL,"Russian");

    QApplication app(argc,argv);
    ISD_QT2 w;
    w.showMaximized();
    return QApplication::exec();
}
