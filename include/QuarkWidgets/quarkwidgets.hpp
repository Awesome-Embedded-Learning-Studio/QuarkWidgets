// QuarkWidgets — org-level Qt6 Widgets control library.
//
// Library namespace is `quark` (short; avoids the QuarkWidgets::QuarkBulb
// stutter). Include-path dir stays QuarkWidgets/. This header is the B0 anchor:
// a version probe proving the library is linked and the include path is wired.
// Real widgets (QuarkBulb, LedPanel, UartTerminalView, …) live alongside.
#pragma once

#include <QString>

namespace quark {

// Library version, bumped per release. Semver-ish.
inline constexpr char kVersion[] = "0.1.0-dev";

// Returns kVersion as a QString. Lets a consumer sanity-check it linked the
// right build (and gives the skeleton example something to display).
QString version();

}  // namespace quark
