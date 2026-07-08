// LedPanel self-test (QTest). Covers the bitmask→bulb mapping, layout rebuilds,
// the timing stub, and a render smoke (grabs the panel to a PNG for review).
#include "QuarkWidgets/LedPanel.hpp"
#include "QuarkWidgets/QuarkBulb.hpp"
#include "QuarkWidgets/Theme.hpp"

#include <QColor>
#include <QCoreApplication>
#include <QDir>
#include <QObject>
#include <QPixmap>
#include <QString>
#include <QTest>
#include <QtGlobal>

class TestLedPanel : public QObject {
    Q_OBJECT
  private slots:
    void defaults() {
        quark::LedPanel p(8, Qt::Vertical);
        QCOMPARE(p.count(), 8);
        QCOMPARE(p.levels(), 0);
        QCOMPARE(p.color(), QColor(quark::Colors::kBulbWarm));
        QCOMPARE(p.labelPrefix(), QStringLiteral("PA"));
        QVERIFY(p.showHex());
        QCOMPARE(p.edgeCount(), 0);
        QCOMPARE(p.orientation(), Qt::Vertical);
    }

    void setLevelsDrivesBulbs() {
        quark::LedPanel p(8);
        const quint16 bits = 0b10100101;   // 0xA5
        p.setLevels(bits);
        QCOMPARE(p.levels(), bits);
        for (int i = 0; i < 8; ++i) {
            const bool expected = static_cast<bool>((bits >> i) & 1u);
            QCOMPARE(p.bulb(i)->state(), expected);
        }
    }

    void setLevelsIdempotent() {
        quark::LedPanel p(4);
        p.setLevels(0x0F);
        QCOMPARE(p.levels(), quint16(0x0F));
        p.setLevels(0x0F);   // no change
        QCOMPARE(p.levels(), quint16(0x0F));
    }

    void countClampedAndRebuilds() {
        quark::LedPanel p(8);
        p.setCount(99);
        QCOMPARE(p.count(), quark::LedPanel::kMaxCount);
        p.setCount(-5);
        QCOMPARE(p.count(), 0);
        p.setCount(4);
        QCOMPARE(p.count(), 4);
        QVERIFY(p.bulb(0) != nullptr);
        QVERIFY(p.bulb(3) != nullptr);
        QVERIFY(p.bulb(4) == nullptr);
    }

    void colorAppliedToAllBulbs() {
        quark::LedPanel p(4);
        p.setColor(Qt::red);
        QCOMPARE(p.color(), QColor(Qt::red));
        for (int i = 0; i < 4; ++i)
            QCOMPARE(p.bulb(i)->color(), QColor(Qt::red));
    }

    void appendEdgeRecordsButDoesNotChangeLevels() {
        quark::LedPanel p(8);
        p.setLevels(0x0F);
        p.appendEdge(1000, 4, true);
        p.appendEdge(2000, 4, false);
        p.appendEdge(3000, 0, true);
        QCOMPARE(p.edgeCount(), 3);
        // The static panel must NOT apply timing edges to current levels.
        QCOMPARE(p.levels(), quint16(0x0F));
    }

    void orientationSwitchKeepsBulbs() {
        quark::LedPanel p(4, Qt::Vertical);
        p.setLevels(0b0101);
        QVERIFY(p.bulb(0) != nullptr);
        QCOMPARE(p.bulb(0)->state(), true);
        p.setOrientation(Qt::Horizontal);
        QCOMPARE(p.orientation(), Qt::Horizontal);
        QVERIFY(p.bulb(0) != nullptr);
        // Levels survive the rebuild.
        QCOMPARE(p.levels(), quint16(0b0101));
        QCOMPARE(p.bulb(0)->state(), true);
    }

    void rendersSmoke() {
        quark::LedPanel p(8, Qt::Horizontal);
        p.setLevels(0xAA);
        p.resize(p.sizeHint());
        const QPixmap pix = p.grab();
        QVERIFY(!pix.isNull());
        QVERIFY(pix.width() > 0 && pix.height() > 0);
        const QString dir = QCoreApplication::applicationDirPath();
        QVERIFY(pix.save(dir + QDir::separator() + QStringLiteral("ledpanel.png")));
    }
};

QTEST_MAIN(TestLedPanel)
#include "test_ledpanel.moc"
