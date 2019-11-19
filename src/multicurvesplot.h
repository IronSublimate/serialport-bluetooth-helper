#ifndef MULTICURVESPLOT_H
#define MULTICURVESPLOT_H
#include "qcustomplot.h"

class MultiCurvesPlotPrivate;
class MultiCurvesPlot : public QCustomPlot
{
public:
    MultiCurvesPlot(QWidget *parent = nullptr);
    virtual ~MultiCurvesPlot() override;

public:
    bool isStart();
public slots:
    void addData(const QString& name,qreal value);
    void clear();
    void start();
    void stop();
protected:
    virtual void keyPressEvent(QKeyEvent *e)override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;
private:
    MultiCurvesPlotPrivate* multiCurvesPlotPrivate;
};

#endif // MULTICURVESPLOT_H
