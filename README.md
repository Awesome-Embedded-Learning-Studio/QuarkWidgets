# QuarkWidgets

QuarkWidgets is a reusable Qt6 Widgets component library — the unified UI
foundation across Awesome Embedded Learning Studio projects (micro-forge,
TAMCPP, …).

## Principles

- **Pure Qt.** Zero host-app dependencies. Widgets take plain Qt data
  (`setState(bool)`, `appendText(QString)`, …); they never reference consumer
  types (`snapshot` / `EventBus` / `GpioEdge` / …). Consumers write the adapter.
- **No QSS.** Theming via `QPalette` + C++ constant tokens
  (`quark::Colors::kBg`, `quark::Metrics::kPad`); custom widgets self-draw with
  `QPainter`.
- **C++23, Qt6 Widgets.** Namespace `quark` (short; the include dir stays
  `QuarkWidgets/`).

## Layout

```
include/QuarkWidgets/   public headers (one per widget + Theme.hpp)
src/                    widget implementations
examples/               minimal standalone apps (one per widget)
test/                   QTest self-tests
```

## Widgets

- **QuarkBulb** — a self-drawn incandescent indicator. Three QPainter layers:
  radial-gradient glow, glass bulb with a specular highlight, zigzag filament.
  API: `setState(bool)` / `setColor(QColor)` / `setBrightness(qreal 0..1)`.
- **LedPanel** — a row/column of QuarkBulbs driven by a bitmask, with pin
  labels and an ODR-hex readout. Composes QuarkBulb children (no custom
  paintEvent). API: `setLevels(quint16)` / `setColor` / `setCount` (≤16) /
  `setOrientation` / `setBulbSize`. `appendEdge(t_us, pin, hi)` is a
  forward-compat timing hook for a future LedTimingView — edges are *recorded*
  into a ring but not rendered by this static panel.
- **UartTerminalView** — a monospace scrolling serial terminal with an RX/TX
  byte-counter status bar (auto-scroll toggle + Clear). Composes a read-only
  QPlainTextEdit driven by font + palette (no QSS). API: `appendText(QString)` /
  `appendByte(uint8_t)` (non-printables render as `\xNN`) / `appendByte(t_us, b,
  Dir)` (records a directed-protocol ring for a future annotated view).

## Build standalone

```sh
cmake -B build -S .
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure   # QTest + dumps render PNGs
```

## Consume from another project (dev: local path)

```cmake
add_subdirectory("${QUARKWIDGETS_ROOT}" "${CMAKE_BINARY_DIR}/quarkwidgets-build")
target_link_libraries(your-app PRIVATE QuarkWidgets)
```

When consumed via `add_subdirectory`, standalone examples + tests are off
(`PROJECT_IS_TOP_LEVEL` is false), so they never pollute the host's ctest. For
release/CI, swap the local path for a proper `git submodule` under
`third_party/QuarkWidgets`.
