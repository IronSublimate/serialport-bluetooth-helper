#include "multicurvesplot.h"
#include "multicurvesplotprivate.h"
#include <QKeyEvent>
#include <QTime>
#include <QtGlobal>
#include <ciso646>
#include <QVector3D>

#define WAVE_DEBUG 1//生成虚假波形用于调试

static inline QColor generateColor(){
    static QVector3D last_vec={255,255,255};
    while(1){
        QVector3D vec(qrand()%256,qrand()%256,qrand()%256);
        if((vec-last_vec).length()>20 and (vec-QVector3D(255,255,255)).length()>20){
            last_vec = vec;
            return QColor(vec[0],vec[1],vec[2]);
        }
    }
}
static inline QColor generateColor4(){
    return QColor(qrand()%256,qrand()%256,qrand()%256,qrand()%256);
}
MultiCurvesPlot::MultiCurvesPlot(QWidget *parent):
    QCustomPlot(parent)
{
//    this->setSelectionRect(new QCPSelectionRect(this));
//    this->setSelectionRectMode(QCP::srmZoom );
    qsrand(static_cast<uint>(QTime::currentTime().msec()));
    this->multiCurvesPlotPrivate = new MultiCurvesPlotPrivate();
    this->axisRect()->setupFullAxesBox(true);
    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

}

MultiCurvesPlot::~MultiCurvesPlot()
{
    delete this->multiCurvesPlotPrivate;
}

bool MultiCurvesPlot::isStart()
{
    return multiCurvesPlotPrivate->timerId>=0;
}

void MultiCurvesPlot::addData(const QString &name, qreal value,int type)//0一直增加(default)，1固定长度，2触发
{
    if(multiCurvesPlotPrivate->timerId>=0){
        if(multiCurvesPlotPrivate->data.contains(name)){
            qreal time = (QDateTime::currentDateTime().toMSecsSinceEpoch()-multiCurvesPlotPrivate->startTime)/1000.0;
            multiCurvesPlotPrivate->data[name]->addData(time, value);
            type=1;
            if(type == 1){

            }
        } else {
            auto graph = this->addGraph();
            QPen pen(generateColor());
            pen.setWidth(2);
            graph->setPen(pen);
            graph->addData(
                        (QDateTime::currentDateTime().toMSecsSinceEpoch()-multiCurvesPlotPrivate->startTime)/1000.0,
                        value);
            graph->setName(name);

            multiCurvesPlotPrivate->data[name] = graph;
        }
        multiCurvesPlotPrivate->needRefresh = true;
    }
}

void MultiCurvesPlot::clear()
{
    for(auto graph:multiCurvesPlotPrivate->data){
        this->removeGraph(graph);
    }

    multiCurvesPlotPrivate->data.clear();
    multiCurvesPlotPrivate->startTime=0;
    this->replot();
}

void MultiCurvesPlot::start()
{
    if(multiCurvesPlotPrivate->timerId<0){
        multiCurvesPlotPrivate->startTime += QDateTime::currentDateTime().toMSecsSinceEpoch();
        multiCurvesPlotPrivate->timerId = startTimer(multiCurvesPlotPrivate->interval);
        this->legend->setVisible(true);
    }
}

void MultiCurvesPlot::stop()
{
    killTimer(multiCurvesPlotPrivate->timerId);
    multiCurvesPlotPrivate->timerId = -1;
    this->legend->setVisible(false);
    multiCurvesPlotPrivate->startTime -=
            QDateTime::currentDateTime().toMSecsSinceEpoch();
    //保存停止的时间，使得starttime为负值，在加入新点后当前时间-starttime为正值，就能从上次停止的位置开始
}


void MultiCurvesPlot::keyPressEvent(QKeyEvent *e)
{
    QCustomPlot::keyPressEvent(e);
    if(e->key()==Qt::Key::Key_Escape){
        this->rescaleAxes();
        this->replot();
    }
}

void MultiCurvesPlot::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::MouseButton::RightButton){
        this->setSelectionRectMode(QCP::srmZoom );
    }
    double posX = xAxis->pixelToCoord(event->pos().x());
    double posY = yAxis->pixelToCoord(event->pos().y());
    qDebug()<<posX<<' '<<posY;
    QCustomPlot::mousePressEvent(event);
}

void MultiCurvesPlot::mouseReleaseEvent(QMouseEvent *event)
{
    QCustomPlot::mouseReleaseEvent(event);
    if(event->button()==Qt::MouseButton::RightButton){
        this->setSelectionRectMode(QCP::srmNone );
    }
}

#if defined(WAVE_DEBUG) && WAVE_DEBUG
#include <QtMath>
static inline qreal gen_fake_wave_points(const QString& name, qreal startValue=0){
    static QMap<QString,qreal> map;
    if(map.contains(name)){
        ++map[name];
    } else {
        map[name]=startValue;
    }
    return qSin((map[name])/10);
}
#endif

void MultiCurvesPlot::timerEvent(QTimerEvent *event)
{
#if defined(WAVE_DEBUG) && WAVE_DEBUG
    if(event->timerId()==multiCurvesPlotPrivate->timerId){
        this->addData("wave0",gen_fake_wave_points("wave0"));
        this->addData("wave1",gen_fake_wave_points("wave1",15.7));
    }
#endif
    if(event->timerId()==multiCurvesPlotPrivate->timerId and multiCurvesPlotPrivate->needRefresh){
        this->rescaleAxes();
        this->xAxis->setRange(this->xAxis->range().upper, 5, Qt::AlignRight);
        this->replot();

        multiCurvesPlotPrivate->needRefresh = false;
        //qDebug()<<"MultiCurvesPlot::timerEvent";
    }
    QCustomPlot::timerEvent(event);
}
