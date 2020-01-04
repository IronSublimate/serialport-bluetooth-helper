#include "dialogskin.h"
#include "ui_dialogskin.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTranslator>
#include <QString>
#include <QDebug>
#include <QSettings>

static const QString translationDir = ":/translation/translation";
static const QString fileNames[]={
    "SBHelper_zh_CN.qm",
    "SBHelper_en.qm"
};

DialogSkin::DialogSkin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSkin),
    translator(new QTranslator)
{
    ui->setupUi(this);

    fillSkinParameters();
    readSettings();
    updateSettings();
    updateSkin();
//    if(translator->load(QLocale(),"SBHelper","_",":/translation/translation")){
    if(translator->load(fileNames[settings.languageIndex],translationDir)){
        qApp->installTranslator(translator);
    }
}

DialogSkin::~DialogSkin()
{
    delete ui;
    delete translator;
}
QString DialogSkin::getSkinName() const{
    return ui->comboBox_Skin->currentText();
}

DialogSkin::Settings DialogSkin::getSettings() const
{
    return this->settings;
}

void DialogSkin::accept()
{

    if(settings.skinIndex!=ui->comboBox_Skin->currentIndex()){
        updateSkin();
    }
    if(settings.languageIndex!=ui->comboBox_Lang->currentIndex()){
        auto current_index = ui->comboBox_Lang->currentIndex();

        if(translator->load(fileNames[current_index],translationDir)){

        }
    }
    writeSettings();
    updateSettings();
    QDialog::accept();
}

void DialogSkin::changeEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        this->updateTranslation();
        break;
    default:
        break;
    }
    QDialog::changeEvent(event);
}

void DialogSkin::fillSkinParameters()
{
    QDir dir(":/style/style/");
    QFileInfoList info_list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
//    ui->comboBox->clear();
    ui->comboBox_Skin->addItem("default");
    for(const auto& file_info:info_list){
        ui->comboBox_Skin->addItem(file_info.baseName());
    }
}

void DialogSkin::updateSettings()
{
    settings.languageIndex=ui->comboBox_Lang->currentIndex();
    settings.skinIndex=ui->comboBox_Skin->currentIndex();
}

void DialogSkin::updateTranslation()
{
    ui->comboBox_Skin->setItemText(0,tr("default"));
}

void DialogSkin::readSettings()
{
    QSettings settings;
    settings.beginGroup(this->objectName());
    ui->comboBox_Lang->setCurrentIndex(settings.value(ui->comboBox_Lang->objectName(),0).toInt());
    ui->comboBox_Skin->setCurrentIndex(settings.value(ui->comboBox_Skin->objectName(),0).toInt());
    settings.endGroup();
}

void DialogSkin::writeSettings()
{
    QSettings settings;
    settings.beginGroup(this->objectName());
    settings.setValue(ui->comboBox_Lang->objectName(),ui->comboBox_Lang->currentIndex());
    settings.setValue(ui->comboBox_Skin->objectName(),ui->comboBox_Skin->currentIndex());
    settings.endGroup();
}

void DialogSkin::updateSkin()
{
    auto skin = this->getSkinName();
    if(skin == tr("default")){
        qApp->setStyleSheet("");
    } else {
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
