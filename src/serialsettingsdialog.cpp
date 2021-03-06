﻿/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "serialsettingsdialog.h"
#include "ui_serialsettingsdialog.h"

#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <QtBluetooth/qbluetoothservicediscoveryagent.h>

#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPortInfo>
#include <QSettings>
#include <ciso646>
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras>
#endif

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

static const QLatin1String serviceUuid("00001101-0000-1000-8000-00805F9B34FB");
#ifdef Q_OS_ANDROID
static const QLatin1String reverseUuid("FB349B5F-8000-0080-0010-000001110000");
#endif

SerialSettingsDialog::SerialSettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog),
    m_intValidator(new QIntValidator(0, 4000000, this))
{
    m_ui->setupUi(this);
    m_ui->localAdapterBox->setVisible(false);
#if defined(Q_OS_ANDROID) || defined (Q_OS_IOS)
    m_ui->selectBox->setVisible(false);
#endif
    m_ui->additionalOptionsGroupBox->setVisible(false);
    m_ui->baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    findBluetoothLoacalAdapter();
//    m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(this);

    connect(m_ui->applyButton, &QPushButton::clicked,
            this, &SerialSettingsDialog::apply);
    connect(m_ui->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SerialSettingsDialog::showPortInfo);
    connect(m_ui->baudRateBox,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SerialSettingsDialog::checkCustomBaudRatePolicy);
    connect(m_ui->serialPortInfoListBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SerialSettingsDialog::checkCustomDevicePathPolicy);
//    connect(m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
//            this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
//    connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished()));
//    connect(m_discoveryAgent, SIGNAL(canceled()), this, SLOT(discoveryFinished()));

    fillPortsParameters();
    fillPortsInfo();

    readSettings();
    updateSettings();
}

SerialSettingsDialog::~SerialSettingsDialog()
{
    delete m_ui;
    delete m_discoveryAgent;
}

SerialSettingsDialog::Settings SerialSettingsDialog::settings() const
{
    return m_currentSettings;
}

void SerialSettingsDialog::showPortInfo(int idx)
{
    if (idx == -1)
        return;

    const QStringList list = m_ui->serialPortInfoListBox->itemData(idx).toStringList();
    m_ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    m_ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    m_ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    m_ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    m_ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    m_ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void SerialSettingsDialog::apply()
{
    if(not m_currentSettings.finishedSetting){
        m_currentSettings.finishedSetting = this->foundDevice();
    }
    updateSettings();
    writeSettings();
    hide();
    if (m_discoveryAgent and m_discoveryAgent->isActive()){
        m_discoveryAgent->stop();
    }
}

void SerialSettingsDialog::checkCustomBaudRatePolicy(int idx)
{
    const bool isCustomBaudRate = !m_ui->baudRateBox->itemData(idx).isValid();
    m_ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        m_ui->baudRateBox->clearEditText();
        QLineEdit *edit = m_ui->baudRateBox->lineEdit();
        edit->setValidator(m_intValidator);
    }
}

void SerialSettingsDialog::checkCustomDevicePathPolicy(int idx)
{
    const bool isCustomPath = !m_ui->serialPortInfoListBox->itemData(idx).isValid();
    m_ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        m_ui->serialPortInfoListBox->clearEditText();
}

void SerialSettingsDialog::fillPortsParameters()
{
    auto current_index = m_ui->baudRateBox->currentIndex();
    m_ui->baudRateBox->clear();
    m_ui->baudRateBox->addItem(QStringLiteral("1200"), QSerialPort::Baud1200);
    m_ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_ui->baudRateBox->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    m_ui->baudRateBox->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    m_ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    m_ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    m_ui->baudRateBox->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    m_ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    m_ui->baudRateBox->setCurrentIndex(current_index);
    m_ui->baudRateBox->addItem(tr("Custom"));

    current_index = m_ui->dataBitsBox->currentIndex();
    m_ui->dataBitsBox->clear();
    m_ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    m_ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    m_ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    m_ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    m_ui->dataBitsBox->setCurrentIndex(current_index);

    current_index = m_ui->parityBox->currentIndex();
    m_ui->parityBox->clear();
    m_ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    m_ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    m_ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    m_ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    m_ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);
    m_ui->parityBox->setCurrentIndex(current_index);

    current_index = m_ui->stopBitsBox->currentIndex();
    m_ui->stopBitsBox->clear();
    m_ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    m_ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    m_ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);
    m_ui->stopBitsBox->setCurrentIndex(current_index);

    current_index = m_ui->flowControlBox->currentIndex();
    m_ui->flowControlBox->clear();
    m_ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    m_ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    m_ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
    m_ui->flowControlBox->setCurrentIndex(current_index);
}

void SerialSettingsDialog::fillPortsInfo()
{
    m_ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        m_ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    m_ui->serialPortInfoListBox->addItem("Custom");
}

