#pragma once

#include <QString>
#include <QVector>

// Один критерий оценки пароля
struct Criterion {
    QString name;   // что проверяем
    bool passed;    // выполнен ли
    QString tip;    // совет, если не выполнен
};

// Полный результат анализа пароля
struct Analysis {
    QVector<Criterion> criteria;
    int score = 0;              // 0..10
    double entropyBits = 0.0;   // оценка энтропии
    int length = 0;             // длина в символах
    bool common = false;        // пароль из списка популярных
    QString level;              // словесная оценка
    QString crackTime;          // примерное время подбора
};

namespace PasswordChecker {
    Analysis analyze(const QString& password);
    QString generate(int length);
}
