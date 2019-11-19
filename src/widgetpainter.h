#ifndef WIDGETPAINTER_H
#define WIDGETPAINTER_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QLineF>
class QPixmap;
class WidgetPainter : public QWidget
{
    Q_OBJECT
private:
    int imgWidth=80;
    int imgHeight=60;
    QVector<QLineF> grid_points;
    bool enable_grid = false;
    bool pause = false;
    bool enable_extra_14_bytes = false;
    int extra_bytes_len = 0;
    QPixmap* qpix = nullptr;
//    self.extra_data = None  #
public:
    explicit WidgetPainter(QWidget *parent = nullptr);
    void set_img_height_width(int height,int width){
        this->imgWidth  =width;
        this->imgHeight=height;
        this->calculate_grid_points();
    }
    void set_pixmap(QPixmap* pix);
signals:
    void set_position_text(const QString& s);
    void set_pause_text(bool pause);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void calculate_grid_points();
public slots:
    void set_enable_grid(bool enable){
        this->enable_grid = enable;
        if(enable){//勾选
            this->calculate_grid_points();

        }else {

        }
        this->repaint();
    }
    void set_extra_bytes_len(int length){
        if(length>0){
            this->extra_bytes_len = length;
            this->enable_extra_14_bytes = true;
        } else{
            this->enable_extra_14_bytes = false;
        }
    }
    void saveImg();
};

#endif // WIDGETPAINTER_H
