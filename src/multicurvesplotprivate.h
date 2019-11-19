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
    qint64 startTime=QDateTime::currentDateTime().toMSecsSinceEpoch();

    int interval=30;                   //间隔
    int timerId = -1;//小于0没开始
    bool needRefresh = true;
};

#endif // MULTICURVESPLOTPRIVATE_H
