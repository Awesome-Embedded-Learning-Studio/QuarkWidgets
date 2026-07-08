// Minimal LedPanel app — one 8-LED panel, ODR counter cycling 0x00→0xFF.
// Standalone build:
//   cmake -B build -S . && cmake --build build -j$(nproc) && ./build/examples/ledpanel_example
#include "QuarkWidgets/LedPanel.hpp"

#include <QApplication>
#include <QChar>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QWidget win;
    win.setWindowTitle(QStringLiteral("LedPanel — cycling ODR"));

    auto* panel = new quark::LedPanel(8, Qt::Horizontal);
    auto* status = new QLabel;
    status->setAlignment(Qt::AlignCenter);

    auto* lay = new QVBoxLayout(&win);
    lay->addWidget(panel, 1, Qt::AlignCenter);
    lay->addWidget(status);

    quint16 counter = 0;
    QTimer timer(&win);
    QObject::connect(&timer, &QTimer::timeout, &win, [panel, status, counter]() mutable {
        counter = (counter + 1) & 0xFF;
        panel->setLevels(counter);
        status->setText(QStringLiteral("ODR = 0x%1")
                            .arg(counter, 2, 16, QChar('0')).toUpper());
    });
    timer.start(300);

    win.show();
    return app.exec();
}
