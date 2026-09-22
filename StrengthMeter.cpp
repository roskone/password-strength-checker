#include "StrengthMeter.h"

#include <QPainter>
#include <QPropertyAnimation>

StrengthMeter::StrengthMeter(QWidget* parent)
    : QWidget(parent), m_color("#2C3350"), m_emptyColor("#2C3350") {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(14);
}

void StrengthMeter::setScore(int score, const QColor& color) {
    m_color = color;
    auto* anim = new QPropertyAnimation(this, "progress", this);
    anim->setDuration(260);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->setStartValue(m_progress);
    anim->setEndValue(static_cast<double>(score));
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void StrengthMeter::setProgress(double p) {
    m_progress = p;
    update();
}

void StrengthMeter::paintEvent(QPaintEvent*) {
    QPainter g(this);
    g.setRenderHint(QPainter::Antialiasing);
    g.setPen(Qt::NoPen);

    const int segments = 10;
    const double gap = 5.0;
    const double w = (width() - gap * (segments - 1)) / segments;
    const double h = height();

    for (int i = 0; i < segments; ++i) {
        // Доля заполнения этого сегмента: 1 — полный, 0 — пустой, между — частичный
        double fill = std::clamp(m_progress - i, 0.0, 1.0);
        QRectF r(i * (w + gap), 0, w, h);

        g.setBrush(m_emptyColor);
        g.drawRoundedRect(r, 3, 3);

        if (fill > 0) {
            QRectF f(r.left(), r.top(), r.width() * fill, r.height());
            g.setBrush(m_color);
            g.drawRoundedRect(f, 3, 3);
        }
    }
}
