#pragma once

#include <QMainWindow>
#include <QVector>

class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTimer;
class StrengthMeter;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onPasswordChanged(const QString& text);
    void onToggleVisibility();
    void onGenerate();
    void onCopy();

private:
    void buildUi();
    void applyStyles();
    void showEmptyState();
    void showToast(const QString& text);

    QLineEdit*     m_passwordEdit = nullptr;
    QPushButton*   m_eyeButton    = nullptr;
    QLabel*        m_levelLabel   = nullptr;
    QLabel*        m_scoreLabel   = nullptr;
    StrengthMeter* m_meter        = nullptr;
    QLabel*        m_entropyLabel = nullptr;
    QLabel*        m_crackLabel   = nullptr;
    QLabel*        m_commonLabel  = nullptr;
    QLabel*        m_tipLabel     = nullptr;
    QVector<QLabel*> m_criteriaIcons;
    QVector<QLabel*> m_criteriaTexts;
    QSpinBox*      m_lengthSpin   = nullptr;
    QPushButton*   m_generateBtn  = nullptr;
    QPushButton*   m_copyBtn      = nullptr;
    QLabel*        m_toast        = nullptr;
    QTimer*        m_toastTimer   = nullptr;
};
