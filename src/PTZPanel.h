#ifndef PTZPANEL_H
#define PTZPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>

class PTZPanel : public QWidget {
    Q_OBJECT

public:
    explicit PTZPanel(QWidget *parent = nullptr);

signals:
    void panTiltRequested(int pan, int tilt);
    void zoomRequested(int zoom);
    void homeRequested();
    void menuRequested();

private:
    QPushButton *upBtn, *downBtn, *leftBtn, *rightBtn;
    QPushButton *zoomInBtn, *zoomOutBtn;
    QPushButton *homeBtn, *menuBtn;
    QSlider *speedSlider;
    
    int currentSpeed;
};

#endif
