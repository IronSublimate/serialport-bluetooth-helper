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
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    w.showMaximized();
#else
    WidgetLoading uiSplash;
    uiSplash.show();
    w.show();

    uiSplash.setProcessValue(100);
    uiSplash.finish(&w);
#endif
    return a.exec();
}
