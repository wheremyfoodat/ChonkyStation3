#pragma once

#include <QtWidgets>


class DisabledWidgetOverlay : public QWidget {
    Q_OBJECT

public:
    DisabledWidgetOverlay(QWidget *parent = nullptr, std::string overlay_text = "This widget is disabled") : QWidget(parent) {
        setVisible(false);
        text = QString::fromStdString(overlay_text);
    }

private:
    QString text;

    void paintEvent(QPaintEvent *) override {
        QPainter painter = QPainter(this);
        painter.fillRect(rect(), QColor(60, 60, 60, 128));
        painter.setPen(Qt::gray);
        
        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(18);
        
        painter.setFont(font);
        painter.drawText(rect(), Qt::AlignCenter, text);
    }
};
