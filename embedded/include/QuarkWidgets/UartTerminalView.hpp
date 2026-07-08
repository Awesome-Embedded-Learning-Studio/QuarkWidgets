// UartTerminalView — a monospace scrolling serial terminal with an RX/TX byte
// counter status bar. Composes a read-only QPlainTextEdit (Qt's purpose-built
// log/terminal primitive) driven by font + palette — never QSS (project rule).
//
// The visible terminal is fed entirely by the append* methods: bytes flow in,
// text scrolls out. Three entry points cover two usage styles:
//
//   • Text-oriented (host already decoded bytes):  appendText(QString)
//   • Byte-oriented (raw UART stream):             appendByte(uint8_t b)
//   • Directed protocol (annotated future view):   appendByte(t_us, b, Dir)
//
// appendByte(t_us, b, dir) is the forward-compat hook — like LedPanel::appendEdge
// it RECORDS {t,b,dir} into a ring for a future timestamped/coloured annotated
// view, but UNLIKE LedPanel it also renders the byte inline (the terminal's
// whole job is to show bytes; hiding them would be surprising). The ring is
// metadata only — the visible text is always just the appended bytes.
//
// Data-driven, zero host-app dependency: every method takes a plain Qt / C++
// type. Consumers adapt their own state (a USART output buffer, an event bus…)
// into append calls.
#pragma once

#include <QList>
#include <QString>
#include <QTextCursor>
#include <QWidget>

#include <cstdint>

class QCheckBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;

namespace quark {

// Direction of a directed-protocol byte (appendByte overload below).
enum class Dir : unsigned char { Rx, Tx };

class UartTerminalView : public QWidget {
    Q_OBJECT
  public:
    explicit UartTerminalView(QWidget* parent = nullptr);

    // Append decoded text as received (RX). Every UTF-8 byte of `text` counts
    // toward rxBytes(). Embedded '\n' starts a new line, as expected.
    void appendText(const QString& text);

    // Append one received byte. Printable ASCII (0x20..0x7E) and the common
    // whitespace chars (\n \r \t) pass through verbatim; any other byte renders
    // as the 4-char escape "\xNN". Counts as 1 RX byte.
    void appendByte(std::uint8_t b);

    // Directed protocol entry — records {tUs,b,dir} into a capped ring (for a
    // future annotated view), renders `b` inline (same rules as appendByte),
    // and increments rxBytes (Rx) or txBytes (Tx). See file header.
    void appendByte(qint64 tUs, std::uint8_t b, Dir dir);

    // Clear the text body AND reset both byte counters. Keeps maxLines. Use to
    // resync when the host's backing buffer was reset (e.g. a firmware reload).
    void clear();

    // Auto-scroll: when true (default) each append scrolls the view to the end.
    // Turn off to let the user scroll back through history without being yanked
    // down on every new byte. The checkbox in the status bar toggles this.
    void setAutoScroll(bool on);
    [[nodiscard]] bool autoScroll() const noexcept { return autoScroll_; }

    // Max retained lines; once exceeded the oldest are discarded from the top
    // (bounded memory for long sessions). 0 = unlimited. Default 10000.
    void setMaxLines(int lines);
    [[nodiscard]] int maxLines() const noexcept { return maxLines_; }

    [[nodiscard]] std::uint64_t rxBytes() const noexcept { return rxBytes_; }
    [[nodiscard]] std::uint64_t txBytes() const noexcept { return txBytes_; }

    // Total directed-protocol entries ever recorded (uncapped counter — the
    // ring only bounds retained history, not the count).
    [[nodiscard]] int protoCount() const noexcept { return totalProto_; }

  Q_SIGNALS:
    // Fired after clear(). Lets a host resync its own delta tracker in one place.
    void cleared();

  private:
    struct ProtoEntry { qint64 tUs; std::uint8_t b; Dir dir; };

    // Shared by all three append paths: render text at the document end without
    // disturbing the view's selection (uses a stored cursor, not the view's).
    void appendRendered(const QString& text);
    void refreshStatusBar();

    QPlainTextEdit* view_;
    QLabel*         rxLabel_;
    QLabel*         txLabel_;
    QCheckBox*      autoScrollBox_;
    QTextCursor     cursor_;   // kept at the document end; survives top-trim

    int            maxLines_   = 10000;
    bool           autoScroll_ = true;
    std::uint64_t  rxBytes_    = 0;
    std::uint64_t  txBytes_    = 0;

    QList<ProtoEntry> proto_;   // capped ring (kProtoCap in the .cpp)
    int              totalProto_ = 0;
};

}  // namespace quark
