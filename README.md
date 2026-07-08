# QuarkWidgets

> Awesome Embedded Studio's reusable **Qt6** widget libraries. This repo holds
> two **independent** sub-libraries with separate build architectures — consume
> either or both.

| Sub-library | Path | What it is | Build |
|---|---|---|---|
| **MD3 widget library** | repo root (`ui/`) | Qt6 / Material Design 3 widgets for app UIs (CFDesktop, …). Namespace `qw::`. | SHARED lib, aex dep, GoogleTest, Qt 6.8.x |
| **Embedded-debug controls** | `embedded/` | Hardware-debug indicators for simulator GUIs (micro-forge): QuarkBulb, LedPanel, UartTerminalView. Namespace `quark::`. | STATIC lib, QTest, zero external deps |

---

## MD3 widget library (repo root)

> A Qt6 / Material Design 3 widget library — the unified UI foundation shared
> across the studio's Qt-based projects (CFDesktop and beyond).

A 5-layer pipeline: math/utility -> theme engine -> animation engine -> material
behaviour -> widget adapter (19 MD3 widgets: Button, TextField, Slider, ComboBox,
TreeView, ...). Everything lives under the `qw::` namespace.

### Requirements

- C++23
- CMake >= 3.16
- Qt 6.8.x (Components: Core, Gui, Widgets)
- [aex](https://github.com/Awesome-Embedded-Learning-Studio/aex) (header-only
  foundation: `weak_ptr`, `expected`, ...) -- pulled automatically when building
  standalone.

### Usage

**1. As a CMake subproject / submodule** (recommended inside the org):
```cmake
add_subdirectory(third_party/QuarkWidgets)
target_link_libraries(my_target PRIVATE QuarkWidgets::quarkwidgets)
```
When consumed by a superproject that already provides `aex::aex`, QuarkWidgets
reuses it (no duplicate fetch).

**2. Standalone:**
```bash
cmake -S . -B build -DQW_BUILD_TESTS=ON -DQW_BUILD_EXAMPLES=ON
cmake --build build
ctest --test-dir build
```

Consumers keep their existing include style (`"core/theme_manager.h"`,
`"widget/material/..."`) because QuarkWidgets exposes its source directories as
PUBLIC include paths.

### Structure

```
ui/
  base/         CFColor, GeometryHelper, Easing, DevicePixel (math/utility)
  core/         ThemeManager, MaterialFactory, token system
  components/   animation engine (CFMaterialAnimationFactory, strategies)
  widget/       MD3 widgets + application support
  models/       SDK-like models
test/ui/        GoogleTest suite (500 tests)
example/        gui + ui widget galleries
```

---

## Embedded-debug controls (`embedded/`)

Pure-Qt, data-driven, QPainter-self-drawn widgets for embedded-simulator GUIs
(the board view, LED panels, UART terminal in micro-forge). Zero host-app
dependency: every setter takes a plain Qt type; the host writes the adapter.

- **QuarkBulb** — self-drawn incandescent indicator (glow + glass + filament).
  `setState(bool)` / `setColor(QColor)` / `setBrightness(qreal 0..1)`.
- **LedPanel** — a row/column of QuarkBulbs driven by a bitmask, with pin labels
  and an ODR-hex readout. `setLevels(quint16)` / `setColor` / `setCount` (≤16) /
  `setOrientation` / `setBulbSize`. `appendEdge(t_us, pin, hi)` is a forward-compat
  timing hook for a future LedTimingView (recorded, not rendered).
- **UartTerminalView** — monospace scrolling serial terminal with an RX/TX
  byte-counter status bar (auto-scroll + Clear). `appendText(QString)` /
  `appendByte(uint8_t)` (non-printables render as `\xNN`) /
  `appendByte(t_us, b, Dir)`.

### Consume

```cmake
add_subdirectory("<QuarkWidgets>/embedded" "${CMAKE_BINARY_DIR}/quarkwidgets-embedded")
target_link_libraries(your-app PRIVATE QuarkWidgets::Embedded)
```
Include style is unchanged: `#include "QuarkWidgets/QuarkBulb.hpp"`. When consumed
via `add_subdirectory` the example/test binaries are off (`PROJECT_IS_TOP_LEVEL`
is false). Standalone: `cmake -S embedded -B build && cmake --build build`.

## License

See [LICENSE](LICENSE).
