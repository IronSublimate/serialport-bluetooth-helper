#ifndef WIDGETPARAITEM_H
#define WIDGETPARAITEM_H

#include <QWidget>
#include <QString>
namespace Ui {
class WidgetParaItem;
}
class MainWindow;
class WidgetParaItem : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetParaItem(const QString& para_name,const QString& pos,const QString& para_value, QWidget *parent = nullptr);
    ~WidgetParaItem();
    //friend class MainWindow;
public:
    QString getName() const;
    void setValue(const QString& value);
    bool operator ==(const WidgetParaItem& other);
    bool operator ==(const QString& other);
signals:
    void send_para_to_MCU(const QString &name, const QString &tx_data_index, const QString &tx_data_value);
protected:
    void changeEvent(QEvent* event);
private slots:
    void on_send_to_MCU_clicked();

private:
    Ui::WidgetParaItem *ui;

    QString pos;

};

#endif // WIDGETPARAITEM_H
