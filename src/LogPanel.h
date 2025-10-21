#ifndef LOGPANEL_H
#define LOGPANEL_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>

class LogPanel : public QWidget {
    Q_OBJECT

public:
    explicit LogPanel(QWidget *parent = nullptr);
    void addLog(const QString &msg, int level);
    void clear();

private:
    QPlainTextEdit *logText;
    QPushButton *clearBtn;
    int logCount;
};

#endif
