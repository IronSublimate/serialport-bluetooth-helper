#ifndef BLUETOOTHDIALOG_H
#define BLUETOOTHDIALOG_H

#include <QDialog>

namespace Ui {
class BluetoothDialog;
}

class BluetoothDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BluetoothDialog(QWidget *parent = nullptr);
    ~BluetoothDialog();

private:
    Ui::BluetoothDialog *ui;
};

#endif // BLUETOOTHDIALOG_H
