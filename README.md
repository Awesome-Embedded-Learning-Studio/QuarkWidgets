# QuarkWidgets

> **QuarkWidgets** — Awesome Embedded Studio's reusable **Qt6 / Material Design 3**
> widget library. The unified UI foundation shared across the studio's Qt-based
> projects (CFDesktop and beyond).

A 5-layer pipeline: math/utility -> theme engine -> animation engine -> material
behaviour -> widget adapter (19 MD3 widgets: Button, TextField, Slider, ComboBox,
TreeView, ...). Everything lives under the `qw::` namespace.

## Requirements

- C++23
- CMake >= 3.16
- Qt 6.8.x (Components: Core, Gui, Widgets)
- [aex](https://github.com/Awesome-Embedded-Learning-Studio/aex) (header-only
  foundation: `weak_ptr`, `expected`, ...) -- pulled automatically when building
  standalone.

## Usage

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

## Structure

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

## License

See [LICENSE](LICENSE).
