#pragma execution_character_set("utf-8")

#include "QRibbon.h"
#include "ui_qribbon.h"
#include "QtGui\qevent.h"

#include <QPropertyAnimation>

#include <QDebug>
#include "QtCore\QTimer"
#include "QtWidgets\QMainWindow"
#include "QtCore\qcoreevent.h"

const auto MINIMUM_HEIGHT = 62;

/**
 * QRibbon成员封装
 */
struct QRibbonPrivate
{
	QRibbonPrivate(QRibbon* ribbon)
	{
		// 处理主窗体突然失去焦点时的状态
		_timer = new QTimer();
		QObject::connect(_timer, &QTimer::timeout, ribbon, &QRibbon::onLostFocus);
		_timer->setInterval(100);
	}

	~QRibbonPrivate()
	{
		delete _timer;
	}

	// 处理无边框窗体移动事件
	bool        _pressed = false;
	QPoint      _mouseStartPosition;
	QPoint      _origin;
	QRect _originGeometry;

	QMainWindow* _mainWindow = nullptr;

	QTimer* _timer;

	QPropertyAnimation opacityAnimation;
	QPropertyAnimation animationHideBar;
};

QRibbon::QRibbon()
	: QMenuBar()
	, _(new QRibbonPrivate(this))
{
	ui = new Ui::QRibbon();
	ui->setupUi(this);

	connect(ui->tabWidgetMenuBar, &QTabWidget::tabBarClicked, this, &QRibbon::onTabChanged);
	connect(ui->tabWidgetMenuBar, &QTabWidget::currentChanged, this, &QRibbon::onTabChanged);

	connect(ui->pushButtonFullScreen, &QPushButton::clicked, this, &QRibbon::toggleMaximized);
	connect(ui->pushButtonMinimizeTab, &QPushButton::clicked, this, &QRibbon::hideTab);

	// 隐藏下边线
	connect(&(_->animationHideBar), &QPropertyAnimation::finished, this, &QRibbon::onHideTabFinished);

	// 设置风格主题颜色
	_styleMenu = new QMenu(this);
	_styleMenu->setStyleSheet("QMenu::item{ color:black }");
	auto styleActionGroup = new QActionGroup(_styleMenu);
	styleActionGroup->addAction(_styleMenu->addAction("蓝色", [&]() {setColor("rgb(43, 87, 154)"); }));
	styleActionGroup->addAction(_styleMenu->addAction("绿色", [&]() {setColor("rgb(33,115,70)"); }));
	styleActionGroup->addAction(_styleMenu->addAction("红色", [&]() {setColor("rgb(183, 71, 42)"); }));
	for (auto a : styleActionGroup->actions())
		a->setCheckable(true);
	ui->pushButtonStyle->setMenu(_styleMenu);
}

QRibbon::~QRibbon()
{
	delete _;
	delete ui;
}

