#include "AirWar.h"
#include<qicon.h>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AirWar w;
    w.show();
    return a.exec();
}
