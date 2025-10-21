#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>

class VideoWidget : public QWidget {
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = nullptr);
    void setFrame(const QImage &frame);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void clicked(QPoint pos);

private:
    QImage currentFrame;
    QImage scaledFrame;
    QRect displayRect;
};

#endif
