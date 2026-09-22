#include "MainWindow.h"
#include "PasswordChecker.h"
#include "StrengthMeter.h"

#include <QApplication>
#include <QClipboard>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QVBoxLayout>

namespace {
// Палитра
const QString kBg      = "#151929";
const QString kSurface = "#1E2338";
const QString kBorder  = "#2C3350";
const QString kText    = "#E8EAF2";
const QString kMuted   = "#8A91AB";
const QString kAccent  = "#7C9BFF";
const QString kWeak    = "#F0647A";
const QString kMedium  = "#F2B24A";
const QString kStrong  = "#5CD48F";

QString colorForScore(int score) {
    if (score <= 4) return kWeak;
    if (score <= 6) return kMedium;
    return kStrong;
}
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Проверка надёжности пароля");
    setMinimumSize(520, 600);
    resize(560, 640);
    buildUi();
    applyStyles();
    showEmptyState();
}

void MainWindow::buildUi() {
    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(36, 32, 36, 28);
    root->setSpacing(0);

    // ── Заголовок ──
    auto* title = new QLabel("Проверка надёжности пароля");
    title->setObjectName("title");
    root->addWidget(title);
    root->addSpacing(20);

    // ── Поле ввода ──
    auto* inputRow = new QHBoxLayout();
    inputRow->setSpacing(8);
    m_passwordEdit = new QLineEdit();
    m_passwordEdit->setObjectName("passwordEdit");
    m_passwordEdit->setPlaceholderText("Введите пароль");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_eyeButton = new QPushButton("Показать");
    m_eyeButton->setObjectName("ghost");
    m_eyeButton->setCheckable(true);
    m_eyeButton->setCursor(Qt::PointingHandCursor);
    inputRow->addWidget(m_passwordEdit, 1);
    inputRow->addWidget(m_eyeButton);
    root->addLayout(inputRow);
    root->addSpacing(28);

    // ── Вердикт + шкала ──
    auto* verdictRow = new QHBoxLayout();
    m_levelLabel = new QLabel();
    m_levelLabel->setObjectName("level");
    m_scoreLabel = new QLabel();
    m_scoreLabel->setObjectName("score");
    m_scoreLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    verdictRow->addWidget(m_levelLabel);
    verdictRow->addStretch();
    verdictRow->addWidget(m_scoreLabel);
    root->addLayout(verdictRow);
    root->addSpacing(10);

    m_meter = new StrengthMeter();
    root->addWidget(m_meter);
    root->addSpacing(14);

    // ── Статистика в одну строку ──
    auto* statsRow = new QHBoxLayout();
    statsRow->setSpacing(24);
    m_entropyLabel = new QLabel();
    m_entropyLabel->setObjectName("stat");
    m_crackLabel = new QLabel();
    m_crackLabel->setObjectName("stat");
    statsRow->addWidget(m_entropyLabel);
    statsRow->addWidget(m_crackLabel);
    statsRow->addStretch();
    root->addLayout(statsRow);
    root->addSpacing(28);

    // ── Критерии ──
    auto* grid = new QGridLayout();
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(9);
    grid->setColumnStretch(1, 1);

    Analysis blank = PasswordChecker::analyze("");
    for (int i = 0; i < blank.criteria.size(); ++i) {
        auto* icon = new QLabel();
        icon->setObjectName("icon");
        icon->setFixedWidth(16);
        icon->setAlignment(Qt::AlignCenter);
        auto* text = new QLabel(blank.criteria[i].name);
        grid->addWidget(icon, i, 0);
        grid->addWidget(text, i, 1);
        m_criteriaIcons.push_back(icon);
        m_criteriaTexts.push_back(text);
    }
    m_commonLabel = new QLabel("Этот пароль — один из самых распространённых в мире");
    m_commonLabel->setObjectName("common");
    m_commonLabel->hide();
    grid->addWidget(m_commonLabel, blank.criteria.size(), 0, 1, 2);
    root->addLayout(grid);
    root->addSpacing(20);

    // ── Совет ──
    m_tipLabel = new QLabel();
    m_tipLabel->setObjectName("tip");
    m_tipLabel->setWordWrap(true);
    root->addWidget(m_tipLabel);

    root->addStretch();

    // ── Генератор ──
    auto* line = new QFrame();
    line->setObjectName("separator");
    line->setFixedHeight(1);
    root->addWidget(line);
    root->addSpacing(18);

    auto* genRow = new QHBoxLayout();
    genRow->setSpacing(10);
    auto* genCaption = new QLabel("Длина");
    genCaption->setObjectName("muted");
    m_lengthSpin = new QSpinBox();
    m_lengthSpin->setRange(12, 64);
    m_lengthSpin->setValue(16);
    m_lengthSpin->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    m_toast = new QLabel();
    m_toast->setObjectName("toast");
    m_generateBtn = new QPushButton("Сгенерировать");
    m_generateBtn->setObjectName("primary");
    m_generateBtn->setCursor(Qt::PointingHandCursor);
    m_copyBtn = new QPushButton("Скопировать");
    m_copyBtn->setCursor(Qt::PointingHandCursor);
    genRow->addWidget(genCaption);
    genRow->addWidget(m_lengthSpin);
    genRow->addSpacing(8);
    genRow->addWidget(m_toast);
    genRow->addStretch();
    genRow->addWidget(m_generateBtn);
    genRow->addWidget(m_copyBtn);
    root->addLayout(genRow);

    setCentralWidget(central);

    m_toastTimer = new QTimer(this);
    m_toastTimer->setSingleShot(true);
    connect(m_toastTimer, &QTimer::timeout, m_toast, &QLabel::clear);

    connect(m_passwordEdit, &QLineEdit::textChanged, this, &MainWindow::onPasswordChanged);
    connect(m_eyeButton,    &QPushButton::toggled,   this, &MainWindow::onToggleVisibility);
    connect(m_generateBtn,  &QPushButton::clicked,   this, &MainWindow::onGenerate);
    connect(m_copyBtn,      &QPushButton::clicked,   this, &MainWindow::onCopy);

    m_passwordEdit->setFocus();
}

