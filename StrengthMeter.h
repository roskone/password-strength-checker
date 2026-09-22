#pragma once

#include <QColor>
#include <QWidget>

// Сегментированная шкала надёжности: 10 блоков, заполнение анимируется
class StrengthMeter : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double progress READ progress WRITE setProgress)

public:
    explicit StrengthMeter(QWidget* parent = nullptr);

    void setScore(int score, const QColor& color);   // 0..10, с анимацией
    double progress() const { return m_progress; }
    void setProgress(double p);

    QSize sizeHint() const override { return {200, 14}; }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    double m_progress = 0.0;   // 0..10, дробное во время анимации
    QColor m_color;
    QColor m_emptyColor;
};
