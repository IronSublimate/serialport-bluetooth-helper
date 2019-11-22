#include "iodevice.h"
#include <QIODevice>
#include <QBluetoothServiceInfo>
#include <QBluetoothDeviceInfo>
#include <QSerialPort>
#include "serialsettingsdialog.h"
#include "mainwindow.h"
#include <ciso646>

IODevice::IODevice(MainWindow* parent)
{
    this->parent = parent;
    this->m_serial = new QSerialPort(this);
}
// 将字符串添加到对应的字典中
void IODevice::add_to_dict(QMap<QString, QString> &dic, const QList<QByteArray> &list_of_msg)
{
    for(const auto& entry:list_of_msg){
        if(not entry.isEmpty()){
            auto index = entry.indexOf(':');
            if(index>=0){
                auto key = QString::fromUtf8(entry.data(),index);
                auto value = QString::fromUtf8(entry.begin()+index+1,entry.length()-(index+1));
//                if(dic.contains(key)){
//                    dic.
//                }
                dic[key]=value;
            }
        }
    }
}
// 串口改参数模式发送函数
void IODevice::com_send_change(const QString &name, const QString &tx_data_index, const QString &tx_data_value)
{
    if(this->device and this->device->isOpen()){
        QByteArray tx_data0(1,'\xb1');
        bool isOK0,isOK1;
        tx_data_index.toUInt(&isOK0,16);
        tx_data_value.toUInt(&isOK1,16);
        if(isOK1 and isOK1){
             tx_data0+=tx_data_index.toUtf8()+' '+tx_data_value.toUtf8();
             this->change_paras[name]=tx_data_value;
        }
    }
}

void IODevice::sendControlMessage(int direction, int speed)
{
    if(this->device and this->device->isOpen()){
        QByteArray byte_buffer;
        byte_buffer.append((char)0xc1);
        byte_buffer.append(QString::number(direction).toUtf8());
        byte_buffer.append(' ');
        byte_buffer.append(QString::number(speed).toUtf8());
        byte_buffer.append((char)0);
        this->device->write(byte_buffer);
    }
}

void IODevice::sendVectorControlMessage(qreal x, qreal y, int speed)
{
    if(this->device and this->device->isOpen()){
        QByteArray byte_buffer;
        byte_buffer.append((char)0xc3);
        byte_buffer.append(QString::number(x,'f',3).toUtf8());
        byte_buffer.append(' ');
        byte_buffer.append(QString::number(y,'f',3).toUtf8());
        byte_buffer.append(' ');
        byte_buffer.append(QString::number(speed).toUtf8());
        byte_buffer.append((char)0);
        this->device->write(byte_buffer);
    }
}

// 读取改参数列表
void IODevice::send_read_mcu()
{
    if(this->device and this->device->isOpen()){
        QByteArray start("\xb3\x00",2);
        this->device->write(start);
    }
}

bool IODevice::openDevice()
{
    const SerialSettingsDialog::Settings p = parent->uartConfig->settings();
    if(p.portType==SerialSettingsDialog::SerialPort){

        m_serial->setPortName(p.name);
        m_serial->setBaudRate(p.baudRate);
        m_serial->setDataBits(p.dataBits);
        m_serial->setParity(p.parity);
        m_serial->setStopBits(p.stopBits);
        m_serial->setFlowControl(p.flowControl);
        device = m_serial;
        if (device->open(QIODevice::ReadWrite)) {
            this->status_message = QIODevice::tr("Connected to %1 : %2, %3, %4, %5, %6")
                                  .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                  .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl);
            return true;
        } else {
            return false;
        }
    } else if (p.portType==SerialSettingsDialog::Bluetooth) {
        QBluetoothServiceInfo service=p.m_service;
        qDebug() << "Connecting to service 2" << service.serviceName()
                 << "on" << service.device().name();

        // Create client
        qDebug() << "Going to create client";
        qDebug() << "Connecting...";

//                connect(client, &ChatClient::messageReceived,
//                        this, &Chat::showMessage);
//                connect(client, &ChatClient::disconnected,
//                        this, QOverload<>::of(&Chat::clientDisconnected));
//                connect(client, QOverload<const QString &>::of(&ChatClient::connected),
//                        this, &Chat::connected);
//                connect(client, &ChatClient::socketErrorOccurred,
//                        this, &Chat::reactOnSocketError);
//                connect(this, &Chat::sendMessage, client, &ChatClient::sendMessage);
        qDebug() << "Start client";
                this->startClient(service);
        socket->open(QIODevice::ReadWrite);
        this->device = socket;
    }
    return true;
}

