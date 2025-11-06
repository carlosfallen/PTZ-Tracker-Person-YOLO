#include "MainWindow.h"
#include "VideoWidget.h"
#include "PTZPanel.h"
#include "LogPanel.h"
#include "CaptureEngine.h"
#include "PTZController.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QMenuBar>
#include <QStatusBar>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isRunning(false)
{
    setWindowTitle("PTZ Person Tracker Pro - v2.0 (YOLO)");
    resize(1400, 900);
    
    setupUI();
    createMenuBar();
    createStatusBar();
    updateUIState(false);
    
    QTimer::singleShot(100, this, &MainWindow::refreshWebcams);
}

MainWindow::~MainWindow() {
    if (isRunning) onStopClicked();
}

void MainWindow::setupUI() {
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    
    QGroupBox *controlGroup = new QGroupBox("⚙ Configurações");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    
    controlLayout->addWidget(new QLabel("Webcam:"));
    webcamCombo = new QComboBox();
    webcamCombo->addItem("Carregando...", -1);
    controlLayout->addWidget(webcamCombo, 1);
    
    QPushButton *refreshBtn = new QPushButton("🔄");
    refreshBtn->setMaximumWidth(35);
    refreshBtn->setToolTip("Atualizar lista de webcams");
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshWebcams);
    controlLayout->addWidget(refreshBtn);
    
    controlLayout->addWidget(new QLabel("PTZ:"));
    comPortCombo = new QComboBox();
    comPortCombo->addItem("Desabilitado");
    for (const auto &info : QSerialPortInfo::availablePorts()) {
        comPortCombo->addItem(info.portName());
    }
    controlLayout->addWidget(comPortCombo);
    
    controlLayout->addWidget(new QLabel("FPS:"));
    fpsSpinBox = new QSpinBox();
    fpsSpinBox->setRange(1, 60);
    fpsSpinBox->setValue(30);
    controlLayout->addWidget(fpsSpinBox);
    
    controlLayout->addWidget(new QLabel("Precisão:"));
    thresholdSpinBox = new QDoubleSpinBox();
    thresholdSpinBox->setRange(0.1, 0.9);
    thresholdSpinBox->setValue(0.5);
    thresholdSpinBox->setDecimals(2);
    thresholdSpinBox->setSuffix(" conf");
    connect(thresholdSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double val) {
        if (captureEngine) {
            captureEngine->setConfidenceThreshold(val);
            if (logPanel) logPanel->addLog(QString("Threshold: %1").arg(val), 0);
        }
    });
    controlLayout->addWidget(thresholdSpinBox);
    
    autoTrackCheckbox = new QCheckBox("Auto PTZ");
    connect(autoTrackCheckbox, &QCheckBox::toggled, [this](bool checked) {
        if (captureEngine) {
            captureEngine->setAutoTracking(checked);
            if (logPanel) {
                logPanel->addLog(checked ? "Auto PTZ ativado" : "Auto PTZ desativado", 
                               checked ? 1 : 0);
            }
        }
    });
    controlLayout->addWidget(autoTrackCheckbox);
    
    mainLayout->addWidget(controlGroup);
    
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    
    videoWidget = new VideoWidget();
    splitter->addWidget(videoWidget);
    
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    ptzPanel = new PTZPanel();
    rightLayout->addWidget(ptzPanel);
    
    logPanel = new LogPanel();
    rightLayout->addWidget(logPanel);
    
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter, 1);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    startButton = new QPushButton("▶ Iniciar Detecção");
    startButton->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 10px; font-size: 14px;");
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    buttonLayout->addWidget(startButton);
    
    stopButton = new QPushButton("⏹ Parar");
    stopButton->setStyleSheet("background-color: #f44336; color: white; font-weight: bold; padding: 10px; font-size: 14px;");
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);
    buttonLayout->addWidget(stopButton);
    
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    setCentralWidget(central);
    
    logPanel->addLog("✓ Sistema YOLO iniciado", 1);
}

void MainWindow::refreshWebcams() {
    if (!webcamCombo) return;
    
    webcamCombo->clear();
    
    logPanel->addLog("🔍 Procurando webcams...", 0);
    
    int foundCameras = 0;
    
    for (int i = 0; i < 20; i++) {
        cv::VideoCapture cap;
        
        try {
            cap.open(i, cv::CAP_ANY);
            
            if (cap.isOpened()) {
                cv::Mat testFrame;
                bool canRead = cap.read(testFrame);
                
                if (canRead && !testFrame.empty()) {
                    QString cameraName;
                    
                    #ifdef _WIN32
                        std::string backend = cap.getBackendName();
                        if (!backend.empty() && backend != "FFMPEG" && backend != "GStreamer") {
                            cameraName = QString::fromStdString(backend) + QString(" (ID: %1)").arg(i);
                        } else {
                            cameraName = QString("Camera %1").arg(i);
                        }
                    #elif __linux__
                        cameraName = QString("/dev/video%1").arg(i);
                    #elif __APPLE__
                        cameraName = QString("Camera %1").arg(i);
                    #else
                        cameraName = QString("Camera %1").arg(i);
                    #endif
                    
                    if (i >= 10) {
                        cameraName += " (Virtual?)";
                    }
                    
                    webcamCombo->addItem(cameraName, i);
                    foundCameras++;
                }
                
                cap.release();
            }
        } catch (...) {
        }
    }
    
    if (foundCameras == 0) {
        webcamCombo->addItem("Nenhuma webcam detectada", -1);
        logPanel->addLog("⚠ Nenhuma webcam encontrada", 3);
        logPanel->addLog("Dica: Certifique-se que a webcam está conectada", 0);
    } else {
        logPanel->addLog(QString("✓ %1 webcam(s) encontrada(s)").arg(foundCameras), 1);
    }
}

