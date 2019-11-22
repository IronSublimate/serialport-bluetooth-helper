﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtWidgets/QMessageBox>
#include <QTime>
#include <QTimer>
#include <ciso646>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include "dialogskin.h"
#include <QApplication>
#include "serialsettingsdialog.h"
#include <QDebug>
#include <QTableWidgetItem>
#include <QVector>
#include "widgetparaitem.h"
#include <QtAlgorithms>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    iodevice(this),
    m_status(new QLabel)
{
    ui->setupUi(this);
#if defined(Q_OS_ANDROID) || defined (Q_OS_IOS)

#else
    change_gui_horizontal_screen();
#endif
    //this->timer = new QTimer(this);
    this->uartConfig = new SerialSettingsDialog(this);
    this->skinConfig = new DialogSkin(this);

    ui->statusbar->addWidget(m_status);

    create_signal_slots();
    this->set_control_enable(false);

}

void MainWindow::create_signal_slots()
{
    connect(this->ui->checkBox_showGrid,&QCheckBox::stateChanged,this->ui->label_img,&WidgetPainter::set_enable_grid);
    connect(this->ui->pushButton_saveImg,&QPushButton::clicked,this->ui->label_img,&WidgetPainter::saveImg);
    connect(this->ui->pushButton_clear_dict,&QPushButton::clicked,this,&MainWindow::on_clear_dict);
    connect(this->ui->rudder,&Rudder::rudderMoved,[this](qreal x,qreal y){
        this->iodevice.sendVectorControlMessage(x,y,this->ui->horizontalSlider_speed->value()*100);
    });
    connect(&this->iodevice,&IODevice::signal_update_standard_gui,this,&MainWindow::update_standard_gui);
}

void MainWindow::set_control_enable(bool enable)
{
    this->ui->horizontalSlider_speed->setEnabled(enable);
    this->ui->pushButton_anticlock->setEnabled(enable);
    this->ui->pushButton_back->setEnabled(enable);
    this->ui->pushButton_clock->setEnabled(enable);
    this->ui->pushButton_left->setEnabled(enable);
    this->ui->pushButton_right->setEnabled(enable);
    this->ui->pushButton_stop->setEnabled(enable);
    this->ui->pushButton_forward->setEnabled(enable);
    this->ui->rudder->setEnabled(enable);
}

void MainWindow::set_GUI_enable(bool enable)
{
    ui->action_Uart->setEnabled(enable);
}

void MainWindow::on_clear_dict()
{
    this->iodevice.wave_paras.clear();
    this->iodevice.watch_paras.clear();
    this->ui->tableWidget_para->clear();
    this->ui->tableWidget_para->setRowCount(0);
}

