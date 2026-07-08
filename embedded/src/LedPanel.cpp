// LedPanel — see LedPanel.hpp.
#include "QuarkWidgets/LedPanel.hpp"

#include "QuarkWidgets/Theme.hpp"

#include <QChar>
#include <QGridLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QString>
#include <QVBoxLayout>
#include <Qt>
#include <QtGlobal>

namespace quark {

namespace {
constexpr int kEdgeCap = 1024;   // ring cap for appendEdge() history
}

LedPanel::LedPanel(int count, Qt::Orientation orient, QWidget* parent)
    : QWidget(parent) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(Metrics::kPad, Metrics::kPad, Metrics::kPad, Metrics::kPad);
    outer->setSpacing(Metrics::kPad);

    hexLabel_ = new QLabel(this);
    hexLabel_->setFont(monoFont(10));
    outer->addWidget(hexLabel_);

    grid_ = new QGridLayout();
    grid_->setSpacing(Metrics::kPad);
    outer->addLayout(grid_);

    orient_ = orient;
    setCount(count);   // rebuilds + refreshHex
}

void LedPanel::setLevels(quint16 bits) {
    if (bits == bits_) return;
    bits_ = bits;
    refreshBulbs();
    refreshHex();
}

void LedPanel::setColor(QColor tint) {
    tint_ = tint;
    for (auto* b : bulbs_) b->setColor(tint);
}

void LedPanel::setLabelPrefix(const QString& prefix) {
    prefix_ = prefix;
    for (int i = 0; i < pinLabels_.size() && i < count_; ++i)
        pinLabels_[i]->setText(prefix_ + QString::number(i));
}

void LedPanel::setCount(int count) {
    count_ = qBound(0, count, kMaxCount);
    rebuild();
    refreshHex();
}

void LedPanel::setOrientation(Qt::Orientation orient) {
    if (orient == orient_) return;
    orient_ = orient;
    rebuild();
}

void LedPanel::setShowHex(bool show) {
    showHex_ = show;
    refreshHex();
}

void LedPanel::setBulbSize(QSize size) {
    bulbSize_ = size;
    if (!bulbSize_.isValid()) return;
    for (auto* b : bulbs_) b->setFixedSize(bulbSize_);
}

void LedPanel::appendEdge(qint64 tUs, int pin, bool hi) {
    edges_.append(Edge{tUs, pin, hi});
    ++totalEdges_;
    while (edges_.size() > kEdgeCap) edges_.removeFirst();
    // Static panel: timing is not rendered, so no update()/repaint here.
}

QuarkBulb* LedPanel::bulb(int i) const {
    return (i >= 0 && i < bulbs_.size()) ? bulbs_[i] : nullptr;
}

void LedPanel::rebuild() {
    // Tear down existing grid children.
    for (auto* b : bulbs_) { grid_->removeWidget(b); delete b; }
    bulbs_.clear();
    for (auto* l : pinLabels_) { grid_->removeWidget(l); delete l; }
    pinLabels_.clear();

    for (int i = 0; i < count_; ++i) {
        auto* bulb = new QuarkBulb(this);
        bulb->setColor(tint_);
        if (bulbSize_.isValid()) bulb->setFixedSize(bulbSize_);
        auto* label = new QLabel(prefix_ + QString::number(i), this);
        label->setFont(monoFont(9));
        if (orient_ == Qt::Vertical) {
            grid_->addWidget(bulb, i, 0, Qt::AlignCenter);
            grid_->addWidget(label, i, 1, Qt::AlignLeft | Qt::AlignVCenter);
        } else {
            grid_->addWidget(bulb, 0, i, Qt::AlignCenter);
            grid_->addWidget(label, 1, i, Qt::AlignHCenter | Qt::AlignTop);
        }
        bulbs_.append(bulb);
        pinLabels_.append(label);
    }
    refreshBulbs();
}

void LedPanel::refreshBulbs() {
    for (int i = 0; i < bulbs_.size(); ++i)
        bulbs_[i]->setState(static_cast<bool>((bits_ >> i) & 1u));
}

void LedPanel::refreshHex() {
    if (!hexLabel_) return;
    if (!showHex_) { hexLabel_->hide(); return; }
    hexLabel_->show();
    const unsigned mask = (count_ >= 16) ? 0xFFFFu : ((1u << count_) - 1u);
    const int digits = (count_ + 3) / 4;
    const QString hex = QString::number(static_cast<unsigned>(bits_ & mask), 16)
                            .rightJustified(qMax(1, digits), '0').toUpper();
    hexLabel_->setText(QStringLiteral("ODR: 0x") + hex);
}

}  // namespace quark
