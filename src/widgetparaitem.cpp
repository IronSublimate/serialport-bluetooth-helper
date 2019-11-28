#include "widgetparaitem.h"
#include "ui_widgetparaitem.h"

WidgetParaItem::WidgetParaItem(const QString& para_name,const QString& pos,const QString& para_value, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetParaItem)
{
    ui->setupUi(this);

    this->pos = pos;
    this->ui->para_name->setText(para_name);
    this->ui->para_value->setText(para_value);
}

WidgetParaItem::~WidgetParaItem()
{
    delete ui;
}

QString WidgetParaItem::getName() const
{
    return this->ui->para_name->text();
}

void WidgetParaItem::setValue(const QString &value)
{
    this->ui->para_value->setText(value);
}

bool WidgetParaItem::operator ==(const WidgetParaItem &other)
{
    return this->getName()==other.getName();
}

bool WidgetParaItem::operator ==(const QString &other)
{
    return this->getName()==other;
}

void WidgetParaItem::changeEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
    QWidget::changeEvent(event);
}

void WidgetParaItem::on_send_to_MCU_clicked()
{
    //const QString &name, const QString &tx_data_index, const QString &tx_data_value
    emit send_para_to_MCU(this->ui->para_name->text(),this->pos,this->ui->para_value->text());
}
