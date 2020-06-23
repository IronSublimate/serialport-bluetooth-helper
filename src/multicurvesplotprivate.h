#ifndef MULTICURVESPLOTPRIVATE_H
#define MULTICURVESPLOTPRIVATE_H

#include <QHash>
#include <QString>
#include <QTime>
#include <QDateTime>
class QCPGraph;
class MultiCurvesPlotPrivate
{
public:
    MultiCurvesPlotPrivate();
    QHash<QString,QCPGraph*> data;
    qint64 startTime = 0;//0代表未开始，清除后重新为0

    int interval=30;                   //间隔
    int timerId = -1;//小于0没开始
    bool needRefresh = true;
};

#endif // MULTICURVESPLOTPRIVATE_H
