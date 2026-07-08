// QuarkWidgets — B0 anchor TU.
//
// Defines the version probe. Real widget implementations (QuarkBulb.cpp, …) are
// added next to this file and appended to the CMake source list.
#include "QuarkWidgets/quarkwidgets.hpp"

namespace QuarkWidgets {

QString version() {
    return QString::fromLatin1(kVersion);
}

}  // namespace QuarkWidgets
