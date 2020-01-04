#include "widgetpainter.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTime>
#include <QMessageBox>
#include <ciso646>

WidgetPainter::WidgetPainter(QWidget *parent) : QWidget(parent)
{

}


void WidgetPainter::set_pixmap(QPixmap *pix)
{
    if(not this->pause){
        this->qpix = pix;
        this->repaint();
    }
}

void WidgetPainter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    if(this->qpix){
        painter.drawPixmap({0,0,this->width(),this->height()},*(this->qpix));
    }


    if(this->enable_grid){
        painter.setPen(Qt::darkBlue);
        painter.drawLines(this->grid_points);
    }
}

void WidgetPainter::resizeEvent(QResizeEvent *event)
{
    if(this->enable_grid){
        this->calculate_grid_points();
    }
    QWidget::resizeEvent(event);
}

void WidgetPainter::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x()*this->imgWidth/this->width();
    int y = event->y()*this->imgHeight/this->height();
    QString f_string = tr("mouse position x:%1,y:%2").arg(x,3).arg(y,3);

    emit this->set_position_text(f_string);
}

void WidgetPainter::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_P){
        this->pause=not this->pause;
        emit this->set_pause_text(this->pause);
    }
}

void WidgetPainter::calculate_grid_points()
{
    qreal x=0.0,y=0.0;
    qreal pix_width = static_cast<qreal>(this->width())/this->imgWidth;
    qreal pix_height = static_cast<qreal>(this->height())/this->imgHeight;
    this->grid_points.clear();
    while(x<this->width()){
        this->grid_points.append(QLineF(x,0,x,this->height()));
        x+=pix_width;
    }
    this->grid_points.append(QLineF(this->width(),this->width(),x,this->height()));
    while(y<this->height()){
        this->grid_points.append(QLineF(0,y,this->width(),y));
        y+=pix_height;
    }
    this->grid_points.append(QLineF(0,this->height(),this->width(),this->height()));
}

void WidgetPainter::saveImg()
{
    auto s = QTime::currentTime().toString();
    if(this->qpix){
        this->qpix->save("./output/"+s+".png","png",-1);
        QMessageBox::information(this,tr("成功"),tr("保存成功"));
    }

}
