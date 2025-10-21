#include "LogPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QScrollBar>

LogPanel::LogPanel(QWidget *parent) : QWidget(parent), logCount(0) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);
    
    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *title = new QLabel("📋 Logs do Sistema");
    title->setStyleSheet("font-weight: bold; font-size: 12px;");
    
    clearBtn = new QPushButton("🗑 Limpar");
    clearBtn->setMaximumWidth(80);
    connect(clearBtn, &QPushButton::clicked, this, &LogPanel::clear);
    
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(clearBtn);
    
    layout->addLayout(headerLayout);
    
    // Log text area
    logText = new QPlainTextEdit();
    logText->setReadOnly(true);
    logText->setMaximumBlockCount(500);
    logText->setStyleSheet(R"(
        QPlainTextEdit {
            background-color: #1a1a1a;
            color: #d4d4d4;
            font-family: 'Consolas', 'Courier New', monospace;
            font-size: 11px;
            border: 1px solid #555;
            border-radius: 3px;
            padding: 5px;
        }
    )");
    
    layout->addWidget(logText);
}

void LogPanel::addLog(const QString &msg, int level) {
    QString color;
    QString icon;
    
    switch (level) {
        case 0:  // Info
            color = "#888";
            icon = "ℹ";
            break;
        case 1:  // Success
            color = "#4CAF50";
            icon = "✓";
            break;
        case 2:  // Error
            color = "#f44336";
            icon = "✗";
            break;
        case 3:  // Warning
            color = "#FF9800";
            icon = "⚠";
            break;
        default:
            color = "#888";
            icon = "•";
    }
    
    QString time = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    QString html = QString("<span style='color:%1'>[%2] %3 %4</span>")
        .arg(color, time, icon, msg.toHtmlEscaped());
    
    logText->appendHtml(html);
    
    // Auto-scroll
    QScrollBar *sb = logText->verticalScrollBar();
    sb->setValue(sb->maximum());
    
    logCount++;
}

void LogPanel::clear() {
    logText->clear();
    logCount = 0;
    addLog("Logs limpos", 0);
}