void QRibbon::initialize(QMainWindow* window)
{
	_->_mainWindow = window;

	// 设置透明度渐变动画
	if (!_->opacityAnimation.targetObject())
	{
		_->opacityAnimation.setTargetObject(_->_mainWindow);
		_->opacityAnimation.setPropertyName("windowOpacity");
		_->opacityAnimation.setStartValue(1.0);
		_->opacityAnimation.setEndValue(0.6);
	}

	QMenuBar* menuBar = _->_mainWindow->menuBar();

	if (!menuBar) return;

	//_normalGeom = _mainWindow->geometry();

	connect(ui->pushButtonMinimum, &QPushButton::clicked, _->_mainWindow, &QWidget::showMinimized);
	connect(ui->pushButtonClose, &QPushButton::clicked, _->_mainWindow, &QWidget::close);

	ui->tabWidgetMenuBar->clear();

	auto menus = menuBar->actions();
	for (auto i : menus)
	{
		auto menu = i->menu();

		QList<QAction*> actions;

		// 当没有菜单时
		if (!menu)
		{
			actions.push_back(i);
		}
		else
		{
			actions = menu->actions();
		}

		auto widget = new QWidget;
		auto layout = new QHBoxLayout(widget);
		widget->setLayout(layout);

		for (auto a : actions)
		{
			QWidget* w;
			if (a->isSeparator())
			{
				//qDebug() << a->text();

				auto line = new QWidget();
				line->setFixedWidth(1);
				line->setStyleSheet("background:rgb(177,177,177)");
				w = line;
			}
			else
			{
				//QWidget::addAction(a);
				//this->addAction(a);

				_->_mainWindow->addAction(a);

				QToolButton* btn = new QToolButton;

				btn->setText(a->text());
				btn->setToolTip(a->text());
				btn->setStatusTip(a->text());

				a->setToolTip(a->text());
				a->setStatusTip(a->text());

				btn->setIconSize(QSize(32, 32));
				btn->setAutoRaise(true);

				if (a->menu())
				{
					btn->setPopupMode(QToolButton::MenuButtonPopup);
				}
				btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

				btn->setCheckable(a->isCheckable());
				btn->setChecked(a->isChecked());

				// 无图标则自动添加默认图标
				if ( a->icon().isNull() )
				{
					static QIcon defaultIcon(":/Resource/icon/unknow.png");
					a->setIcon(defaultIcon);
				}
				btn->setDefaultAction(a);

				//connect(a, &QAction::triggered, btn, &QToolButton::triggered);

				w = btn;
			}
			layout->addWidget(w);
		}

		layout->setSpacing(6);
		layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
		ui->tabWidgetMenuBar->addTab(widget, i->text());

		//ui->tabWidgetMenuBar->setTabEnabled(ui->tabWidgetMenuBar->count() - 1, (*i));

		menuBar->removeAction(i);	/// 从菜单栏移除原有菜单项，否则当鼠标单击菜单位置时。仍会弹出菜单项
									//addAction(i);
									// menu->setEnabled(false);
	}

	window->setWindowFlag(Qt::FramelessWindowHint, true);

	QObject::connect(window, &QMainWindow::windowTitleChanged, this, &QRibbon::setWindowTitle);
	this->setWindowTitle(window->windowTitle());

	window->installEventFilter(this);

	window->menuBar()->setParent(0);
	window->setMenuBar(this);

	_styleMenu->actions()[0]->trigger();

	_->_originGeometry = window->normalGeometry();

	//window->show();
	//window->hide();
}

void QRibbon::uninstall()
{
	qDebug() << "TBD...";
}

void QRibbon::setStyleButtonVisible(bool visible)
{
	_styleMenu->setVisible(visible);
}

void QRibbon::install(QMainWindow* window)
{
	// 该window已安装过QRibbon
	if (qobject_cast<QRibbon*>(window->menuBar()))
	{
		return;
	}

	auto _ribbonWidget = new QRibbon();
	_ribbonWidget->initialize(window);
}

void QRibbon::setColor(const QString& colorName)
{
	static QString currentColor = "rgb(43,87,154)";

	QString stylesheet;
	if (!_->_mainWindow->styleSheet().isEmpty())
	{
		stylesheet += _->_mainWindow->styleSheet();
		_->_mainWindow->setStyleSheet("");
	}

	if (!ui->widgetContainer->styleSheet().isEmpty())
	{
		stylesheet += ui->widgetContainer->styleSheet();
		ui->widgetContainer->setStyleSheet("");
	}

	if (!qApp->styleSheet().isEmpty())
	{
		stylesheet += qApp->styleSheet();
	}

	//auto&& stylesheet = QString::fromUtf8(ui->widgetContainer->styleSheet().toLocal8Bit());
	stylesheet = stylesheet.replace(currentColor, colorName);
	//ui->widgetContainer->setStyleSheet("");

	//ui->widgetContainer->setStyleSheet(qss);
	qApp->setStyleSheet(stylesheet);
	// qApp->setStyleSheet(qss);

	//this->repaint();

	//this->setStyleSheet(qs);

	currentColor = colorName;
}

void QRibbon::setWindowTitle(const QString& title)
{
	ui->labelTitle->setText(title);
}

void QRibbon::mousePressEvent(QMouseEvent *evt)
{
	if (evt->button() == Qt::LeftButton && evt->y() < ui->labelTitle->height() + 10)
	{
		_->_pressed = true;

		_->_timer->start();

		_->_originGeometry = _->_mainWindow->normalGeometry();

		//获得鼠标的初始位置
		_->_mouseStartPosition = evt->globalPos();
		//获得窗口的初始位置
		_->_origin = _->_mainWindow->frameGeometry().topLeft();

		if (_->opacityAnimation.state() == QPropertyAnimation::Running)
		{
			_->opacityAnimation.stop();
		}

		if (_->_mainWindow->windowOpacity() != _->opacityAnimation.endValue().toDouble())
		{
			_->opacityAnimation.setDuration(250);
			_->opacityAnimation.setDirection(QAbstractAnimation::Forward);
			_->opacityAnimation.start();
		}
	}
	QMenuBar::mousePressEvent(evt);
}

