#include "rudder.h"
#include <QGraphicsWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QRectF>
#include <QDebug>
#include <QtMath>
#include <QColor>
#include <QString>
#include <QGradient>

static constexpr QPointF rudderCenter(0,0);//圆心坐标
static constexpr double outRadius = 150;//大圆半径
static constexpr double inRadius = 75;//小圆半径

//计算包围⚪的矩形
static constexpr QRectF getRect(const QPointF& center,const double r){
    return QRectF(center.x()-r,center.y()-r,2*r,2*r);
}
//计算小圆心位置，不要超过大圆
static constexpr QPointF getInnerCenter(
        const QPointF& nowInnerCenter,
        const QPointF& outCenter=rudderCenter,
        const double outCircleRadius=outRadius){
    auto vec = nowInnerCenter-outCenter;
    auto dis2 = QPointF::dotProduct(vec,vec);
    auto r2 = outCircleRadius*outCircleRadius;
    if(dis2<=r2){
        return nowInnerCenter;
    } else {
        return outCenter+vec*qSqrt(r2/dis2);
    }
}

Rudder::Rudder(QWidget *parent)
    : QGraphicsView(parent)
{

    this->setStyleSheet("background:transparent;padding:0px;border:0px");
//    this->setBackgroundBrush(Qt::transparent);
//    setAutoFillBackground(true);
    this->setFrameStyle(0);
    auto scene = new QGraphicsScene(this);

    scene->setSceneRect(getRect(rudderCenter,outRadius+inRadius));
    this->setScene(scene);

    this->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    //this->scene()->setSceneRect(getRect(rudderCenter,outRadius+inRadius));
    this->setBackgroundBrush(Qt::transparent);

    outItem = new QGraphicsEllipseItem(getRect(rudderCenter,outRadius));
    inItem = new QGraphicsEllipseItem(getRect(rudderCenter,inRadius));
    //inItem = new RudderItem(getRect(rudderCenter,inRadius),QString(":/svg/in"));

    this->scene()->addItem(outItem);
    this->scene()->addItem(inItem);

    setOutColor(QColor(0,180,255,128));
    setInColor(QColor(0,0,255,100));
}

void Rudder::setInColor(QColor inColor)
{
    m_inColor = inColor;
    QBrush inBrush;
    inBrush.setColor(inColor);
    inBrush.setStyle(Qt::BrushStyle::SolidPattern);
    inItem->setPen(Qt::PenStyle::NoPen);
    inItem->setBrush(inBrush);
}

void Rudder::setOutColor(QColor outColor)
{
    m_outColor = outColor;
    QRadialGradient radialGradient(rudderCenter,outRadius,rudderCenter);
    radialGradient.setColorAt(0.0,QColor(255,255,255,128));
    radialGradient.setColorAt(1.0,outColor);
//    QPen outPen;
//    outPen.setColor(outColor);
//    outPen.setWidth(5);
//    outPen.setStyle(Qt::PenStyle::SolidLine);
    outItem->setPen(QPen(outColor.dark(150),5));
    outItem->setBrush(radialGradient);
}

void Rudder::resizeEvent(QResizeEvent *event){
    //qDebug()<<"resize";
    QGraphicsView::resizeEvent(event);
    fitInView(this->sceneRect(), Qt::KeepAspectRatio);
}

void Rudder::mouseMoveEvent(QMouseEvent *event){
    auto now_pos = getInnerCenter(this->mapToScene(event->pos()));
    auto pos = (now_pos-rudderCenter)/outRadius;
    pos.setY(-pos.y());
    //qDebug()<<pos;
    inItem->setRect(getRect(now_pos,inRadius));
    emit this->rudderMoved(pos.x(),pos.y());
}

void Rudder::mousePressEvent(QMouseEvent *event){
    auto now_pos = getInnerCenter(this->mapToScene(event->pos()));
    auto pos = (now_pos-rudderCenter)/outRadius;
    pos.setY(-pos.y());
    //qDebug()<<pos;
    inItem->setRect(getRect(now_pos,inRadius));
    emit this->rudderMoved(pos.x(),pos.y());
}

void Rudder::mouseReleaseEvent(QMouseEvent *event){
    Q_UNUSED(event)
    auto pos = QPointF(0,0);
    //qDebug()<<pos;
    inItem->setRect(getRect(rudderCenter,inRadius));
    emit this->rudderMoved(pos.x(),pos.y());
}

