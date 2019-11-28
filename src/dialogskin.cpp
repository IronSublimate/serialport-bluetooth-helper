#include "dialogskin.h"
#include "ui_dialogskin.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTranslator>
#include <QString>
#include <QDebug>

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

    QDir dir(":/style/style/");
    QFileInfoList info_list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
//    ui->comboBox->clear();
    ui->comboBox->addItem("default");
    for(const auto& file_info:info_list){
        ui->comboBox->addItem(file_info.baseName());
    }
    updateSettings();

    if(translator->load(QLocale(),"SBHelper","_",":/translation/translation")){
        qApp->installTranslator(translator);
    }
}

DialogSkin::~DialogSkin()
{
    delete ui;
    delete translator;
}
QString DialogSkin::getSkinName() const{
    return ui->comboBox->currentText();
}

DialogSkin::Settings DialogSkin::getSettings() const
{
    return this->settings;
}

void DialogSkin::accept()
{
    auto skin = this->getSkinName();
    if(settings.skinIndex!=ui->comboBox->currentIndex()){
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
    if(settings.languageIndex!=ui->comboBox_Lang->currentIndex()){
        auto current_index = ui->comboBox_Lang->currentIndex();

        if(translator->load(fileNames[current_index],translationDir)){

        }
    }
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

void DialogSkin::updateSettings()
{
    settings.languageIndex=ui->comboBox_Lang->currentIndex();
    settings.skinIndex=ui->comboBox->currentIndex();
}

void DialogSkin::updateTranslation()
{
    ui->comboBox->setItemText(0,tr("default"));
}
