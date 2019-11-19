#include "dialogskin.h"
#include "ui_dialogskin.h"

#include <QDir>
DialogSkin::DialogSkin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSkin)
{
    ui->setupUi(this);
    QDir dir(":/style/style/");
    QFileInfoList info_list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
//    ui->comboBox->clear();
    ui->comboBox->addItem(tr("default"));
    for(const auto& file_info:info_list){
        ui->comboBox->addItem(file_info.baseName());
    }

}

DialogSkin::~DialogSkin()
{
    delete ui;
}
QString DialogSkin::getSkinName() const{
    return ui->comboBox->currentText();
}
