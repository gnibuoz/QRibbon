#include "vgribbonwindow.h"
#include "ui_vgribbonwindow.h"

#include <QShortcut>

#include "../QRibbon/QRibbon.h"

VGRibbonWindow::VGRibbonWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui = new Ui::VGRibbonWindow();
	ui->setupUi(this);

	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	this->addAction(ui->actionFullScreen);
	connect(ui->actionFullScreen, &QAction::triggered, [&]() {

		auto full = ui->actionFullScreen->isChecked();
		menuBar()->setVisible(!full);
		ui->actionFullScreen->setShortcut(  full ? QKeySequence("Esc") : QKeySequence("Ctrl+F"));
		
		static bool maximized = false;// 记录当前状态
		if ( full )
		{
			maximized = isMaximized();
		}
		else if ( maximized && isMaximized() )
		{
			return;
		}

		if ( full && !isMaximized() || !full && isMaximized() )
		{
			if (isMaximized())
			{
				showNormal();
			}
			else
				showMaximized();
		}
	});

	connect(ui->pushButton, &QPushButton::clicked, [this]() {
		QRibbon::install(this);
		this->show();
	});
}

VGRibbonWindow::~VGRibbonWindow()
{
	delete ui;
}