void MainWindow::createMenuBar() {
    QMenuBar *menuBar = new QMenuBar(this);
    
    QMenu *fileMenu = menuBar->addMenu("&Arquivo");
    fileMenu->addAction("&Sair", this, &QWidget::close);
    
    QMenu *helpMenu = menuBar->addMenu("&Ajuda");
    helpMenu->addAction("Sobre", [this]() {
        QMessageBox::about(this, "Sobre", 
            "<h2>PTZ Tracker Pro v2.0</h2>"
            "<p><b>Detecção YOLO de Corpo Inteiro</b></p>"
            "<p>✓ YOLOv8 ONNX<br>"
            "✓ Controle PTZ VISCA<br>"
            "✓ Interface Qt6<br>"
            "✓ Multi-thread<br>"
            "✓ Suporta webcams virtuais (OBS)</p>");
    });
    
    setMenuBar(menuBar);
}

void MainWindow::createStatusBar() {
    statusLabel = new QLabel("Pronto");
    fpsLabel = new QLabel("FPS: --");
    detectionLabel = new QLabel("Detecções: 0");
    
    statusBar()->addWidget(statusLabel, 1);
    statusBar()->addPermanentWidget(detectionLabel);
    statusBar()->addPermanentWidget(fpsLabel);
}

void MainWindow::onStartClicked() {
    if (isRunning) return;
    
    int cameraIndex = webcamCombo->currentData().toInt();
    
    if (cameraIndex < 0) {
        QMessageBox::warning(this, "Erro", "Nenhuma webcam selecionada ou disponível!");
        return;
    }
    
    logPanel->addLog("🚀 Iniciando captura com YOLO...", 1);
    
    captureEngine = std::make_unique<CaptureEngine>(
        std::to_string(cameraIndex),
        fpsSpinBox->value(),
        thresholdSpinBox->value()
    );
    
    captureEngine->setAutoTracking(autoTrackCheckbox->isChecked());
    
    if (comPortCombo->currentText() != "Desabilitado") {
        ptzController = std::make_unique<PTZController>(
            comPortCombo->currentText().toStdString(), 9600
        );
        ptzPanel->setEnabled(true);
        
        connect(ptzPanel, &PTZPanel::panTiltRequested, 
                ptzController.get(), &PTZController::panTilt);
        connect(ptzPanel, &PTZPanel::zoomRequested,
                ptzController.get(), &PTZController::zoom);
        connect(ptzPanel, &PTZPanel::homeRequested,
                ptzController.get(), &PTZController::home);
        connect(ptzPanel, &PTZPanel::menuRequested,
                ptzController.get(), &PTZController::openMenu);
        
        connect(captureEngine.get(), &CaptureEngine::ptzAdjustmentNeeded,
                ptzController.get(), &PTZController::panTilt);
        
        logPanel->addLog("✓ PTZ conectado", 1);
    }
    
    connect(captureEngine.get(), &CaptureEngine::frameReady,
            this, &MainWindow::onFrameReady);
    connect(captureEngine.get(), &CaptureEngine::fpsUpdated,
            this, &MainWindow::onFPSUpdate);
    connect(captureEngine.get(), &CaptureEngine::detectionCount,
            this, &MainWindow::onDetectionCount);
    
    captureEngine->start();
    
    isRunning = true;
    updateUIState(true);
    statusLabel->setText("🎥 Capturando com YOLO...");
    logPanel->addLog("✓ Detecção YOLO ativa!", 1);
}

void MainWindow::onStopClicked() {
    if (!isRunning) return;
    
    captureEngine->stop();
    captureEngine.reset();
    
    if (ptzController) {
        ptzController->stop();
        ptzController.reset();
    }
    
    isRunning = false;
    updateUIState(false);
    statusLabel->setText("⏹ Parado");
    fpsLabel->setText("FPS: --");
    detectionLabel->setText("Detecções: 0");
    logPanel->addLog("⏹ Captura encerrada", 0);
}

void MainWindow::onFrameReady(const QImage &frame) {
    videoWidget->setFrame(frame);
}

void MainWindow::onFPSUpdate(double fps) {
    fpsLabel->setText(QString("FPS: %1").arg(fps, 0, 'f', 1));
}

void MainWindow::onDetectionCount(int count) {
    detectionLabel->setText(QString("👤 Detecções: %1").arg(count));
}

void MainWindow::updateUIState(bool running) {
    startButton->setEnabled(!running);
    stopButton->setEnabled(running);
    webcamCombo->setEnabled(!running);
    comPortCombo->setEnabled(!running);
    fpsSpinBox->setEnabled(!running);
}