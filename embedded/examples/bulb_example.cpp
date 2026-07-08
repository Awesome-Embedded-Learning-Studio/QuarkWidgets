// Minimal QuarkBulb app — one bulb auto-toggling every 600 ms so you see both
// states without interacting. Standalone build:
//   cmake -B build -S . && cmake --build build -j$(nproc) && ./build/examples/bulb_example
#include "QuarkWidgets/QuarkBulb.hpp"

#include <QApplication>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QWidget win;
    win.setWindowTitle(QStringLiteral("QuarkBulb — auto toggle"));

    auto* bulb  = new quark::QuarkBulb;
    auto* label = new QLabel(QStringLiteral("OFF"));
    label->setAlignment(Qt::AlignCenter);

    auto* lay = new QVBoxLayout(&win);
    lay->addWidget(bulb, /*stretch=*/1, Qt::AlignCenter);
    lay->addWidget(label);

    QTimer timer(&win);
    QObject::connect(&timer, &QTimer::timeout, &win, [bulb, label]() {
        const bool on = !bulb->state();
        bulb->setState(on);
        label->setText(on ? QStringLiteral("ON") : QStringLiteral("OFF"));
    });
    timer.start(600);

    win.show();
    return app.exec();
}
