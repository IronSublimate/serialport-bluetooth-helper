#ifndef IODEVICE_H
#define IODEVICE_H
#include <QString>
#include <QObject>
#include <QByteArray>
#include <QMap>
#include <QBitmap>
#include "chatclient.h"

class QIODevice;
class MainWindow;
class QSerialPort;
class IODevice:public ChatClient
{
    Q_OBJECT

public:
    IODevice(MainWindow* parent);
//    enum Type{
//        SerialPort,
//        Bluetooth
//    };
    QMap<QString,QString> change_paras ; // 上位机改参数字典
    QMap<QString,QString> watch_paras  ;  // 上位机看参数
    QMap<QString,QString> wave_paras   ;   // 波形字典
private:
    MainWindow* parent;
    QSerialPort *m_serial = nullptr;

    //# 图像类
    QByteArray standard_rx_data;
    const QByteArray img_start_flag=QByteArray("\x01\xfe");
    //# 标准模式类
    QByteArray img_rx_data;
public:
//    Type type = SerialPort;
    QIODevice* device = nullptr;
    QString status_message;
    // 图像类
    QBitmap img ;
    int imgHeight = 80;
    int imgWidth = 60;
    int totalImgSize = this->imgHeight * this->imgWidth;

signals:
    void signal_update_standard_gui(int index);
public :
    bool openDevice();
    void closeDevice();

    void add_to_dict(QMap<QString,QString>& dic,const QList<QByteArray>& list_of_msg);

    QString com_receive_normal(bool is_hex, const QString &code_type);
    void com_receive_standard();
    QBitmap* com_receive_image(int cb_index,int extra_bytes_len);
    void com_send_data(const QString &tx_data, bool is_hex, const QString &codetype);
public slots:
    void send_read_mcu();
    void com_send_change(const QString& name,const QString& tx_data_index,const QString& tx_data_value);
    void sendControlMessage(int direction, int speed) ;
    void sendVectorControlMessage(qreal x,qreal y, int speed);


};

#endif // IODEVICE_H
