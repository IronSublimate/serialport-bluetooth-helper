#ifndef WIDGETLOADING_H
#define WIDGETLOADING_H


#include <QSplashScreen>
namespace Ui {
class WidgetLoading;
}

class WidgetLoading : public QSplashScreen
{
    Q_OBJECT

public:
    explicit WidgetLoading(QWidget *parent = nullptr);
    ~WidgetLoading();

    void setProcessValue(int value);
private:
    Ui::WidgetLoading *ui;
};

#endif // WIDGETLOADING_H
