#ifndef RUDDER_H
#define RUDDER_H



#include <QGraphicsView>
#include <QColor>

class Rudder : public QGraphicsView
{
    Q_OBJECT
    Q_PROPERTY(QColor inColor READ inColor WRITE setInColor)
    Q_PROPERTY(QColor outColor READ outColor WRITE setOutColor)

public:
    Rudder(QWidget *parent = nullptr);
    ~Rudder()override = default;

    QColor inColor() const
    {
        return m_inColor;
    }

    QColor outColor() const
    {
        return m_outColor;
    }

public slots:
    void setInColor(QColor inColor);

    void setOutColor(QColor outColor);

signals:
    void rudderMoved(qreal x,qreal y);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent( QMouseEvent * event )override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QGraphicsEllipseItem* inItem;
    QGraphicsEllipseItem* outItem;
    QColor m_inColor;
    QColor m_outColor;
};

#endif // RUDDER_H
