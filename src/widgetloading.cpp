#include "widgetloading.h"
#include "ui_widgetloading.h"

WidgetLoading::WidgetLoading(QWidget *parent) :
    QSplashScreen(parent,QPixmap(":/img/loading.jpg")),
    ui(new Ui::WidgetLoading)
{
    ui->setupUi(this);
}

WidgetLoading::~WidgetLoading()
{
    delete ui;
}

void WidgetLoading::setProcessValue(int value)
{
    this->ui->progressBar->setValue(value);
}
