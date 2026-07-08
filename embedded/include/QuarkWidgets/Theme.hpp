// QuarkWidgets design tokens — the single source of visual truth for every
// control. C++ constants only: never QSS (per project rule — QSS is hard to keep
// consistent across dock widgets and custom-painted controls).
//
// Color tokens are `static inline const QColor` (QColor is not constexpr-
// constructible, so constexpr is reserved for numeric Metrics). Retune a token
// here and every widget follows.
#pragma once

#include <QColor>
#include <QFont>

namespace quark {

// Color palette. Dark UI by default (matches a hardware-debug dashboard).
struct Colors {
    // ── general palette ──
    static inline const QColor kBg       {24,  24,  28};   // window background
    static inline const QColor kPanelBg  {34,  34,  40};   // dock / panel fill
    static inline const QColor kText     {220, 222, 228};
    static inline const QColor kTextDim  {150, 152, 158};

    // ── QuarkBulb ──
    static inline const QColor kBulbWarm        {255, 176, 64};  // default on tint (warm amber)
    static inline const QColor kBulbOffGlass    {60,  60,  66};
    static inline const QColor kBulbOffFilament {40,  40,  44};
};

// Numeric metrics. constexpr where the type allows.
struct Metrics {
    static constexpr int   kBulbDefaultDiameter     = 28;    // sizeHint bulb side, px
    static constexpr qreal kBulbGlowRadiusFactor    = 1.5;   // glow r = bulb r × this
    static constexpr qreal kBulbFilamentWidthFactor = 0.045; // pen width ÷ bulb side
    static constexpr int   kPad                     = 8;     // generic padding
};

// Monospace font for terminal-style widgets (UartTerminalView, hex dumps).
inline QFont monoFont(int pointSize = 10) {
    QFont f(QStringLiteral("Monospace"));
    f.setStyleHint(QFont::Monospace);
    f.setPointSize(pointSize);
    return f;
}

}  // namespace quark
