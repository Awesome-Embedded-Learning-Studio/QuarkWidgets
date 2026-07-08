// Minimal standalone QuarkWidgets app — proves the library links and runs.
//
// Build & run standalone:
//   cmake -B build -S .
//   cmake --build build -j$(nproc)
//   ./build/examples/quarkwidgets_example
#include "QuarkWidgets/quarkwidgets.hpp"

#include <QApplication>
#include <QLabel>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QLabel label(QStringLiteral("QuarkWidgets %1 (skeleton)").arg(QuarkWidgets::version()));
    label.setAlignment(Qt::AlignCenter);
    label.show();
    return app.exec();
}