void IODevice::closeDevice()
{
    if(device){
        device->close();
    }
}

// 串口普通模式发送数据
void IODevice::com_send_data(const QString &tx_data, bool is_hex,const QString &codetype){
    if(tx_data.length()==0){
        return ;
    }
    if(not is_hex){
        QByteArray ss;
        if(codetype=="utf-8"){
            ss = tx_data.toUtf8();
            this->device->write(ss);
        } else if(codetype=="gb2312"){
            this->device->write(tx_data.toLocal8Bit());
        }
    } else {
        //TODO
        auto hex_data = tx_data.split(' ',QString::SplitBehavior::SkipEmptyParts);
        QByteArray byteArray;
        for (const auto& s : hex_data) {
            bool isOK;
            char value = static_cast<char>(s.toInt(&isOK,16));
            if(isOK){
                byteArray.push_back(value);
            }
        }
        this->device->write(byteArray);
    }
}

QString IODevice::com_receive_normal(bool is_hex,const QString& code_type){
    auto rx_data = this->device->readAll();
    if(not is_hex){
        if(code_type == "utf-8"){
            return QString::fromUtf8(rx_data);
        } else if (code_type == "gb2312") {
            return QString::fromLocal8Bit(rx_data);
        } else {
            return QString();
        }
    } else {
        QString ss;
        for(const auto c:rx_data){
            ss.push_back(QString::number(static_cast<uint>(c),16));
            ss.push_back(' ');
        }
        return ss;
    }
}

void IODevice::com_receive_standard()
{
    auto rx_data = this->device->readAll();
    this->standard_rx_data+=rx_data;
    while (true) {
        auto index = this->standard_rx_data.indexOf(static_cast<char>(0));
        if(index<0){
            break;
        } else if(index == 0){
            this->standard_rx_data.remove(0,1);
            break;
        }
        qint8 msg = static_cast<qint8>(this->standard_rx_data[0]);
        auto standard_rx_data_temp = QByteArray(this->standard_rx_data.data()+1,index-1);
//        this->standard_rx_data=QByteArray(this->standard_rx_data.begin()+index+1);
        this->standard_rx_data.remove(0,index+1);

        if(standard_rx_data_temp.size()<=0){
            break;
        }

        auto list_of_msg = standard_rx_data_temp.split('\n');
        switch (msg) {
        case '\xa0':// 看参数模式
            this->add_to_dict(this->watch_paras,list_of_msg);
            emit this->signal_update_standard_gui(0);
            break;
        case '\xa8':// 波形模式
            this->add_to_dict(this->wave_paras,list_of_msg);
            this->signal_update_standard_gui(1);
            break;
        case '\xb2':// 改参数模式，读取参数
            this->add_to_dict(this->change_paras,list_of_msg);
            this->signal_update_standard_gui(2);
            break;
        case '\xb0':// 改参数模式，成功修改参数
            this->signal_update_standard_gui(-1);
            break;
        default:
            break;
        }
    }
}

QBitmap* IODevice::com_receive_image(int cb_index, int extra_bytes_len)
{
    if(this->img_rx_data.size()==0){
        this->img_rx_data=device->readAll();
        if(not this->img_rx_data.startsWith(this->img_start_flag)){//不是图像的起始位
            this->img_rx_data.clear();
            return nullptr;
        }
    } else {
        this->img_rx_data+=device->readAll();

        //校验位两位，其余14位为传的数据
        if(this->img_rx_data.size()>=this->totalImgSize+img_start_flag.size()+extra_bytes_len){
            auto extra_data=QByteArray(this->img_rx_data.begin()+img_start_flag.size(),extra_bytes_len);
            uchar* img_start_pos = reinterpret_cast<uchar*>(this->img_rx_data.data()+img_start_flag.size()+extra_bytes_len);
            if(cb_index==0){//二值化图像
                this->img=QBitmap::fromData(
                {this->imgWidth,this->imgHeight},
                            img_start_pos,
                            QImage::Format_Mono);

            } else if(cb_index==1){//灰度图像
                for(auto p=img_start_pos;p<img_start_pos+this->imgWidth+this->imgHeight;++p){
                    if(*p>0){
                        *p=255;
                    }
                }
                this->img=QBitmap::fromImage(
                            QImage(img_start_pos, this->imgWidth, this->imgHeight, QImage::Format_Grayscale8)
                            );
            }

            this->img_rx_data.clear();
            return &this->img;
        }//if

    }//if
    return nullptr;
}


