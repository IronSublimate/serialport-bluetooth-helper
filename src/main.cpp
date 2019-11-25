#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QString>

#include "widgetloading.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    MainWindow w;
    w.showMaximized();
#else
    WidgetLoading uiSplash;
    uiSplash.show();
    MainWindow w;
    uiSplash.setProcessValue(100);

    w.show();
    uiSplash.finish(&w);
#endif
    return a.exec();
}