void MainWindow::applyStyles() {
    setStyleSheet(QString(R"(
        QWidget { background: %1; color: %4; font-size: 14px; }
        #title { font-size: 20px; font-weight: 600; letter-spacing: -0.2px; }

        #passwordEdit {
            font-family: "Menlo", "Consolas", "DejaVu Sans Mono", monospace;
            font-size: 17px; padding: 11px 14px;
            background: %2; border: 1px solid %3; border-radius: 10px;
            selection-background-color: %6; selection-color: %1;
        }
        #passwordEdit:focus { border: 1px solid %6; }

        #level { font-size: 30px; font-weight: 600; letter-spacing: -0.5px; }
        #score { font-size: 15px; color: %5; padding-bottom: 5px; }
        #stat  { font-size: 14px; color: %5; }
        #stat b { color: %4; font-weight: 500; }
        #icon  { font-size: 15px; font-weight: 700; }
        #common { color: %7; padding-top: 6px; }
        #tip {
            color: %4; padding: 8px 0 8px 14px;
            border-left: 3px solid %6; border-radius: 0;
        }
        #separator { background: %3; }
        #muted { color: %5; }
        #toast { color: %5; font-size: 13px; }

        QSpinBox {
            padding: 6px 8px; background: %2; border: 1px solid %3; border-radius: 8px;
            selection-background-color: %6; selection-color: %1;
        }

        QPushButton {
            padding: 9px 16px; background: %2; color: %4;
            border: 1px solid %3; border-radius: 9px; font-weight: 500;
        }
        QPushButton:hover { border-color: %5; }
        QPushButton:pressed { background: %3; }
        QPushButton:disabled { color: %3; }
        QPushButton:focus { border-color: %6; outline: none; }

        #ghost { background: transparent; }
        #ghost:checked { color: %6; border-color: %6; }

        #primary { background: %6; color: %1; border: 1px solid %6; font-weight: 600; }
        #primary:hover { background: #93ADFF; border-color: #93ADFF; }
        #primary:pressed { background: #6A89EE; }
    )")
        .arg(kBg, kSurface, kBorder, kText, kMuted, kAccent, kWeak));
}

void MainWindow::showEmptyState() {
    m_meter->setScore(0, kBorder);
    m_levelLabel->setText("Пока пусто");
    m_levelLabel->setStyleSheet("color: " + kMuted + ";");
    m_scoreLabel->clear();
    m_entropyLabel->setText("Оценка появится по мере ввода");
    m_crackLabel->clear();
    m_tipLabel->hide();
    m_commonLabel->hide();
    for (int i = 0; i < m_criteriaIcons.size(); ++i) {
        m_criteriaIcons[i]->setText("·");
        m_criteriaIcons[i]->setStyleSheet("color: " + kMuted + ";");
        m_criteriaTexts[i]->setStyleSheet("color: " + kMuted + ";");
    }
    m_copyBtn->setEnabled(false);
}

void MainWindow::onPasswordChanged(const QString& text) {
    if (text.isEmpty()) { showEmptyState(); return; }

    Analysis a = PasswordChecker::analyze(text);
    const QString col = colorForScore(a.score);

    m_meter->setScore(a.score, col);
    m_levelLabel->setText(a.level);
    m_levelLabel->setStyleSheet("color: " + col + ";");
    m_scoreLabel->setText(QString("%1 из 10").arg(a.score));

    for (int i = 0; i < a.criteria.size(); ++i) {
        const bool ok = a.criteria[i].passed;
        m_criteriaIcons[i]->setText(ok ? "✓" : "✕");
        m_criteriaIcons[i]->setStyleSheet("color: " + (ok ? kStrong : kWeak) + ";");
        m_criteriaTexts[i]->setStyleSheet("color: " + (ok ? kText : kMuted) + ";");
    }
    m_commonLabel->setVisible(a.common);

    m_entropyLabel->setText(QString("<b>%1 бит</b> энтропии").arg(QString::number(a.entropyBits, 'f', 0)));
    m_crackLabel->setText(QString("подбор за <b>%1</b>").arg(a.crackTime));

    QString tip;
    if (a.common) tip = "Выберите уникальный пароль, а не словарное слово.";
    else if (a.length < 12) tip = "Увеличьте длину хотя бы до 12 символов.";
    else for (const auto& c : a.criteria) if (!c.passed) { tip = c.tip; tip[0] = tip[0].toUpper(); tip += "."; break; }

    if (tip.isEmpty()) m_tipLabel->hide();
    else { m_tipLabel->setText(tip); m_tipLabel->show(); }

    m_copyBtn->setEnabled(true);
}

void MainWindow::onToggleVisibility() {
    const bool show = m_eyeButton->isChecked();
    m_passwordEdit->setEchoMode(show ? QLineEdit::Normal : QLineEdit::Password);
    m_eyeButton->setText(show ? "Скрыть" : "Показать");
}

void MainWindow::onGenerate() {
    m_passwordEdit->setText(PasswordChecker::generate(m_lengthSpin->value()));
    m_eyeButton->setChecked(true);
    showToast("Сгенерирован");
}

void MainWindow::onCopy() {
    QApplication::clipboard()->setText(m_passwordEdit->text());
    showToast("Скопировано");
}

void MainWindow::showToast(const QString& text) {
    m_toast->setText(text);
    m_toastTimer->start(2000);
}