void MainWindow::update_standard_gui(int index_receive)
{
    auto index = this->ui->tabWidget_other->currentIndex();
    //if(index ==0 and index_receive<2 ){//读参数模式
    if(index_receive<2 ){//读参数模式
        for(const auto & key:this->iodevice.watch_paras.keys()){
            auto lis = this->ui->tableWidget_para->findItems(key,Qt::MatchExactly);
            auto value = this->iodevice.watch_paras.value(key);
            if(lis.size()>0 and lis[0]->column()==0){//有该元素
                this->ui->tableWidget_para->item(lis[0]->row(),1)->setText(value);

            }else{//没有该元素
                auto length = this->ui->tableWidget_para->rowCount();
                this->ui->tableWidget_para->insertRow(length);
                this->ui->tableWidget_para->setItem(length,0,new QTableWidgetItem(key,0));
                this->ui->tableWidget_para->setItem(length,1,new QTableWidgetItem(value,0));
                this->ui->tableWidget_para->sortByColumn(0,Qt::SortOrder::AscendingOrder);
            }
        }
        for(const auto & key:this->iodevice.wave_paras.keys()){
            auto lis = this->ui->tableWidget_para->findItems(key,Qt::MatchExactly);
            auto value = this->iodevice.wave_paras.value(key);
            if(lis.size()>0 and lis[0]->column()==0){//有该元素
                this->ui->tableWidget_para->item(lis[0]->row(),1)->setText(value);

            }else{//没有该元素
                auto length = this->ui->tableWidget_para->rowCount();
                this->ui->tableWidget_para->insertRow(length);
                this->ui->tableWidget_para->setItem(length,0,new QTableWidgetItem(key,0));
                this->ui->tableWidget_para->setItem(length,1,new QTableWidgetItem(value,0));
                this->ui->tableWidget_para->sortByColumn(0,Qt::SortOrder::AscendingOrder);
            }
        }//for
        if(index_receive == 1){//波形模式
            for(const auto& key:this->iodevice.wave_paras.keys()){
                bool isok=false;
                qreal value = this->iodevice.wave_paras.value(key).toDouble(&isok);
                if(isok){
                    this->ui->widget_CurvesPlot->addData(key,value);
                }
            }
        } //if(index_receive == 1)
    // if(index ==0 and index_receive<2 )//读参数模式
    } else if(index_receive == 2){ //改参数模式
        QVector<QString> error_keys;
        for(const auto &key:this->iodevice.change_paras.keys()){
            auto value_str = this->iodevice.change_paras.value(key);
            auto pos_va=value_str.split(',',QString::SplitBehavior::SkipEmptyParts);
            if(pos_va.size()!=2){
                error_keys.append(key);
            } else{//正常解析
                if(this->paraWidgets.contains(key)){
                    this->paraWidgets.value(key)->setValue(pos_va[1]);
                } else {
                    auto para_widget = new WidgetParaItem (key,pos_va[0],pos_va[1],this);
                    connect(para_widget,&WidgetParaItem::send_para_to_MCU,&(this->iodevice),&IODevice::com_send_change);
                    //TODO
                    this->paraWidgets[key]=para_widget;
                    auto para_list_widget_item = new QListWidgetItem(this->ui->listWidget_para);
                    this->ui->listWidget_para->addItem(para_list_widget_item);
                    this->ui->listWidget_para->setItemWidget(para_list_widget_item,para_widget);
                    auto size=para_widget->minimumSizeHint();
                    para_list_widget_item->setSizeHint(size);
                }
            }
        }//for
        for(const auto& k:error_keys){
            this->iodevice.change_paras.remove(k);
        }
    //else if(index == 2) //改参数模式
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


//void MainWindow::on_about_this(){
//    QMessageBox::about(this,this->about_this_title,this->about_this_text);
//}

void MainWindow::on_Send_Button_clicked()
{
    this->iodevice.com_send_data(ui->textEdit_Send->toPlainText(),
                       ui->hexSending_checkBox->isChecked(),
                       ui->comboBox_codetype->currentText());

}


void MainWindow::on_device_open()
{
    //m_console->setEnabled(true);
    //m_console->setLocalEchoEnabled(p.localEchoEnabled);
    //m_ui->actionConnect->setEnabled(false);
    //m_ui->actionDisconnect->setEnabled(true);
    this->set_GUI_enable(false);
    showStatusMessage(iodevice.status_message);
    connect(this->iodevice.device,&QIODevice::readyRead,this,&MainWindow::com_receive_data);
}

void MainWindow::on_device_close()
{
//    QMessageBox::critical(this, tr("Error"), iodevice.device->errorString());

//    showStatusMessage(tr("Open error"));
    this->set_GUI_enable(true);
    showStatusMessage("");
    disconnect(this->iodevice.device,&QIODevice::readyRead,this,&MainWindow::com_receive_data);
}


void MainWindow::on_actiondfa_triggered()
{
    //DialogSkin dskin;
    auto ret = this->skinConfig->exec();
    if(ret == QDialog::Accepted){
        auto skin = skinConfig->getSkinName();
        if(skin == tr("default")){
            qApp->setStyleSheet("");
            return;
        }
        QFile qss(":/style/style/"+skin+".qss");
        if(qss.open(QFile::ReadOnly)){
            //a.setStyleSheet(QLatin1String(qss.readAll()));
            qApp->setStyleSheet(qss.readAll());
            qss.close();
        } else {
            QMessageBox::warning(nullptr, "warning", "Open failed", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        }
    }
}


//void MainWindow::on_action_Open_Close_triggered()
//{

//}

//void MainWindow::on_action_Bluetooth_triggered()
//{

//}

//void MainWindow::on_action_Uart_triggered()
//{

//}

void MainWindow::on_action_exit_triggered()
{
    QApplication::exit();
}

void MainWindow::com_receive_data(){
    auto tab_widget_current_index = ui->tabWidget->currentIndex();
    if(tab_widget_current_index == 0){//收发模式
        auto msg = this->iodevice.com_receive_normal( ui->hexShowing_checkBox->isChecked(),
                           ui->comboBox_codetype->currentText());
        ui->textEdit_Recive->moveCursor(QTextCursor::End);
        ui->textEdit_Recive->insertPlainText(msg);
    } else if (tab_widget_current_index == 1) {//图像模式
        QPixmap* ret = this->iodevice.com_receive_image(this->cb_index,14);
        if(ret){
            this->ui->label_img->set_pixmap(ret);
        }
    } else if (tab_widget_current_index == 2) {//标准模式
        this->iodevice.com_receive_standard();

    }
}


void MainWindow::on_actionConnect_triggered(bool checked)
{
    qDebug()<<checked;
    if(checked){//打开
        if(iodevice.openDevice()){
            ui->actionConnect->setText(tr("Disconnect"));
            on_device_open();
        } else { //没打开
            ui->actionConnect->setChecked(false);
        }
    } else {//关闭
        iodevice.closeDevice();
        ui->actionConnect->setText(tr("Connect"));
        on_device_close();
    }
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

//横向屏幕的布局
void MainWindow::change_gui_horizontal_screen()
{
    ui->widget_key->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->rudder->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    delete ui->widget_control->layout();
    auto vlayout = new QVBoxLayout(ui->widget_control);

    vlayout->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    vlayout->addWidget(ui->widget_key);
    vlayout->addWidget(ui->rudder);

    delete ui->tab_other->layout();
    auto hlayout = new QHBoxLayout(ui->tab_other);
    hlayout->setObjectName(QString::fromUtf8("verticalLayout"));
    hlayout->addWidget(ui->tabWidget_other);
    hlayout->addWidget(ui->widget_control);
}



void MainWindow::on_tabWidget_other_currentChanged(int index)
{
//    this->update_standard_gui(index);
}

void MainWindow::on_pushButton_StartStop_clicked()
{
    if(this->ui->widget_CurvesPlot->isStart()){
        this->ui->widget_CurvesPlot->stop();
        this->ui->pushButton_StartStop->setText(tr("Start"));
    } else {
        this->ui->widget_CurvesPlot->start();
        this->ui->pushButton_StartStop->setText(tr("Stop"));
    }
}


//手动控制
void MainWindow::on_pushButton_Clear_clicked()
{
    this->ui->widget_CurvesPlot->clear();
}

void MainWindow::on_checkBox_manual_stateChanged(int arg1)
{
    set_control_enable(bool(arg1));
    if(arg1==0){
        this->iodevice.sendControlMessage(
                    turn_around,this->ui->horizontalSlider_speed->value()*100);
    } else {

    }
}

void MainWindow::on_pushButton_stop_clicked()
{
    this->iodevice.sendControlMessage(
                stop,this->ui->horizontalSlider_speed->value()*100);
}



void MainWindow::on_pushButton_forward_clicked()
{
    this->iodevice.sendControlMessage(
                forward,this->ui->horizontalSlider_speed->value()*100);
}

void MainWindow::on_pushButton_back_clicked()
{
    this->iodevice.sendControlMessage(
                back,this->ui->horizontalSlider_speed->value()*100);
}

void MainWindow::on_pushButton_left_clicked()
{
    this->iodevice.sendControlMessage(
                left,this->ui->horizontalSlider_speed->value()*100);
}

void MainWindow::on_pushButton_right_clicked()
{
    this->iodevice.sendControlMessage(
                right,this->ui->horizontalSlider_speed->value()*100);
}

void MainWindow::on_pushButton_anticlock_clicked()
{
    this->iodevice.sendControlMessage(
                anticlock,this->ui->horizontalSlider_speed->value()*100);
}

void MainWindow::on_pushButton_clock_clicked()
{
    this->iodevice.sendControlMessage(
                clock,this->ui->horizontalSlider_speed->value()*100);
}


void MainWindow::on_pushButton_readMCU_clicked()
{
    this->iodevice.send_read_mcu();
}

void MainWindow::on_action_Uart_triggered()
{
//    uartConfig->fillPortsInfo();
    uartConfig->show();
}

void MainWindow::on_actionAboutThis_triggered()
{
    QMessageBox::about(this,this->about_this_title,
                       QString("<p>")+
                       this->author_str+this->author+"<br/>"+
                       this->version_str+this->version+
                       QString("QCustomPlot Version:")+QCUSTOMPLOT_VERSION_STR+
                       QString("</p>")+
                       this->source_code_str+"<a href=\""+this->source_code_address+"\">"+this->source_code_address+"</a>"
                       );
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}
