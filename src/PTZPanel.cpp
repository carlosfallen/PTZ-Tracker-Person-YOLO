#include "PTZPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

PTZPanel::PTZPanel(QWidget *parent) : QWidget(parent), currentSpeed(10) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    
    // ========== PAN/TILT ==========
    QGroupBox *panTiltGroup = new QGroupBox("🎮 Pan / Tilt");
    QGridLayout *panTiltLayout = new QGridLayout(panTiltGroup);
    panTiltLayout->setSpacing(5);
    
    upBtn = new QPushButton("▲");
    downBtn = new QPushButton("▼");
    leftBtn = new QPushButton("◄");
    rightBtn = new QPushButton("►");
    
    upBtn->setMinimumSize(70, 45);
    downBtn->setMinimumSize(70, 45);
    leftBtn->setMinimumSize(70, 45);
    rightBtn->setMinimumSize(70, 45);
    
    QFont btnFont;
    btnFont.setPointSize(16);
    btnFont.setBold(true);
    upBtn->setFont(btnFont);
    downBtn->setFont(btnFont);
    leftBtn->setFont(btnFont);
    rightBtn->setFont(btnFont);
    
    panTiltLayout->addWidget(upBtn, 0, 1);
    panTiltLayout->addWidget(leftBtn, 1, 0);
    
    QLabel *centerLabel = new QLabel("⊙");
    centerLabel->setAlignment(Qt::AlignCenter);
    centerLabel->setStyleSheet("font-size: 24px; color: #42a5f5;");
    panTiltLayout->addWidget(centerLabel, 1, 1);
    
    panTiltLayout->addWidget(rightBtn, 1, 2);
    panTiltLayout->addWidget(downBtn, 2, 1);
    
    connect(upBtn, &QPushButton::pressed, [this]() { 
        emit panTiltRequested(0, currentSpeed); 
    });
    connect(upBtn, &QPushButton::released, [this]() { 
        emit panTiltRequested(0, 0); 
    });

    connect(downBtn, &QPushButton::pressed, [this]() { 
        emit panTiltRequested(0, -currentSpeed); 
    });
    connect(downBtn, &QPushButton::released, [this]() { 
        emit panTiltRequested(0, 0); 
    });

    connect(leftBtn, &QPushButton::pressed, [this]() { 
        emit panTiltRequested(-currentSpeed, 0); 
    });
    connect(leftBtn, &QPushButton::released, [this]() { 
        emit panTiltRequested(0, 0); 
    });

    connect(rightBtn, &QPushButton::pressed, [this]() { 
        emit panTiltRequested(currentSpeed, 0); 
    });
    connect(rightBtn, &QPushButton::released, [this]() { 
        emit panTiltRequested(0, 0); 
    });
    
    mainLayout->addWidget(panTiltGroup);
    
    // ========== VELOCIDADE ==========
    QGroupBox *speedGroup = new QGroupBox("⚡ Velocidade");
    QVBoxLayout *speedLayout = new QVBoxLayout(speedGroup);
    
    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(1, 24);
    speedSlider->setValue(10);
    speedSlider->setTickPosition(QSlider::TicksBelow);
    speedSlider->setTickInterval(5);
    
    QLabel *speedLabel = new QLabel("Velocidade: 10");
    speedLabel->setAlignment(Qt::AlignCenter);
    
    connect(speedSlider, &QSlider::valueChanged, [this, speedLabel](int value) {
        currentSpeed = value;
        speedLabel->setText(QString("Velocidade: %1").arg(value));
    });
    
    speedLayout->addWidget(speedLabel);
    speedLayout->addWidget(speedSlider);
    
    mainLayout->addWidget(speedGroup);
    
    // ========== ZOOM ==========
    QGroupBox *zoomGroup = new QGroupBox("🔍 Zoom");
    QHBoxLayout *zoomLayout = new QHBoxLayout(zoomGroup);
    
    zoomOutBtn = new QPushButton("➖ OUT");
    zoomInBtn = new QPushButton("➕ IN");
    zoomOutBtn->setMinimumHeight(45);
    zoomInBtn->setMinimumHeight(45);
    
    connect(zoomInBtn, &QPushButton::released, [this]() { 
        emit zoomRequested(0); 
    });
    connect(zoomOutBtn, &QPushButton::released, [this]() { 
        emit zoomRequested(0); 
    });
    
    zoomLayout->addWidget(zoomOutBtn);
    zoomLayout->addWidget(zoomInBtn);
    
    mainLayout->addWidget(zoomGroup);
    
    // ========== AÇÕES ==========
    QGroupBox *actionsGroup = new QGroupBox("🎯 Ações");
    QVBoxLayout *actionsLayout = new QVBoxLayout(actionsGroup);
    
    homeBtn = new QPushButton("🏠 HOME");
    homeBtn->setMinimumHeight(40);
    homeBtn->setStyleSheet("background-color: #2196F3; font-weight: bold;");
    connect(homeBtn, &QPushButton::clicked, this, &PTZPanel::homeRequested);
    
    menuBtn = new QPushButton("⚙ Menu VISCA");
    menuBtn->setMinimumHeight(40);
    menuBtn->setStyleSheet("background-color: #FF9800; font-weight: bold;");
    connect(menuBtn, &QPushButton::clicked, this, &PTZPanel::menuRequested);
    
    actionsLayout->addWidget(homeBtn);
    actionsLayout->addWidget(menuBtn);
    
    mainLayout->addWidget(actionsGroup);
    mainLayout->addStretch();
    
    setEnabled(false);
}
