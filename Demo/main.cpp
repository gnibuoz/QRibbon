#pragma execution_character_set("utf-8")

#include "QtWidgets/qapplication.h"
#include "vgribbonwindow.h"

int main(int argc, char** argv)
{
	QApplication a(argc, argv);
	VGRibbonWindow w;
	w.show();
	return a.exec();
}