void QRibbon::mouseMoveEvent(QMouseEvent *evt)
{
	if (_->_pressed)
	{
		//获得鼠标移动的距离
		auto&& offset = evt->globalPos() - _->_mouseStartPosition;
		//改变窗口的位置

		bool maxOrFull = _->_mainWindow->windowState() & (Qt::WindowFullScreen | Qt::WindowMaximized);

		if (maxOrFull)
		{
			// 最大化
			if (offset.manhattanLength() > 1)
			{
				auto w = _->_mainWindow;
				auto geom = _->_mainWindow->normalGeometry();
				w->mapFromGlobal(evt->globalPos());
				auto localX = geom.width() * (evt->globalX()*1.0 / w->width());
				geom.moveTopLeft(QPoint(evt->x() - localX, 0));
				toggleMaximized();
				_->_mainWindow->setGeometry(geom);
				_->_origin = _->_mainWindow->frameGeometry().topLeft();
			}
		}
		else
		{
			_->_mainWindow->move(_->_origin + offset);
		}
	}
	//QMenuBar::mouseMoveEvent(event);
}

void QRibbon::mouseReleaseEvent(QMouseEvent *evt)
{
	if (evt->button() == Qt::LeftButton)
	{
		_->_pressed = false;

		auto moved = (evt->globalPos() - _->_mouseStartPosition).manhattanLength() > 3;

		bool maxOrFull = _->_mainWindow->windowState() & (Qt::WindowFullScreen | Qt::WindowMaximized);

		// 移动到屏幕顶部最大化显示
		if (moved && !maxOrFull && evt->globalY() < 3)
		{
			_->_mainWindow->setGeometry(_->_originGeometry);
			toggleMaximized();
			//_normalGeom = _beforeGeometry;
		}
		else if (_->_mainWindow->y() < 0) {
			_->_mainWindow->move(_->_mainWindow->x(), 0);
		}
	}

	if (_->opacityAnimation.state() == QPropertyAnimation::Running)
	{
		_->opacityAnimation.stop();
	}
	if (_->_mainWindow->windowOpacity() != _->opacityAnimation.startValue().toDouble())
	{
		_->opacityAnimation.setDuration(150);
		_->opacityAnimation.setDirection(QAbstractAnimation::Backward);
		_->opacityAnimation.start();
	}

	QMenuBar::mouseReleaseEvent(evt);
}

void QRibbon::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->y() < ui->labelTitle->height() + 10)
	{
		toggleMaximized();
	}

	//QMenuBar::mouseDoubleClickEvent(event);
}

bool QRibbon::eventFilter(QObject* tgt, QEvent* e)
{
	if (tgt == _->_mainWindow && e->type() == QEvent::WindowStateChange)
	{
		ui->pushButtonFullScreen->setText(_->_mainWindow->windowState() & (Qt::WindowFullScreen | Qt::WindowMaximized) ? "⧉" : "☐");
	}

	return QMenuBar::eventFilter(tgt, e);
}

void QRibbon::toggleMaximized()
{
	_->_mainWindow->setWindowState(_->_mainWindow->windowState() & (Qt::WindowFullScreen | Qt::WindowMaximized) ? Qt::WindowActive : Qt::WindowActive | Qt::WindowMaximized);
}

void QRibbon::hideTab()
{
	auto& animationHideBar = _->animationHideBar;
	if (!animationHideBar.targetObject())
	{
		animationHideBar.setTargetObject(this);
		animationHideBar.setPropertyName("minimumHeight");
		animationHideBar.setStartValue(height());
		animationHideBar.setEndValue(MINIMUM_HEIGHT);
	}

	if (animationHideBar.state() == QAbstractAnimation::Running)
	{
		animationHideBar.stop();
	}

	animationHideBar.setDirection(QAbstractAnimation::Forward);
	animationHideBar.start();
}

void QRibbon::onHideTabFinished()
{
	ui->widgetBottomBar->setMinimumHeight(_->animationHideBar.direction() == QAbstractAnimation::Forward ? 0 : 1);
}

void QRibbon::onTabChanged()
{
	if (minimumHeight() == MINIMUM_HEIGHT)
	{
		_->animationHideBar.setDirection(QAbstractAnimation::Backward);
		_->animationHideBar.start();
	}
}

void QRibbon::onLostFocus()
{
	if (!_->_mainWindow) return;

	if (_->_pressed == true)
	{
		if (_->_mainWindow->isActiveWindow())
		{
			return;
		}
		_->_pressed = false;
		_->_mainWindow->setWindowOpacity(1.0);
	}
	_->_timer->stop();
}