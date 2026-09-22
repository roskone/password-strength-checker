#include "PasswordChecker.h"

#include <QRandomGenerator>
#include <QSet>
#include <algorithm>
#include <cmath>

namespace {

bool isLower(QChar c)   { return c.isLetter() && c.isLower(); }
bool isUpper(QChar c)   { return c.isLetter() && c.isUpper(); }
bool isDigit(QChar c)   { return c.isDigit(); }
bool isSpecial(QChar c) { return !c.isLetterOrNumber() && !c.isSpace(); }
bool isCyrillic(QChar c){ return c.unicode() >= 0x400 && c.unicode() <= 0x4FF; }

// Три одинаковых символа подряд: aaa, 111
bool hasRepeated(const QString& p) {
    for (int i = 2; i < p.size(); ++i)
        if (p[i] == p[i - 1] && p[i] == p[i - 2]) return true;
    return false;
}

// Последовательности вроде abc, 123, cba, 321
bool hasSequence(const QString& p) {
    for (int i = 2; i < p.size(); ++i) {
        ushort a = p[i - 2].toLower().unicode();
        ushort b = p[i - 1].toLower().unicode();
        ushort c = p[i].toLower().unicode();
        if ((b == a + 1 && c == b + 1) || (b + 1 == a && c + 1 == b)) return true;
    }
    return false;
}

bool isCommon(const QString& password) {
    static const QSet<QString> common = {
        "password", "123456", "12345678", "123456789", "qwerty", "abc123",
        "111111", "letmein", "admin", "welcome", "iloveyou", "monkey",
        "dragon", "123123", "qwerty123", "password1", "1234567890",
        "пароль", "йцукен", "привет", "любовь"
    };
    return common.contains(password.toLower());
}

// Примерное время перебора при 10 млрд попыток/с
QString crackTime(double bits) {
    if (bits <= 0) return "мгновенно";
    double seconds = std::pow(2.0, bits) / 1e10;
    if (seconds < 1) return "мгновенно";
    if (seconds > 3.15e7 * 1e12) return "практически вечность";

    struct Unit { double sec; const char* name; };
    static const Unit units[] = {
        {3.15e7 * 1e9, "млрд лет"}, {3.15e7 * 1e6, "млн лет"}, {3.15e7 * 1e3, "тыс. лет"},
        {3.15e7, "лет"}, {86400 * 30.0, "мес."}, {86400, "дн."}, {3600, "ч."}, {60, "мин."}, {1, "сек."}
    };
    for (const auto& u : units) {
        if (seconds >= u.sec) {
            double v = seconds / u.sec;
            return QString::number(v, 'f', v < 10 ? 1 : 0) + " " + u.name;
        }
    }
    return "мгновенно";
}

} // namespace

namespace PasswordChecker {

Analysis analyze(const QString& password) {
    Analysis a;
    a.length = password.size();

    bool lower = false, upper = false, digit = false, special = false, cyr = false;
    for (QChar c : password) {
        if (isLower(c))    lower = true;
        if (isUpper(c))    upper = true;
        if (isDigit(c))    digit = true;
        if (isSpecial(c))  special = true;
        if (isCyrillic(c)) cyr = true;
    }

    // Размер алфавита для оценки энтропии
    int pool = 0;
    if (lower)   pool += cyr ? 33 : 26;
    if (upper)   pool += cyr ? 33 : 26;
    if (digit)   pool += 10;
    if (special) pool += 32;
    if (pool > 0) a.entropyBits = a.length * std::log2(pool);

    int lenPts = a.length >= 16 ? 4 : a.length >= 12 ? 3 : a.length >= 8 ? 2 : 0;

    a.criteria = {
        {"Длина не менее 8 символов",            a.length >= 8,          "увеличьте длину хотя бы до 12 символов"},
        {"Строчные буквы (a-z, а-я)",            lower,                  "добавьте строчные буквы"},
        {"Заглавные буквы (A-Z, А-Я)",           upper,                  "добавьте заглавные буквы"},
        {"Цифры (0-9)",                          digit,                  "добавьте цифры"},
        {"Спецсимволы (!@#$%^&*…)",              special,                "добавьте спецсимволы (!@#$%^&*)"},
        {"Нет повторов (aaa, 111)",              !hasRepeated(password), "уберите повторяющиеся символы подряд"},
        {"Нет последовательностей (abc, 123)",   !hasSequence(password), "уберите последовательности вроде abc или 123"},
    };

    a.score = lenPts;
    for (int i = 1; i < a.criteria.size(); ++i)
        if (a.criteria[i].passed) a.score += 1;

    a.common = isCommon(password);
    if (a.common) { a.score = 0; a.entropyBits = 0; }
    a.score = std::clamp(a.score, 0, 10);

    if      (a.score <= 2) a.level = "Очень слабый";
    else if (a.score <= 4) a.level = "Слабый";
    else if (a.score <= 6) a.level = "Средний";
    else if (a.score <= 8) a.level = "Надёжный";
    else                   a.level = "Очень надёжный";

    a.crackTime = crackTime(a.entropyBits);
    return a;
}

QString generate(int length) {
    static const QString lower   = "abcdefghijkmnpqrstuvwxyz";   // без l — похожа на 1
    static const QString upper   = "ABCDEFGHJKLMNPQRSTUVWXYZ";   // без I и O
    static const QString digits  = "23456789";                   // без 0 и 1
    static const QString special = "!@#$%^&*-_=+?";
    static const QString all     = lower + upper + digits + special;

    auto* rng = QRandomGenerator::global();
    auto pick = [&](const QString& s) { return s[rng->bounded(s.size())]; };

    QString pw;
    do {
        pw.clear();
        pw += pick(lower); pw += pick(upper); pw += pick(digits); pw += pick(special);
        while (pw.size() < length) pw += pick(all);
        std::shuffle(pw.begin(), pw.end(), *rng);
    } while (hasRepeated(pw) || hasSequence(pw));
    return pw;
}

} // namespace PasswordChecker
