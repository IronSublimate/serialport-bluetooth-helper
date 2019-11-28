#include <QString>
#include <QObject>
#include "qcustomplot.h"
static const QString source_code_address="https://github.com/IronSublimate/serialport-bluetooth-helper";
QString aboutThisInformation(){
    const QString about_this_str =
            QString("<p>")+
            QObject::tr("Author: ")+QObject::tr("Hou Yuxuan")+QString("<br/>")+
            //QObject::tr("Copyright: ")+APP_COPYRIGHT+QString("<br/>")+
            QObject::tr("Version: ")+APP_VERSION+QString("<br/>")+
            QObject::tr("QCustomPlot Version: ")+QCUSTOMPLOT_VERSION_STR+QString("<br/>")+
            QObject::tr("QSS are from : ")+
            QString("<a href=\"https://github.com/GTRONICK/QSS\">GTRONICK</a>")+QString(" ")+
            QString("<a href=\"https://github.com/wzguo/QUI\">wzguo</a>")+
            QString("</p>")+
            QObject::tr("Source Code: ")+"<a href=\""+source_code_address+"\">"+source_code_address+"</a>";
    return about_this_str;
}
