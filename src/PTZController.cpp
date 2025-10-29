#include "PTZController.h"
#include <QThread>

PTZController::PTZController(const std::string &port, int baudrate)
    : connected(false), lastPanSpeed(0), lastTiltSpeed(0), lastZoomSpeed(0)
{
    serial = std::make_unique<QSerialPort>();
    serial->setPortName(QString::fromStdString(port));
    serial->setBaudRate(baudrate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    
    if (serial->open(QIODevice::ReadWrite)) {
        connected = true;
        emit commandSent("PTZ conectado em " + QString::fromStdString(port));
    } else {
        emit error("Falha ao abrir porta " + QString::fromStdString(port));
    }
}

PTZController::~PTZController() {
    stop();
    if (serial && serial->isOpen()) {
        serial->close();
    }
}

void PTZController::panTilt(int pan, int tilt) {
    if (!connected) return;
    
    if (pan == lastPanSpeed && tilt == lastTiltSpeed) return;
    
    lastPanSpeed = pan;
    lastTiltSpeed = tilt;
    
    unsigned char panSpd = std::min(std::abs(pan), 24);
    unsigned char tiltSpd = std::min(std::abs(tilt), 20);
    unsigned char panDir = (pan > 0) ? 0x02 : (pan < 0) ? 0x01 : 0x03;
    unsigned char tiltDir = (tilt > 0) ? 0x01 : (tilt < 0) ? 0x02 : 0x03;
    
    QByteArray cmd;
    cmd.append((char)0x81);
    cmd.append((char)0x01);
    cmd.append((char)0x06);
    cmd.append((char)0x01);
    cmd.append((char)panSpd);
    cmd.append((char)tiltSpd);
    cmd.append((char)panDir);
    cmd.append((char)tiltDir);
    cmd.append((char)0xFF);
    
    sendCommand(cmd);
}

void PTZController::zoom(int speed) {
    if (!connected) return;
    
    if (speed == lastZoomSpeed) return;
    lastZoomSpeed = speed;
    
    QByteArray cmd;
    cmd.append((char)0x81);
    cmd.append((char)0x01);
    cmd.append((char)0x04);
    cmd.append((char)0x07);
    
    if (speed > 0) {
        unsigned char spd = std::min(std::abs(speed), 7);
        cmd.append((char)(0x20 | spd));
    } else if (speed < 0) {
        unsigned char spd = std::min(std::abs(speed), 7);
        cmd.append((char)(0x30 | spd));
    } else {
        cmd.append((char)0x00);
    }
    
    cmd.append((char)0xFF);
    sendCommand(cmd);
}

void PTZController::home() {
    if (!connected) return;
    
    QByteArray cmd;
    cmd.append((char)0x81);
    cmd.append((char)0x01);
    cmd.append((char)0x06);
    cmd.append((char)0x04);
    cmd.append((char)0xFF);
    
    sendCommand(cmd);
    emit commandSent("Retornando para HOME");
}

void PTZController::stop() {
    if (!connected) return;
    
    lastPanSpeed = 0;
    lastTiltSpeed = 0;
    lastZoomSpeed = 0;
    
    QByteArray cmd;
    cmd.append((char)0x81);
    cmd.append((char)0x01);
    cmd.append((char)0x06);
    cmd.append((char)0x01);
    cmd.append((char)0x00);
    cmd.append((char)0x00);
    cmd.append((char)0x03);
    cmd.append((char)0x03);
    cmd.append((char)0xFF);
    
    sendCommand(cmd);
}

void PTZController::openMenu() {
    if (!connected) return;
    
    QByteArray cmd;
    cmd.append((char)0x81);
    cmd.append((char)0x01);
    cmd.append((char)0x06);
    cmd.append((char)0x06);
    cmd.append((char)0x02);
    cmd.append((char)0xFF);
    
    sendCommand(cmd);
    emit commandSent("Abrindo menu VISCA...");
}

void PTZController::sendCommand(const QByteArray &cmd) {
    if (serial && serial->isOpen()) {
        serial->write(cmd);
        serial->flush();
        serial->waitForBytesWritten(50);
        
        QThread::msleep(30);
        
        emit commandSent(commandToString(cmd));
    }
}

QString PTZController::commandToString(const QByteArray &cmd) {
    QString hex;
    for (unsigned char byte : cmd) {
        hex += QString::number(byte, 16).toUpper().rightJustified(2, '0') + " ";
    }
    return hex.trimmed();
}