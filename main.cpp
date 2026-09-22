#include "MainWindow.h"

#include <QApplication>
#include <QFont>
#include <QPalette>
#include <QStyleFactory>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Проверка надёжности пароля");

    // Fusion одинаково рисует виджеты на всех ОС и полностью подчиняется стилям
    app.setStyle(QStyleFactory::create("Fusion"));

    // Тёмная палитра — для элементов, которые Fusion рисует сам (стрелки, подсказки)
    QPalette pal;
    pal.setColor(QPalette::Window,          QColor("#151929"));
    pal.setColor(QPalette::Base,            QColor("#1E2338"));
    pal.setColor(QPalette::Button,          QColor("#1E2338"));
    pal.setColor(QPalette::WindowText,      QColor("#E8EAF2"));
    pal.setColor(QPalette::Text,            QColor("#E8EAF2"));
    pal.setColor(QPalette::ButtonText,      QColor("#E8EAF2"));
    pal.setColor(QPalette::PlaceholderText, QColor("#8A91AB"));
    pal.setColor(QPalette::Highlight,       QColor("#7C9BFF"));
    pal.setColor(QPalette::HighlightedText, QColor("#151929"));
    pal.setColor(QPalette::ToolTipBase,     QColor("#1E2338"));
    pal.setColor(QPalette::ToolTipText,     QColor("#E8EAF2"));
    app.setPalette(pal);

    QFont font;
#if defined(Q_OS_MACOS)
    font.setFamilies({"Avenir Next", "Helvetica Neue"});
#elif defined(Q_OS_WIN)
    font.setFamilies({"Segoe UI", "Arial"});
#else
    font.setFamilies({"Inter", "Noto Sans", "DejaVu Sans"});
#endif
    font.setPointSize(13);
    app.setFont(font);

    MainWindow window;
    window.show();

    return app.exec();
}
