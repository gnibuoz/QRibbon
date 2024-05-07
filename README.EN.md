# QRibbon

A Ribbon-style menu bar implemented in Qt, the basic idea is to customize QTabWidget and adjust the display style through QSS (style sheet). The principle of QRibbon is to minimize intrusion into normal business logic development. Therefore, when developing programs based on QMainWindow, you can create interfaces, create normal menu bars and menu items, and associate their signal slots as usual. Finally, call the `QRibbon::install(&amp;mainWindow)` function to automatically create a Ribbon corresponding to the original QMenuBar of QMainWindow...

## Usage
* Create a QMainWindow as usual.
* Add menus and menu items to the QMainWindow (It is recommended to add appropriate icons for all menu items, so that QRibbon can directly use the icons of menu items as button icons).
* Add all files related to the QRibbon library to the project.
* After constructing the main interface, call the following code to add QRibbon:

    ```QRibbon::install(&mainWindow);```

## Comparison of effects before and after using QRibbon::install(window)
![image](https://github.com/gnibuoz/QRibbon/blob/master/images/QRibbon.gif)

## Interface before using QRibbon
![image](https://github.com/gnibuoz/QRibbon/blob/master/images/%E4%BD%BF%E7%94%A8QRibbon%E5%89%8D.png)

## Interface after using QRibbon
![image](https://github.com/gnibuoz/QRibbon/blob/master/images/Qt-Ribbon.gif)
