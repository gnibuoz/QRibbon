#pragma once

#include <QMainWindow>
namespace Ui { class VGRibbonWindow; };

class VGRibbonWindow : public QMainWindow
{
	Q_OBJECT

public:
	VGRibbonWindow(QWidget *parent = Q_NULLPTR);
	~VGRibbonWindow();
private:
	Ui::VGRibbonWindow *ui;
};