void SerialSettingsDialog::updateSettings()
{
    m_currentSettings.portType = Type(m_ui->tabWidget->currentIndex());

    m_currentSettings.name = m_ui->serialPortInfoListBox->currentText();
    if (m_ui->baudRateBox->currentIndex() == 9) {
        m_currentSettings.baudRate = m_ui->baudRateBox->currentText().toInt();
    } else {
        m_currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                                         m_ui->baudRateBox->itemData(m_ui->baudRateBox->currentIndex()).toInt());
    }
    m_currentSettings.stringBaudRate = QString::number(m_currentSettings.baudRate);
    m_currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                                     m_ui->dataBitsBox->itemData(m_ui->dataBitsBox->currentIndex()).toInt());
    m_currentSettings.stringDataBits = m_ui->dataBitsBox->currentText();

    m_currentSettings.parity = static_cast<QSerialPort::Parity>(
                                   m_ui->parityBox->itemData(m_ui->parityBox->currentIndex()).toInt());
    m_currentSettings.stringParity = m_ui->parityBox->currentText();

    m_currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                                     m_ui->stopBitsBox->itemData(m_ui->stopBitsBox->currentIndex()).toInt());
    m_currentSettings.stringStopBits = m_ui->stopBitsBox->currentText();

    m_currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                                        m_ui->flowControlBox->itemData(m_ui->flowControlBox->currentIndex()).toInt());
    m_currentSettings.stringFlowControl = m_ui->flowControlBox->currentText();

    m_currentSettings.localEchoEnabled = m_ui->localEchoCheckBox->isChecked();

    //Bluetooth

    m_currentSettings.m_service=m_discoveredServices.value(this->m_ui->remoteDevices->currentItem());
}

void SerialSettingsDialog::findBluetoothLoacalAdapter()
{
    localAdapters = QBluetoothLocalDevice::allDevices();
    if (localAdapters.count() < 2) {
        m_ui->localAdapterBox->setVisible(false);
    } else {
        //we ignore more than two adapters
        m_ui->localAdapterBox->setVisible(true);
        m_ui->firstAdapter->setText(tr("Default (%1)", "%1 = Bluetooth address").
                                  arg(localAdapters.at(0).address().toString()));
        m_ui->secondAdapter->setText(localAdapters.at(1).address().toString());
        m_ui->firstAdapter->setChecked(true);
        connect(m_ui->firstAdapter, &QRadioButton::clicked, this, &SerialSettingsDialog::newAdapterSelected);
        connect(m_ui->secondAdapter, &QRadioButton::clicked, this, &SerialSettingsDialog::newAdapterSelected);
        QBluetoothLocalDevice adapter(localAdapters.at(0).address());
        adapter.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    }
    m_currentSettings.localName = QBluetoothLocalDevice().name();
}

int SerialSettingsDialog::adapterFromUserSelection() const
{
    int result = 0;
    QBluetoothAddress newAdapter = localAdapters.at(0).address();

    if (m_ui->secondAdapter->isChecked()) {
        newAdapter = localAdapters.at(1).address();
        result = 1;
    }
    return result;
}

void SerialSettingsDialog::updateTranslation()
{
    m_ui->serialPortInfoListBox->setItemText(m_ui->serialPortInfoListBox->count() -1,tr("custom"));
    this->showPortInfo(m_ui->serialPortInfoListBox->currentIndex());
    fillPortsParameters();
}

void SerialSettingsDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup(this->objectName());
    QString portName = settings.value(m_ui->serialPortInfoListBox->objectName()).toString();
    auto index = m_ui->serialPortInfoListBox->findText(portName);
    m_ui->serialPortInfoListBox->setCurrentIndex(index);
    m_ui->baudRateBox->setCurrentIndex(settings.value(m_ui->baudRateBox->objectName(),0).toInt());
    m_ui->dataBitsBox->setCurrentIndex(settings.value(m_ui->dataBitsBox->objectName(),0).toInt());
    m_ui->flowControlBox->setCurrentIndex(settings.value(m_ui->flowControlBox->objectName(),0).toInt());
    m_ui->parityBox->setCurrentIndex(settings.value(m_ui->parityBox->objectName(),0).toInt());
    m_ui->stopBitsBox->setCurrentIndex(settings.value(m_ui->stopBitsBox->objectName(),0).toInt());
    m_ui->tabWidget->setCurrentIndex(settings.value(m_ui->tabWidget->objectName(),0).toInt());
    settings.endGroup();
}

void SerialSettingsDialog::writeSettings()
{
    QSettings settings;
    settings.beginGroup(this->objectName());
    settings.setValue(m_ui->serialPortInfoListBox->objectName(),m_ui->serialPortInfoListBox->currentText());
    settings.setValue(m_ui->baudRateBox->objectName(),m_ui->baudRateBox->currentIndex());
    settings.setValue(m_ui->dataBitsBox->objectName(),m_ui->dataBitsBox->currentIndex());
    settings.setValue(m_ui->flowControlBox->objectName(),m_ui->flowControlBox->currentIndex());
    settings.setValue(m_ui->parityBox->objectName(),m_ui->parityBox->currentIndex());
    settings.setValue(m_ui->stopBitsBox->objectName(),m_ui->stopBitsBox->currentIndex());
    settings.setValue(m_ui->tabWidget->objectName(),m_ui->tabWidget->currentIndex());
    settings.endGroup();
}

