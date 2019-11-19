#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QString>

#include "widgetloading.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    WidgetLoading uiSplash;
    uiSplash.show();
    w.show();

    uiSplash.setProcessValue(100);
    uiSplash.finish(&w);

    return a.exec();
}
