#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QTranslator>
#include "widgetloading.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("Beijing Insititute of Technology Smart Car CLub");
    QApplication::setApplicationName("Serialport-Bluetooth-Helper");
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    MainWindow w;
    w.init();
    w.showMaximized();
#else
    QTranslator translator;
    if(translator.load(QLocale(),"SBHelper","_",":/translation/translation")){
        a.installTranslator(&translator);
    }
    WidgetLoading uiSplash;
    uiSplash.show();
    a.removeTranslator(&translator);
    MainWindow w;
    w.init();
    uiSplash.setProcessValue(100);

    w.show();
    uiSplash.finish(&w);
#endif
    return a.exec();
}
