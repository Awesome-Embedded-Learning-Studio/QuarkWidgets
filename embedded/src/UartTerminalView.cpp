// UartTerminalView — see UartTerminalView.hpp.
#include "QuarkWidgets/UartTerminalView.hpp"

#include "QuarkWidgets/Theme.hpp"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QString>
#include <QTextCursor>
#include <QVBoxLayout>
#include <Qt>

namespace quark {

namespace {
constexpr int kProtoCap        = 4096;   // directed-entry ring cap (history only)
constexpr int kDefaultMaxLines = 10000;

// Render one byte: printable ASCII + common whitespace pass through; anything
// else becomes the 4-char escape "\xNN" so binary protocols stay visible.
QString byteToText(std::uint8_t b) {
    if ((b >= 0x20 && b <= 0x7E) || b == '\n' || b == '\r' || b == '\t')
        return QString(QChar::fromLatin1(static_cast<char>(b)));
    return QStringLiteral("\\x%1").arg(b, 2, 16, QChar('0')).toUpper();
}
} // namespace

UartTerminalView::UartTerminalView(QWidget* parent) : QWidget(parent) {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(Metrics::kPad, Metrics::kPad, Metrics::kPad, Metrics::kPad);
    lay->setSpacing(Metrics::kPad);

    view_ = new QPlainTextEdit(this);
    view_->setReadOnly(true);
    view_->setFont(monoFont(10));
    view_->setMaximumBlockCount(kDefaultMaxLines);
    view_->setCenterOnScroll(false);
    // No QSS: palette + font only (project rule).
    QPalette pal = view_->palette();
    pal.setColor(QPalette::Base, Colors::kPanelBg);
    pal.setColor(QPalette::Text, Colors::kText);
    view_->setPalette(pal);
    cursor_ = QTextCursor(view_->document());
    cursor_.movePosition(QTextCursor::End);
    lay->addWidget(view_);

    // ── status bar: [RX: N] [TX: M]  <stretch>  [☑ auto-scroll] [Clear] ──
    auto* bar = new QHBoxLayout;
    bar->setSpacing(Metrics::kPad);
    rxLabel_ = new QLabel(QStringLiteral("RX: 0"), this);
    txLabel_ = new QLabel(QStringLiteral("TX: 0"), this);
    rxLabel_->setFont(monoFont(9));
    txLabel_->setFont(monoFont(9));
    bar->addWidget(rxLabel_);
    bar->addWidget(txLabel_);
    bar->addStretch(1);

    autoScrollBox_ = new QCheckBox(QStringLiteral("auto-scroll"), this);
    autoScrollBox_->setChecked(autoScroll_);
    bar->addWidget(autoScrollBox_);

    auto* clearBtn = new QPushButton(QStringLiteral("Clear"), this);
    bar->addWidget(clearBtn);
    lay->addLayout(bar);

    connect(autoScrollBox_, &QCheckBox::toggled, this,
            [this](bool on) { autoScroll_ = on; });
    connect(clearBtn, &QPushButton::clicked, this, [this] { clear(); });

    refreshStatusBar();
}

void UartTerminalView::appendText(const QString& text) {
    appendRendered(text);
    // Count the true UTF-8 byte length (ASCII-preserving), not UTF-16 units.
    rxBytes_ += static_cast<std::uint64_t>(text.toUtf8().size());
    refreshStatusBar();
}

void UartTerminalView::appendByte(std::uint8_t b) {
    appendRendered(byteToText(b));
    ++rxBytes_;
    refreshStatusBar();
}

void UartTerminalView::appendByte(qint64 tUs, std::uint8_t b, Dir dir) {
    proto_.append(ProtoEntry{tUs, b, dir});
    ++totalProto_;
    while (proto_.size() > kProtoCap) proto_.removeFirst();

    appendRendered(byteToText(b));
    if (dir == Dir::Rx) ++rxBytes_; else ++txBytes_;
    refreshStatusBar();
}

void UartTerminalView::clear() {
    view_->clear();
    rxBytes_ = 0;
    txBytes_ = 0;
    // view_->clear() swaps the document, so re-anchor our cursor on the new one.
    cursor_ = QTextCursor(view_->document());
    cursor_.movePosition(QTextCursor::End);
    refreshStatusBar();
    emit cleared();
}

void UartTerminalView::setAutoScroll(bool on) {
    autoScroll_ = on;
    autoScrollBox_->setChecked(on);
}

void UartTerminalView::setMaxLines(int lines) {
    maxLines_ = qMax(0, lines);
    view_->setMaximumBlockCount(maxLines_);
}

void UartTerminalView::appendRendered(const QString& text) {
    cursor_.insertText(text);
    if (autoScroll_) {
        // Make the view's cursor track our end cursor, then scroll to reveal it.
        // Only touches the view cursor when autoScroll is on, so scrolling back
        // to read history (autoScroll off) never clobbers a text selection.
        view_->setTextCursor(cursor_);
        view_->ensureCursorVisible();
    }
}

void UartTerminalView::refreshStatusBar() {
    rxLabel_->setText(QStringLiteral("RX: %1").arg(rxBytes_));
    txLabel_->setText(QStringLiteral("TX: %1").arg(txBytes_));
}

}  // namespace quark
