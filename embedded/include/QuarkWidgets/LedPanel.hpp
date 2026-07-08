// LedPanel — a row/column of QuarkBulbs driven by a bitmask, with pin labels
// and an ODR hex readout. Composes QuarkBulb children (no custom paintEvent —
// the bulbs self-draw, this widget just arranges them).
//
// Static panel: reflects the latest setLevels() only. appendEdge() is a
// forward-compat hook for a future LedTimingView (a logic-analyser-style trace)
// — edges are RECORDED into a ring buffer but NOT rendered here. The static
// panel intentionally ignores them so setLevels() stays the single source of
// what's lit right now.
#pragma once

#include "QuarkWidgets/QuarkBulb.hpp"

#include <QColor>
#include <QList>
#include <QSize>
#include <QString>
#include <QWidget>

class QGridLayout;
class QLabel;

namespace quark {

class LedPanel : public QWidget {
    Q_OBJECT
  public:
    explicit LedPanel(int count = 8, Qt::Orientation orient = Qt::Vertical,
                      QWidget* parent = nullptr);

    // Drive every LED from a bitmask (bit i = pin i driven high). Repaints.
    void setLevels(quint16 bits);
    [[nodiscard]] quint16 levels() const noexcept { return bits_; }

    // Per-panel tint applied to all bulbs (default warm amber).
    void setColor(QColor tint);
    [[nodiscard]] QColor color() const noexcept { return tint_; }

    // Pin-label prefix (default "PA" → PA0..PA{n-1}).
    void setLabelPrefix(const QString& prefix);
    [[nodiscard]] QString labelPrefix() const noexcept { return prefix_; }

    // LED count (rebuilds the layout). Clamped to [0, kMaxCount].
    void setCount(int count);
    [[nodiscard]] int count() const noexcept { return count_; }

    void setOrientation(Qt::Orientation orient);
    [[nodiscard]] Qt::Orientation orientation() const noexcept { return orient_; }

    // Show/hide the ODR hex readout label.
    void setShowHex(bool show);
    [[nodiscard]] bool showHex() const noexcept { return showHex_; }

    // Force a fixed size on each bulb — for dense layouts (e.g. a board view
    // with many pins). Default invalid → bulbs use QuarkBulb::sizeHint().
    void setBulbSize(QSize size);
    [[nodiscard]] QSize bulbSize() const noexcept { return bulbSize_; }

    // Forward-compat timing hook — see file header.
    void appendEdge(qint64 tUs, int pin, bool hi);
    [[nodiscard]] int edgeCount() const noexcept { return totalEdges_; }

    // Read access to the i-th bulb (nullptr if out of range). For tests and for
    // per-bulb tweaks (e.g. dimming one pin) by advanced consumers.
    [[nodiscard]] QuarkBulb* bulb(int i) const;

    static constexpr int kMaxCount = 16;   // a GPIO port is 16 pins

  private:
    struct Edge { qint64 tUs; int pin; bool hi; };

    void rebuild();
    void refreshBulbs();
    void refreshHex();

    int count_ = 8;
    Qt::Orientation orient_ = Qt::Vertical;
    quint16 bits_ = 0;
    QColor tint_ {Colors::kBulbWarm};
    QString prefix_ {QStringLiteral("PA")};
    QSize bulbSize_;   // invalid → QuarkBulb::sizeHint()
    bool showHex_ = true;

    QList<QuarkBulb*> bulbs_;
    QList<QLabel*> pinLabels_;
    QLabel* hexLabel_ = nullptr;
    QGridLayout* grid_ = nullptr;

    QList<Edge> edges_;   // capped ring (see kEdgeCap in the .cpp)
    int totalEdges_ = 0;
};

}  // namespace quark
