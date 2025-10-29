#include "VideoWidget.h"

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(640, 480);
    setStyleSheet("background-color: #000;");
}

void VideoWidget::setFrame(const QImage &frame) {
    qDebug() << "Frame recebido:" << frame.size() << "isNull:" << frame.isNull();
    currentFrame = frame.copy();
    
    if (!currentFrame.isNull()) {
        scaledFrame = currentFrame.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = (width() - scaledFrame.width()) / 2;
        int y = (height() - scaledFrame.height()) / 2;
        displayRect = QRect(x, y, scaledFrame.width(), scaledFrame.height());
    }
    
    update();
}

void VideoWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), Qt::black);
    
    if (scaledFrame.isNull()) {
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setPointSize(14);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, "⏳ Aguardando vídeo...");
        
        font.setPointSize(10);
        painter.setFont(font);
        painter.setPen(QColor(150, 150, 150));
        painter.drawText(rect().adjusted(0, 50, 0, 0), Qt::AlignCenter, 
            "Clique em 'Iniciar Detecção' para começar");
    } else {
        painter.drawImage(displayRect, scaledFrame);
        painter.setPen(QPen(QColor(42, 130, 218), 2));
        painter.drawRect(displayRect);
    }
}

void VideoWidget::mousePressEvent(QMouseEvent *event) {
    if (displayRect.contains(event->pos())) {
        QPoint relativePos = event->pos() - displayRect.topLeft();
        float scaleX = (float)currentFrame.width() / scaledFrame.width();
        float scaleY = (float)currentFrame.height() / scaledFrame.height();
        
        QPoint framePos(relativePos.x() * scaleX, relativePos.y() * scaleY);
        emit clicked(framePos);
    }
}