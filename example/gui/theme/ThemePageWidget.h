/**
 * @file ThemePageWidget.h
 * @brief Base class for all theme gallery pages
 *
 * @author CFDesktop Team
 * @date 2026-02-28
 * @version 0.1
 */

#pragma once

#include <QWidget>

namespace qw::core {
struct ICFTheme;
}

namespace qw::gallery {

/**
 * @brief Base class for all content pages in the Material Gallery.
 *
 * Provides a common interface for all pages that can be switched via the sidebar.
 * Each page is responsible for displaying its own content and responding to theme changes.
 */
class ThemePageWidget : public QWidget {
    Q_OBJECT

  public:
    explicit ThemePageWidget(QWidget* parent = nullptr);
    ~ThemePageWidget() override = default;

    /**
     * @brief Get the display title of this page
     * @return Page title for display purposes
     */
    virtual QString pageTitle() const = 0;

    /**
     * @brief Apply a new theme to this page
     * @param theme The theme to apply
     */
    virtual void applyTheme(const qw::core::ICFTheme& theme) = 0;
};

} // namespace qw::gallery
