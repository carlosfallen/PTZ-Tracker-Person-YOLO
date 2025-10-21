#include "VideoWidget.h"

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {
    setMinimumSize(640, 480);
    setStyleSheet("background-color: #000;");
}

void VideoWidget::setFrame(const QImage &frame) {
    currentFrame = frame;
    
    // Escala mantendo aspect ratio
    if (!currentFrame.isNull()) {
        scaledFrame = currentFrame.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        // Calcula posição centralizada
        int x = (width() - scaledFrame.width()) / 2;
        int y = (height() - scaledFrame.height()) / 2;
        displayRect = QRect(x, y, scaledFrame.width(), scaledFrame.height());
    }
    
    update();
}

void VideoWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Background preto
    painter.fillRect(rect(), Qt::black);
    
    if (scaledFrame.isNull()) {
        // Mensagem de aguardando
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setPointSize(14);
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, "⏳ Aguardando vídeo...");
        
        // Instrução
        font.setPointSize(10);
        painter.setFont(font);
        painter.setPen(QColor(150, 150, 150));
        painter.drawText(rect().adjusted(0, 50, 0, 0), Qt::AlignCenter, 
            "Clique em 'Iniciar Detecção' para começar");
    } else {
        // Desenha frame
        painter.drawImage(displayRect, scaledFrame);
        
        // Borda do vídeo
        painter.setPen(QPen(QColor(42, 130, 218), 2));
        painter.drawRect(displayRect);
    }
}

void VideoWidget::mousePressEvent(QMouseEvent *event) {
    if (displayRect.contains(event->pos())) {
        // Converte coordenadas da tela para coordenadas do frame original
        QPoint relativePos = event->pos() - displayRect.topLeft();
        float scaleX = (float)currentFrame.width() / scaledFrame.width();
        float scaleY = (float)currentFrame.height() / scaledFrame.height();
        
        QPoint framePos(relativePos.x() * scaleX, relativePos.y() * scaleY);
        emit clicked(framePos);
    }
}
