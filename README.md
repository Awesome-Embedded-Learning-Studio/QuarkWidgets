# QuarkWidgets

QuarkWidgets is a reusable Qt component library serving as the unified UI
foundation across Awesome Embedded Learning Studio projects (micro-forge,
TAMCPP, …).

## Principles

- **Pure Qt.** Zero host-app dependencies. Widgets take plain Qt data
  (`setState(bool)`, `appendText(QString)`, …); they never reference consumer
  types (`snapshot` / `EventBus` / `GpioEdge` / …). Consumers write the adapter.
- **No QSS.** Theming via `QPalette` + C++ constant tokens (`Colors::bg`,
  `Metrics::pad`); custom widgets self-draw with `QPainter`.
- **C++23, Qt6 Widgets.**

## Layout

```
include/QuarkWidgets/   public headers (one per widget)
src/                    widget implementations
examples/               minimal standalone apps (one per widget)
test/                   self-tests
```

## Build standalone

```sh
cmake -B build -S .
cmake --build build -j$(nproc)
```

## Consume from another project (dev: local path)

```cmake
add_subdirectory("${QUARKWIDGETS_ROOT}" "${CMAKE_BINARY_DIR}/quarkwidgets-build")
target_link_libraries(your-app PRIVATE QuarkWidgets)
```

When consumed via `add_subdirectory`, standalone examples are off
(`PROJECT_IS_TOP_LEVEL` is false). For release/CI, swap the local path for a
proper `git submodule` under `third_party/QuarkWidgets`.

## Status

B0 skeleton — library scaffolding + version probe only. First widget
`QuarkBulb` lands next.
