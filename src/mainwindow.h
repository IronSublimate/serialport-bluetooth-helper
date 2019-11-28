#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QTimer>
#include "iodevice.h"

namespace Ui {
class MainWindow;
}

class SerialSettingsDialog;
class QIODevice;
class QTimer;
class QLabel;
class WidgetParaItem;
class DialogSkin;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    enum Direction_t{
        stop = 0,
        turn_around = 1, //关闭手动控制
        back = 2,
        left = 4,
        right = 6,
        forward = 8,
        clock = 7,
        anticlock = 9
    };

public:
    void init();

public:
    SerialSettingsDialog* uartConfig = nullptr;
protected:
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
private:
    Ui::MainWindow *ui;

    DialogSkin* skinConfig = nullptr;
//    const QString version=APP_VERSION;
//    const QString about_this_title=tr("SerialPort-Bluetooth-helper");
//    const QString author=tr("Hou Yuxuan");
//    const QString version_str=tr("Version: ");
//    const QString author_str=tr("Author: ");
//    const QString source_code_str=tr("Source Code: ");
    const QString source_code_address="https://github.com/IronSublimate/serialport-bluetooth-helper";
    //QIODevice* device = nullptr;
    IODevice iodevice;
    //SerialSettingsDialog* uartConfig = nullptr;
    QLabel *m_status = nullptr;


// Qt ListWidget 类
    QMap<QString,WidgetParaItem*> paraWidgets;
    int cb_index = 0;
private:
    void set_widgets_enabled(bool enable);

    //void com_send_data(const QString &tx_data, bool is_hex, const QString &codetype);
    //QString com_receive_normal(bool is_hex, const QString &code_type);
    void on_device_open();
    void on_device_close();
    void showStatusMessage(const QString &message);

    void change_gui_horizontal_screen();
    void change_gui_vertical_screen();
    void create_signal_slots();
    void set_control_enable(bool enable);
    void set_GUI_enable(bool enable);

private slots:
    void on_clear_dict();
    void update_standard_gui(int index_receive);
    void on_control_key_release();

    void com_receive_data();
    void on_Send_Button_clicked();
    //void on_tabWidget_currentChanged(int index);
    void on_action_exit_triggered();
    void on_actiondfa_triggered();
    void on_actionConnect_triggered(bool checked);

    void on_tabWidget_other_currentChanged(int index);
    void on_pushButton_StartStop_clicked();
    void on_pushButton_Clear_clicked();
    void on_checkBox_manual_stateChanged(int arg1);
    void on_pushButton_stop_clicked();

    void on_pushButton_forward_pressed();
    void on_pushButton_back_pressed();
    void on_pushButton_left_pressed();
    void on_pushButton_right_pressed();
    void on_pushButton_anticlock_pressed();
    void on_pushButton_clock_pressed();
    void on_pushButton_readMCU_clicked();
    void on_action_Uart_triggered();
    void on_actionAboutThis_triggered();
    void on_actionAbout_Qt_triggered();
};

#endif // MAINWINDOW_H
