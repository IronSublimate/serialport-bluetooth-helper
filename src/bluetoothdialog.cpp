#include "bluetoothdialog.h"
#include "ui_bluetoothdialog.h"

BluetoothDialog::BluetoothDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BluetoothDialog)
{
    ui->setupUi(this);
}

BluetoothDialog::~BluetoothDialog()
{
    delete ui;
}
