// UartTerminalView self-test (QTest). Covers the three append paths, RX/TX
// byte counters, the directed-protocol ring stub, clear()+signal, autoscroll
// and maxLines setters, and a render smoke (grab → PNG for review).
#include "QuarkWidgets/UartTerminalView.hpp"
#include "QuarkWidgets/Theme.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QObject>
#include <QPixmap>
#include <QSignalSpy>
#include <QString>
#include <QTest>
#include <QtGlobal>

class TestUartTerminalView : public QObject {
    Q_OBJECT
  private slots:
    void defaults() {
        quark::UartTerminalView t;
        QCOMPARE(t.rxBytes(), std::uint64_t{0});
        QCOMPARE(t.txBytes(), std::uint64_t{0});
        QCOMPARE(t.protoCount(), 0);
        QVERIFY(t.autoScroll());
        QVERIFY(t.maxLines() > 0);
    }

    void appendTextCountsUtf8Bytes() {
        quark::UartTerminalView t;
        t.appendText(QStringLiteral("abc"));          // 3 bytes
        QCOMPARE(t.rxBytes(), std::uint64_t{3});
        t.appendText(QStringLiteral("©"));            // U+00A9 = 2 UTF-8 bytes
        QCOMPARE(t.rxBytes(), std::uint64_t{5});
        QCOMPARE(t.txBytes(), std::uint64_t{0});
    }

    void appendByteCountsAndEscapes() {
        quark::UartTerminalView t;
        t.appendByte('A');                  // printable → rx +1
        t.appendByte('\n');                 // newline   → rx +1, passes through
        t.appendByte(0x1B);                 // ESC       → rx +1, renders "\x1B"
        QCOMPARE(t.rxBytes(), std::uint64_t{3});
        QCOMPARE(t.txBytes(), std::uint64_t{0});
    }

    void directedByteSplitsRxTx() {
        quark::UartTerminalView t;
        t.appendByte(/*tUs=*/100, 0x41, quark::Dir::Rx);   // 'A' RX
        t.appendByte(/*tUs=*/200, 0x42, quark::Dir::Tx);   // 'B' TX
        t.appendByte(/*tUs=*/300, 0x43, quark::Dir::Rx);   // 'C' RX
        QCOMPARE(t.rxBytes(), std::uint64_t{2});
        QCOMPARE(t.txBytes(), std::uint64_t{1});
        QCOMPARE(t.protoCount(), 3);
    }

    void protoRingCapsButCounterDoesNot() {
        quark::UartTerminalView t;
        t.setMaxLines(0);   // don't let line-trim muddy the test
        // Push well past the internal ring cap (4096) — counter keeps climbing.
        for (int i = 0; i < 5000; ++i)
            t.appendByte(/*tUs=*/i, 0x41, quark::Dir::Rx);
        QCOMPARE(t.protoCount(), 5000);
        QCOMPARE(t.rxBytes(), std::uint64_t{5000});
    }

    void clearResetsAndEmits() {
        quark::UartTerminalView t;
        t.appendText(QStringLiteral("data"));
        t.appendByte(1, 0x42, quark::Dir::Tx);
        QSignalSpy spy(&t, &quark::UartTerminalView::cleared);
        t.clear();
        QCOMPARE(t.rxBytes(), std::uint64_t{0});
        QCOMPARE(t.txBytes(), std::uint64_t{0});
        QCOMPARE(spy.count(), 1);
        // Terminal still usable after clear.
        t.appendByte('Z');
        QCOMPARE(t.rxBytes(), std::uint64_t{1});
    }

    void setAutoScrollReflectsCheckbox() {
        quark::UartTerminalView t;
        QVERIFY(t.autoScroll());
        t.setAutoScroll(false);
        QVERIFY(!t.autoScroll());
        t.setAutoScroll(true);
        QVERIFY(t.autoScroll());
    }

    void setMaxLinesClampsNonNegative() {
        quark::UartTerminalView t;
        t.setMaxLines(-5);
        QCOMPARE(t.maxLines(), 0);
        t.setMaxLines(2048);
        QCOMPARE(t.maxLines(), 2048);
    }

    void rendersSmoke() {
        quark::UartTerminalView t;
        t.appendText(QStringLiteral("UART terminal ready\r\n"));
        t.appendText(QStringLiteral("hello world 0x%1\r\n")
                         .arg(0xAB, 2, 16, QChar('0')).toUpper());
        t.appendByte(0x1B);
        t.appendByte(50, 0x06, quark::Dir::Tx);
        t.resize(520, 320);
        const QPixmap pix = t.grab();
        QVERIFY(!pix.isNull());
        QVERIFY(pix.width() > 0 && pix.height() > 0);
        const QString dir = QCoreApplication::applicationDirPath();
        QVERIFY(pix.save(dir + QDir::separator() + QStringLiteral("uartterminal.png")));
    }
};

QTEST_MAIN(TestUartTerminalView)
#include "test_uartterminalview.moc"
