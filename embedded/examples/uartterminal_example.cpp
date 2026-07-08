// Minimal UartTerminalView app — simulates a UART stream: a periodic RX line
// plus the occasional directed TX byte, to show counters + autoscroll + Clear.
// Standalone build:
//   cmake -B build -S . && cmake --build build -j$(nproc) && ./build/examples/uartterminal_example
#include "QuarkWidgets/UartTerminalView.hpp"

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QWidget win;
    win.setWindowTitle(QStringLiteral("UartTerminalView — simulated stream"));
    win.resize(560, 360);

    auto* term = new quark::UartTerminalView;

    auto* inject = new QPushButton(QStringLiteral("Inject binary byte 0x1B"));
    auto* status = new QLabel;
    status->setFont(QFont(QStringLiteral("Monospace")));

    auto* lay = new QVBoxLayout(&win);
    lay->addWidget(term);
    lay->addWidget(inject);
    lay->addWidget(status);

    // Seed a header, then drip an RX line every 400ms.
    term->appendText(QStringLiteral("UART terminal ready\r\n"));

    int tick = 0;
    QTimer timer(&win);
    QObject::connect(&timer, &QTimer::timeout, &win, [term, status, &tick]() {
        ++tick;
        term->appendText(QStringLiteral("hello #%1\r\n").arg(tick));
        // Every 5th tick, also send a directed TX ack byte.
        if (tick % 5 == 0)
            term->appendByte(/*tUs=*/tick * 1000, 0x06, quark::Dir::Tx); // ACK
        status->setText(QStringLiteral("RX=%1  TX=%2  proto=%3")
                            .arg(term->rxBytes())
                            .arg(term->txBytes())
                            .arg(term->protoCount()));
    });
    timer.start(400);

    QObject::connect(inject, &QPushButton::clicked, &win, [term, status] {
        term->appendByte(0x1B);   // ESC → renders as "\x1B" (non-printable demo)
        status->setText(QStringLiteral("RX=%1  TX=%2  proto=%3")
                            .arg(term->rxBytes())
                            .arg(term->txBytes())
                            .arg(term->protoCount()));
    });

    win.show();
    return app.exec();
}
