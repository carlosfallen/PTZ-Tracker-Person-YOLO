#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Metadados
    QApplication::setApplicationName("PTZ Tracker Pro YOLO");
    QApplication::setApplicationVersion("2.0");
    QApplication::setOrganizationName("PTZTracker");
    
    // Estilo moderno
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Paleta escura
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    app.setPalette(darkPalette);
    
    // CSS adicional
    app.setStyleSheet(R"(
        QGroupBox {
            border: 2px solid #555;
            border-radius: 5px;
            margin-top: 10px;
            font-weight: bold;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
        QPushButton {
            border-radius: 4px;
            padding: 5px;
            min-height: 25px;
        }
        QPushButton:hover {
            border: 2px solid #42a5f5;
        }
        QPushButton:pressed {
            background-color: #1976d2;
        }
        QComboBox, QSpinBox, QDoubleSpinBox, QLineEdit {
            padding: 5px;
            border: 1px solid #555;
            border-radius: 3px;
            background-color: #2b2b2b;
        }
        QStatusBar {
            background-color: #2b2b2b;
            border-top: 1px solid #555;
        }
    )");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
