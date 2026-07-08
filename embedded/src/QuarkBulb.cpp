// QuarkBulb — see QuarkBulb.hpp.
#include "QuarkWidgets/QuarkBulb.hpp"

#include <QGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPen>
#include <QPointF>
#include <QRectF>
#include <QSize>
#include <QtGlobal>  // qBound, qMax, qMin
#include <QtMath>    // qRound

namespace quark {

namespace {
// Filament path: two vertical supports strung with a zigzag coil. Fits inside a
// circle of the given radius centred at c.
QPainterPath makeFilament(const QPointF& c, qreal radius) {
    const qreal half_w = radius * 0.34;
    const qreal top     = c.y() - radius * 0.55;
    const qreal bottom  = c.y() + radius * 0.60;
    QPainterPath p;
    // supports
    p.moveTo(c.x() - half_w, top);
    p.lineTo(c.x() - half_w, bottom);
    p.moveTo(c.x() + half_w, top);
    p.lineTo(c.x() + half_w, bottom);
    // zigzag coil between them
    const int teeth = 5;
    p.moveTo(c.x() - half_w, top);
    for (int i = 1; i <= teeth; ++i) {
        const qreal t = static_cast<qreal>(i) / teeth;
        const qreal y = top + (bottom - top) * t;
        const qreal x = (i % 2 == 1) ? c.x() + half_w : c.x() - half_w;
        p.lineTo(x, y);
    }
    return p;
}
}  // namespace

QuarkBulb::QuarkBulb(QWidget* parent) : QWidget(parent) {
    // Fixed policy: the bulb draws at sizeHint() and the glow fits inside it.
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void QuarkBulb::setState(bool on) {
    if (on != on_) {
        on_ = on;
        update();
    }
}

void QuarkBulb::setColor(QColor tint) {
    tint_ = tint;
    update();
}

void QuarkBulb::setBrightness(qreal b) {
    brightness_ = qBound(0.0, b, 1.0);
    update();
}

QSize QuarkBulb::sizeHint() const {
    // Widget side = bulb side × glow factor + a hair of padding, so the on-state
    // glow (radius = bulb r × kBulbGlowRadiusFactor) stays fully inside.
    const int side = qRound(Metrics::kBulbDefaultDiameter *
                            Metrics::kBulbGlowRadiusFactor) + 2;
    return {side, side};
}

void QuarkBulb::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // Bulb fills the widget except the glow margin (glow extends beyond the
    // glass to glowR = bulb r × factor). Responsive to the actual widget size.
    const qreal widget_side = qMin(qreal(width()), qreal(height()));
    const qreal bulb_side   = qMax(qreal(8.0),
                                   widget_side / Metrics::kBulbGlowRadiusFactor);
    const qreal radius      = bulb_side / 2.0;
    const QPointF c         = rect().center();
    const QRectF bulb_rect(c.x() - radius, c.y() - radius, bulb_side, bulb_side);

    // ── Layer 1: glow (lit only) ─────────────────────────────────────────────
    // Opacity AND radius scale linearly with brightness — monotonic across the
    // whole [0,1] range so a dimming transition fades smoothly (no mid-ring
    // inversion at low brightness). At b=0 the glow is fully transparent; at
    // b=1 it reaches kBulbGlowRadiusFactor × radius with peak core opacity. The
    // radius never shrinks inside the glass (min = radius), keeping the
    // documented "glow extends beyond the glass" invariant for any b > 0.
    if (on_) {
        const qreal glow_r = radius * (1.0 + (Metrics::kBulbGlowRadiusFactor - 1.0) *
                                       brightness_);
        QRadialGradient g(c, glow_r);
        const QColor tint_rgb(tint_.red(), tint_.green(), tint_.blue());
        QColor core = tint_rgb; core.setAlphaF(0.70 * brightness_);
        QColor mid  = tint_rgb; mid.setAlphaF(0.30 * brightness_);
        g.setColorAt(0.0, core);
        g.setColorAt(0.5, mid);
        g.setColorAt(1.0, QColor(tint_rgb.red(), tint_rgb.green(), tint_rgb.blue(), 0));
        p.setBrush(g);
        p.setPen(Qt::NoPen);
        p.drawEllipse(c, glow_r, glow_r);
    }

    // ── Layer 2: glass bulb ──────────────────────────────────────────────────
    const QColor base = on_
        ? tint_.lighter(qRound(125 + 40 * brightness_))   // brighter when brighter
        : Colors::kBulbOffGlass;
    p.setPen(QPen(base.darker(160), 1));
    p.setBrush(base);
    p.drawEllipse(bulb_rect);
    // glass sheen — faint translucent white wash over the whole bulb
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 255, 255, on_ ? 36 : 18));
    p.drawEllipse(bulb_rect);
    // specular highlight — top-left
    const QRectF hl(c.x() - bulb_side * 0.24, c.y() - bulb_side * 0.28,
                    bulb_side * 0.26, bulb_side * 0.20);
    p.setBrush(QColor(255, 255, 255, on_ ? 170 : 80));
    p.drawEllipse(hl);

    // ── Layer 3: filament ────────────────────────────────────────────────────
    const QColor fil_color = on_ ? tint_.lighter(140) : Colors::kBulbOffFilament;
    const qreal pen_w = qMax(qreal(1.0), bulb_side * Metrics::kBulbFilamentWidthFactor);
    p.setPen(QPen(fil_color, pen_w, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    p.drawPath(makeFilament(c, radius));
}

}  // namespace quark