bool SerialSettingsDialog::foundDevice()
{
    if(this->m_ui->tabWidget->currentIndex()==Type::SerialPort){
        if(this->m_ui->serialPortInfoListBox->currentIndex()<0){
            return false;
        } else{
            return true;
        }
    } else if(this->m_ui->tabWidget->currentIndex()==Type::Bluetooth){
        if(this->m_discoveredServices.isEmpty()){//存设备的字典为空说明没找到
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

void SerialSettingsDialog::on_pushButton_Refresh_clicked()
{
    this->fillPortsInfo();
}


void SerialSettingsDialog::startDiscovery(const QBluetoothUuid &uuid)
{
    m_discoveredServices.clear();
    if(not m_discoveryAgent){
        return;
    }
    m_ui->status->setText(tr("Scanning..."));
    if (m_discoveryAgent->isActive()) {
        m_discoveryAgent->stop();
    }
    m_ui->remoteDevices->clear();

//    m_discoveryAgent->setUuidFilter(uuid);
    Q_UNUSED(uuid);
    m_discoveryAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);

}

void SerialSettingsDialog::stopDiscovery()
{
    if (m_discoveryAgent and m_discoveryAgent){
        m_discoveryAgent->stop();
    }
}

QBluetoothServiceInfo SerialSettingsDialog::service() const
{
    return m_currentSettings.m_service;
}

void SerialSettingsDialog::changeEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        updateTranslation();
        break;
    default:
        break;
    }
    QDialog::changeEvent(event);
}

void SerialSettingsDialog::serviceDiscovered(const QBluetoothServiceInfo &serviceInfo)
{
#if 0
    qDebug() << "Discovered service on"
             << serviceInfo.device().name() << serviceInfo.device().address().toString();
    qDebug() << "\tService name:" << serviceInfo.serviceName();
    qDebug() << "\tDescription:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
    qDebug() << "\tProvider:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceProvider).toString();
    qDebug() << "\tL2CAP protocol service multiplexer:"
             << serviceInfo.protocolServiceMultiplexer();
    qDebug() << "\tRFCOMM server channel:" << serviceInfo.serverChannel();
#endif
    QMapIterator<QListWidgetItem *, QBluetoothServiceInfo> i(m_discoveredServices);
    while (i.hasNext()){
        i.next();
        if (serviceInfo.device().address() == i.value().device().address()){
            return;
        }
    }

    QString remoteName;
    if (serviceInfo.device().name().isEmpty())
        remoteName = serviceInfo.device().address().toString();
    else
        remoteName = serviceInfo.device().name();

    QListWidgetItem *item =
        new QListWidgetItem(QString::fromLatin1("%1 %2").arg(remoteName,
                                                             serviceInfo.serviceName()));

    m_discoveredServices.insert(item, serviceInfo);
    m_ui->remoteDevices->addItem(item);
}

void SerialSettingsDialog::discoveryFinished()
{
    m_ui->status->setText(tr("Select the chat service to connect to."));
}

void SerialSettingsDialog::newAdapterSelected()
{
    const int newAdapterIndex = adapterFromUserSelection();
    if (m_currentSettings.currentAdapterIndex != newAdapterIndex) {
//        server->stopServer();
        m_currentSettings.currentAdapterIndex = newAdapterIndex;
        const QBluetoothHostInfo info = localAdapters.at(m_currentSettings.currentAdapterIndex);
        QBluetoothLocalDevice adapter(info.address());
        adapter.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
//        server->startServer(info.address());
        m_currentSettings.localName = info.name();
    }
}

void SerialSettingsDialog::on_remoteDevices_itemActivated(QListWidgetItem *item)
{
    qDebug() << "got click" << item->text();
//    m_currentSettings.m_service = m_discoveredServices.value(item);
//    if (m_discoveryAgent->isActive()){
//        m_discoveryAgent->stop();
//    }
    //accept();
}

void SerialSettingsDialog::on_pushButton_RefreshBluetooth_clicked()
{

    const QBluetoothAddress adapterAdress = localAdapters.isEmpty() ?
                                           QBluetoothAddress() :
                                           localAdapters.at(m_currentSettings.currentAdapterIndex).address();
    qDebug()<<localAdapters.count();

//    QBluetoothLocalDevice device(adapterAdress);
//    qDebug()<<device.isValid();
    if(m_discoveryAgent){
        delete m_discoveryAgent;
    }
    m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAdress,this);
    connect(m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
    connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished()));
    connect(m_discoveryAgent, SIGNAL(canceled()), this, SLOT(discoveryFinished()));
#ifdef Q_OS_ANDROID
    if (QtAndroid::androidSdkVersion() >= 23)
        this->startDiscovery(QBluetoothUuid(reverseUuid));
    else
        this->startDiscovery(QBluetoothUuid(serviceUuid));
#else
    this->startDiscovery(QBluetoothUuid(serviceUuid));
#endif

}
