#include "multicurvesplot.h"
#include "multicurvesplotprivate.h"
#include <QKeyEvent>
#include <QTime>
#include <QtGlobal>
#include <ciso646>

static inline QColor generateColor(){
    return QColor(qrand()%256,qrand()%256,qrand()%256);
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

void MultiCurvesPlot::addData(const QString &name, qreal value)
{
    if(multiCurvesPlotPrivate->timerId>=0){
        if(multiCurvesPlotPrivate->data.contains(name)){
            multiCurvesPlotPrivate->data[name]->addData(
                        QDateTime::currentDateTime().toMSecsSinceEpoch()-multiCurvesPlotPrivate->startTime,
                        value);
        } else {
            auto graph = this->addGraph();
            graph->setPen(generateColor());
            graph->addData(
                        QDateTime::currentDateTime().toMSecsSinceEpoch()-multiCurvesPlotPrivate->startTime,
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
    this->replot();
}

void MultiCurvesPlot::start()
{
    if(multiCurvesPlotPrivate->timerId<0){
        multiCurvesPlotPrivate->startTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        multiCurvesPlotPrivate->timerId = startTimer(multiCurvesPlotPrivate->interval);
    }
}

void MultiCurvesPlot::stop()
{
    killTimer(multiCurvesPlotPrivate->timerId);
    multiCurvesPlotPrivate->timerId = -1;
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

void MultiCurvesPlot::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==multiCurvesPlotPrivate->timerId and multiCurvesPlotPrivate->needRefresh){
        this->rescaleAxes();
        this->replot();
        multiCurvesPlotPrivate->needRefresh = false;
        //qDebug()<<"MultiCurvesPlot::timerEvent";
    }
    QCustomPlot::timerEvent(event);
}
