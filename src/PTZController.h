#ifndef PTZCONTROLLER_H
#define PTZCONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <memory>

class PTZController : public QObject {
    Q_OBJECT

public:
    PTZController(const std::string &port, int baudrate);
    ~PTZController();
    
public slots:
    void panTilt(int pan, int tilt);
    void zoom(int speed);
    void home();
    void stop();
    void openMenu();

signals:
    void commandSent(const QString &cmd);
    void error(const QString &msg);

private:
    void sendCommand(const QByteArray &cmd);
    QString commandToString(const QByteArray &cmd);
    
    std::unique_ptr<QSerialPort> serial;
    bool connected;
};

#endif
