#pragma once

#include <QMenuBar>

class QMainWindow;
class QMouseEvent;
class QEvent;

struct QRibbonPrivate;

namespace Ui
{
    class QRibbon;
};


class QRibbon : public QMenuBar
{
    Q_OBJECT
public:
    static void install(QMainWindow *window);

    void setColor(const QString &colorName);

    ~QRibbon();

    QRibbon();

public:

    /**
     * 安装本实例至QMainWindow实例，
     * 自动根据QMainWindow的菜单栏创建Ribbon内容
     */
    void initialize(QMainWindow *window);

    /**
     * 卸载
     * TBD
     */
    void uninstall();

    void setStyleButtonVisible(bool visible);

public slots:

    /**
     * 设置标题
     */
    void setWindowTitle(const QString &title);

    /**
     * 切换全屏
     */
    void toggleMaximized();

private slots:

    /**
     * 展开标签页
     */
    void expandTab();

    /**
     * 隐藏标签页
     */
    void hideTab();

    /**
     * 隐藏标签页动画结束
     */
    void onHideTabFinished();

    /**
     * 标签页变化
     */
    void onTabChanged();

    /**
     * 失去焦点时
     */
    void onLostFocus();

    /**
     * tab被点击
     */
    void clickTab();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *tgt, QEvent *e) override;

private:
    Ui::QRibbon *ui;

    friend struct QRibbonPrivate;
    QRibbonPrivate *_;

    QMenu *_styleMenu;

    bool   m_bExpandStaus;
};
