// QuarkBulb — a self-drawn incandescent indicator ("the light is on").
//
// Three QPainter layers (tokens in Theme.hpp):
//   1. Glow       — QRadialGradient tint → transparent; drawn only when lit.
//                   Radius + opacity scale with brightness (PWM-dim hook).
//   2. Glass bulb — translucent circle, warm-tinted when lit / grey when dark,
//                   with a top-left specular highlight.
//   3. Filament   — QPainterPath zigzag between two supports; hot when lit,
//                   dark grey when dark.
//
// Data-driven, zero host-app dependency: every setter takes a plain Qt type.
// Consumers adapt their own state (GPIO bits, PWM duty, …) into these calls.
#pragma once

#include "QuarkWidgets/Theme.hpp"

#include <QColor>
#include <QWidget>

namespace quark {

class QuarkBulb : public QWidget {
    Q_OBJECT
  public:
    explicit QuarkBulb(QWidget* parent = nullptr);

    // Lit state. Off by default.
    void setState(bool on);
    [[nodiscard]] bool state() const noexcept { return on_; }

    // Emitted-light tint — the glow, warm glass and hot filament all derive
    // from it. Default warm amber (Colors::kBulbWarm).
    void setColor(QColor tint);
    [[nodiscard]] QColor color() const noexcept { return tint_; }

    // Perceived intensity, clamped to [0, 1]. Scales glow radius/opacity and
    // filament heat. Hook for future PWM-style dimming; default 1.0.
    void setBrightness(qreal b);
    [[nodiscard]] qreal brightness() const noexcept { return brightness_; }

    [[nodiscard]] QSize sizeHint() const override;

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    bool   on_         {false};
    QColor tint_       {Colors::kBulbWarm};
    qreal  brightness_ {1.0};
};

}  // namespace quark
