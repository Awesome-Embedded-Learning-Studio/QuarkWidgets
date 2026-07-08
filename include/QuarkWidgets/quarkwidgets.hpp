// QuarkWidgets — org-level Qt6 Widgets control library.
//
// Pure Qt, data-driven, QPainter self-drawn widgets. This header is the B0
// anchor: a version probe that proves the library is linked and the include
// path is wired. Real widgets (QuarkBulb, LedPanel, UartTerminalView, …) land
// alongside in subsequent batches; each gets its own header here.
#pragma once

#include <QString>

namespace QuarkWidgets {

// Library version, bumped per release. Semver-ish.
inline constexpr char kVersion[] = "0.1.0-dev";

// Returns kVersion as a QString. Lets a consumer sanity-check it linked the
// right build (and gives the skeleton example something to display).
QString version();

}  // namespace QuarkWidgets
