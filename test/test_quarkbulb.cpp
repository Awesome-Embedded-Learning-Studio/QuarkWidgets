// QuarkBulb self-test (QTest).
//
// Covers the API (defaults, state toggle, brightness clamping, color round-trip)
// plus a render smoke: grab the widget off/on and assert the lit state actually
// changes pixels (the glow + warmer glass must show up). The smoke also dumps
// the rendered PNGs next to the test binary (bulb_off.png, bulb_on.png, …) for
// visual review by humans / a vision model.
#include "QuarkWidgets/QuarkBulb.hpp"
#include "QuarkWidgets/Theme.hpp"

#include <QColor>
#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <QObject>
#include <QPixmap>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QTest>
#include <QtGlobal>

class TestQuarkBulb : public QObject {
    Q_OBJECT
  private slots:
    void defaults() {
        quark::QuarkBulb b;
        QCOMPARE(b.state(), false);
        QCOMPARE(b.brightness(), 1.0);
        QCOMPARE(b.color(), QColor(quark::Colors::kBulbWarm));
    }

    void stateToggle() {
        quark::QuarkBulb b;
        b.setState(true);  QVERIFY(b.state());
        b.setState(false); QVERIFY(!b.state());
        b.setState(false); QVERIFY(!b.state());  // idempotent
    }

    void brightnessClamped() {
        quark::QuarkBulb b;
        b.setBrightness(0.5);  QCOMPARE(b.brightness(), 0.5);
        b.setBrightness(5.0);  QCOMPARE(b.brightness(), 1.0);
        b.setBrightness(-1.0); QCOMPARE(b.brightness(), 0.0);
    }

    void colorRoundTrip() {
        quark::QuarkBulb b;
        b.setColor(Qt::red);
        QCOMPARE(b.color(), QColor(Qt::red));
    }

    void rendersDistinctOnOff() {
        quark::QuarkBulb b;
        b.setFixedSize(b.sizeHint());
        const QString dir = QCoreApplication::applicationDirPath();

        b.setState(false);
        const QPixmap off_pix = b.grab();
        QVERIFY(!off_pix.isNull());
        QVERIFY(off_pix.save(dir + QDir::separator() + QStringLiteral("bulb_off.png")));

        b.setState(true);
        const QPixmap on_pix = b.grab();
        QVERIFY(!on_pix.isNull());
        QVERIFY(on_pix.save(dir + QDir::separator() + QStringLiteral("bulb_on.png")));

        // Pixel diff: the glow + warmer glass must change a meaningful number of
        // pixels (guards against "lit state paints nothing").
        const QImage off_img = off_pix.toImage().convertToFormat(QImage::Format_ARGB32);
        const QImage on_img  = on_pix.toImage().convertToFormat(QImage::Format_ARGB32);
        QCOMPARE(on_img.size(), off_img.size());
        const int w = off_img.width();
        const int h = off_img.height();
        int diff = 0;
        for (int y = 0; y < h; ++y) {
            const auto* a = reinterpret_cast<const QRgb*>(off_img.constScanLine(y));
            const auto* c = reinterpret_cast<const QRgb*>(on_img.constScanLine(y));
            for (int x = 0; x < w; ++x) {
                if (a[x] != c[x]) ++diff;
            }
        }
        QVERIFY2(diff > 50,
                 qPrintable(QStringLiteral("on/off render differ in only %1 pixels; "
                                           "the lit state may not be painting its glow.")
                                .arg(diff)));

        // Extra variants dumped for visual review (not asserted beyond non-null).
        b.setBrightness(0.35);
        QVERIFY(b.grab().save(dir + QDir::separator() + QStringLiteral("bulb_on_dim.png")));
        b.setBrightness(1.0);
        b.setColor(Qt::green);
        QVERIFY(b.grab().save(dir + QDir::separator() + QStringLiteral("bulb_green.png")));
    }
};

QTEST_MAIN(TestQuarkBulb)
#include "test_quarkbulb.moc"
