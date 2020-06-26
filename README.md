# QRibbon
Qt 实现的 Ribbon 风格菜单栏，基本思路是定制QTabWidget，通过QSS（样式表）实现显示样式的调整，QRibbon的原则是尽量不侵入正常业务逻辑的开发，所以在开发基于QMainWindow的程序时，可以按照正常的开发流程创建界面，创建普通的菜单栏以及菜单项及其信号槽关联，最后调用QRibbon::install(&amp;mainWindow)函数即可自动创建出与QMainWindow原有QMenuBar相对应的Ribbon...

## 使用方法
* 正常创建QMainWindow
* 为QMainWindow添加菜单和菜单项（建议为所有菜单项都添加合适的图标，这样最终QRibbon就可以直接使用菜单项的图标作为按钮图标）
* 将QRibbon库相关的所有文件添加至工程
* 构造完成主界面后，调用如下代码即可实现QRibbon的添加:  

    ```QRibbon::install(&mainWindow);```

## 使用QRibbon::install(window)前后效果对比
![image](https://github.com/gnibuoz/QRibbon/blob/master/images/QRibbon.gif)

## 使用QRibbon前界面效果
![image](https://github.com/gnibuoz/QRibbon/blob/master/images/%E4%BD%BF%E7%94%A8QRibbon%E5%89%8D.png)

## 使用QRibbon后界面效果
![image](https://github.com/gnibuoz/QRibbon/blob/master/images/Qt-Ribbon.gif)
