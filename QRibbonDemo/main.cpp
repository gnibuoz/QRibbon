#include "vgribbonwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VGRibbonWindow w;
    w.show();
    return a.exec();
}
