/**
 * @file color_helper_test.cpp
 * @brief Comprehensive unit tests for qw::base color helper functions
 *
 * Test Coverage:
 * 1. blend() - Color blending with ratio
 * 2. elevationOverlay() - Material Design elevation overlay
 * 3. contrastRatio() - WCAG contrast ratio calculation
 * 4. tonalPalette() - Tonal palette generation from key color
 */

#include "ui/base/color_helper.h"
#include <gtest/gtest.h>

// =============================================================================
// Test Suite 1: blend()
// =============================================================================

TEST(ColorHelperTest, Blend_Ratio0_ReturnsBase) {
    qw::base::CFColor base(QColor(255, 0, 0));    // Red
    qw::base::CFColor overlay(QColor(0, 0, 255)); // Blue

    qw::base::CFColor result = qw::base::blend(base, overlay, 0.0f);

    EXPECT_EQ(result.native_color(), base.native_color());
}

TEST(ColorHelperTest, Blend_Ratio1_ReturnsOverlay) {
    qw::base::CFColor base(QColor(255, 0, 0));    // Red
    qw::base::CFColor overlay(QColor(0, 0, 255)); // Blue

    qw::base::CFColor result = qw::base::blend(base, overlay, 1.0f);

    EXPECT_EQ(result.native_color(), overlay.native_color());
}

TEST(ColorHelperTest, Blend_Ratio05_ReturnsMidpoint) {
    qw::base::CFColor base(QColor(0, 0, 0));          // Black
    qw::base::CFColor overlay(QColor(255, 255, 255)); // White

    qw::base::CFColor result = qw::base::blend(base, overlay, 0.5f);

    // Should be gray
    EXPECT_EQ(result.native_color(), QColor(128, 128, 128));
}

TEST(ColorHelperTest, Blend_RedAndWhite) {
    qw::base::CFColor base(QColor(255, 0, 0));        // Red
    qw::base::CFColor overlay(QColor(255, 255, 255)); // White

    qw::base::CFColor result = qw::base::blend(base, overlay, 0.5f);

    // Pink-ish
    EXPECT_GT(result.native_color().red(), 200);
    EXPECT_GT(result.native_color().green(), 100);
    EXPECT_GT(result.native_color().blue(), 100);
}

TEST(ColorHelperTest, Blend_ClampsRatioAbove1) {
    qw::base::CFColor base(QColor(0, 0, 0));          // Black
    qw::base::CFColor overlay(QColor(255, 255, 255)); // White

    qw::base::CFColor result = qw::base::blend(base, overlay, 1.5f);

    EXPECT_EQ(result.native_color(), overlay.native_color());
}

TEST(ColorHelperTest, Blend_ClampsRatioBelow0) {
    qw::base::CFColor base(QColor(0, 0, 0));          // Black
    qw::base::CFColor overlay(QColor(255, 255, 255)); // White

    qw::base::CFColor result = qw::base::blend(base, overlay, -0.5f);

    EXPECT_EQ(result.native_color(), base.native_color());
}

TEST(ColorHelperTest, Blend_WithAlpha) {
    qw::base::CFColor base(QColor(255, 0, 0, 255));    // Opaque red
    qw::base::CFColor overlay(QColor(0, 0, 255, 128)); // Semi-transparent blue

    qw::base::CFColor result = qw::base::blend(base, overlay, 0.5f);

    // Alpha should also be blended
    EXPECT_GT(result.native_color().alpha(), 128);
    EXPECT_LT(result.native_color().alpha(), 255);
}

TEST(ColorHelperTest, Blend_SameColor) {
    qw::base::CFColor base(QColor(100, 100, 100));
    qw::base::CFColor overlay(QColor(100, 100, 100));

    qw::base::CFColor result = qw::base::blend(base, overlay, 0.5f);

    EXPECT_EQ(result.native_color(), base.native_color());
}

TEST(ColorHelperTest, Blend_MultipleRatios) {
    qw::base::CFColor base(QColor(0, 0, 0));
    qw::base::CFColor overlay(QColor(100, 0, 0));

    auto r1 = qw::base::blend(base, overlay, 0.25f);
    auto r2 = qw::base::blend(base, overlay, 0.5f);
    auto r3 = qw::base::blend(base, overlay, 0.75f);

    EXPECT_LT(r1.native_color().red(), r2.native_color().red());
    EXPECT_LT(r2.native_color().red(), r3.native_color().red());
}

// =============================================================================
// Test Suite 2: elevationOverlay()
// =============================================================================

TEST(ColorHelperTest, ElevationOverlay_Level0_NoOverlay) {
    qw::base::CFColor surface(QColor(200, 200, 200));
    qw::base::CFColor primary(QColor(100, 100, 255));

    qw::base::CFColor result = qw::base::elevationOverlay(surface, primary, 0);

    // Level 0 should have no overlay (alpha = 0.00)
    EXPECT_EQ(result.native_color(), surface.native_color());
}

TEST(ColorHelperTest, ElevationOverlay_PositiveLevel_Darkens) {
    qw::base::CFColor surface(QColor(255, 255, 255)); // White
    qw::base::CFColor primary(QColor(0, 0, 0));       // Black

    qw::base::CFColor result = qw::base::elevationOverlay(surface, primary, 5);

    // Should be darker than white
    int total =
        result.native_color().red() + result.native_color().green() + result.native_color().blue();
    EXPECT_LT(total, 765); // 765 = 255+255+255
}

TEST(ColorHelperTest, ElevationOverlay_ClampsNegativeElevation) {
    qw::base::CFColor surface(QColor(200, 200, 200));
    qw::base::CFColor primary(QColor(100, 100, 100));

    qw::base::CFColor result = qw::base::elevationOverlay(surface, primary, -10);

    // Should be clamped to level 0
    EXPECT_EQ(result.native_color(), surface.native_color());
}

TEST(ColorHelperTest, ElevationOverlay_ClampsElevationAbove5) {
    qw::base::CFColor surface(QColor(200, 200, 200));
    qw::base::CFColor primary(QColor(0, 0, 0));

    qw::base::CFColor result1 = qw::base::elevationOverlay(surface, primary, 5);
    qw::base::CFColor result2 = qw::base::elevationOverlay(surface, primary, 10);

    // Both should be clamped to level 5
    EXPECT_EQ(result1.native_color(), result2.native_color());
}

TEST(ColorHelperTest, ElevationOverlay_IncreasingLevels) {
    qw::base::CFColor surface(QColor(255, 255, 255));
    qw::base::CFColor primary(QColor(0, 0, 0));

    auto level1 = qw::base::elevationOverlay(surface, primary, 1);
    auto level3 = qw::base::elevationOverlay(surface, primary, 3);
    auto level5 = qw::base::elevationOverlay(surface, primary, 5);

    // Higher elevation = more overlay effect (darker)
    int l1_total =
        level1.native_color().red() + level1.native_color().green() + level1.native_color().blue();
    int l3_total =
        level3.native_color().red() + level3.native_color().green() + level3.native_color().blue();
    int l5_total =
        level5.native_color().red() + level5.native_color().green() + level5.native_color().blue();

    EXPECT_GT(l1_total, l3_total);
    EXPECT_GT(l3_total, l5_total);
}

TEST(ColorHelperTest, ElevationOverlay_AlphaValues) {
    // Verify alpha values match Material Design spec
    qw::base::CFColor surface(QColor(255, 255, 255));
    qw::base::CFColor primary(QColor(0, 0, 0));

    // Level 5 should have 0.17 alpha
    qw::base::CFColor result = qw::base::elevationOverlay(surface, primary, 5);

    // Result should be 17% black blended with white
    // Expected value: 255 * (1 - 0.17) = 211.65 ≈ 212
    EXPECT_NEAR(result.native_color().red(), 212, 2);
}

// =============================================================================
// Test Suite 3: contrastRatio()
// =============================================================================

TEST(ColorHelperTest, ContrastRatio_BlackWhite) {
    qw::base::CFColor black(Qt::black);
    qw::base::CFColor white(Qt::white);

    float ratio = qw::base::contrastRatio(black, white);

    // Maximum contrast ratio is approximately 21:1
    EXPECT_NEAR(ratio, 21.0f, 1.0f);
}

TEST(ColorHelperTest, ContrastRatio_SameColor) {
    qw::base::CFColor color(QColor(128, 128, 128));

    float ratio = qw::base::contrastRatio(color, color);

    EXPECT_FLOAT_EQ(ratio, 1.0f);
}

TEST(ColorHelperTest, ContrastRatio_DarkGrayWhite) {
    qw::base::CFColor dark(QColor(50, 50, 50));
    qw::base::CFColor white(Qt::white);

    float ratio = qw::base::contrastRatio(dark, white);

    // Should be high contrast
    EXPECT_GT(ratio, 10.0f);
}

TEST(ColorHelperTest, ContrastRatio_RedGreen) {
    qw::base::CFColor red(Qt::red);
    qw::base::CFColor green(Qt::green);

    float ratio = qw::base::contrastRatio(red, green);

    // Red and green have similar luminance, so contrast is low
    EXPECT_LT(ratio, 3.0f);
}

TEST(ColorHelperTest, ContrastRatio_WCAG_AA) {
    // WCAG AA requires 4.5:1 for normal text
    qw::base::CFColor dark(QColor(30, 30, 30));
    qw::base::CFColor light(QColor(240, 240, 240));

    float ratio = qw::base::contrastRatio(dark, light);

    EXPECT_GT(ratio, 4.5f);
}

TEST(ColorHelperTest, ContrastRatio_WCAG_AAA) {
    // WCAG AAA requires 7:1 for normal text
    qw::base::CFColor black(Qt::black);
    qw::base::CFColor white(Qt::white);

    float ratio = qw::base::contrastRatio(black, white);

    EXPECT_GT(ratio, 7.0f);
}

TEST(ColorHelperTest, ContrastRatio_Symmetric) {
    qw::base::CFColor color1(QColor(100, 50, 50));
    qw::base::CFColor color2(QColor(200, 200, 200));

    float ratio1 = qw::base::contrastRatio(color1, color2);
    float ratio2 = qw::base::contrastRatio(color2, color1);

    EXPECT_FLOAT_EQ(ratio1, ratio2);
}

TEST(ColorHelperTest, ContrastRatio_LightOnDark) {
    qw::base::CFColor dark(QColor(20, 20, 20));
    qw::base::CFColor light(QColor(230, 230, 230));

    float ratio = qw::base::contrastRatio(dark, light);

    // Should exceed WCAG AA
    EXPECT_GT(ratio, 4.5f);
}

TEST(ColorHelperTest, ContrastRatio_GrayOnWhite) {
    qw::base::CFColor gray(QColor(120, 120, 120));
    qw::base::CFColor white(Qt::white);

    float ratio = qw::base::contrastRatio(gray, white);

    // Gray on white typically fails WCAG AA
    EXPECT_LT(ratio, 4.5f);
}

// =============================================================================
// Test Suite 4: tonalPalette()
// =============================================================================

TEST(ColorHelperTest, TonalPalette_Size) {
    qw::base::CFColor keyColor(QColor(100, 150, 200));

    QList<qw::base::CFColor> palette = qw::base::tonalPalette(keyColor);

    EXPECT_EQ(palette.size(), 13);
}

TEST(ColorHelperTest, TonalPalette_ToneValues) {
    qw::base::CFColor keyColor(QColor(100, 150, 200));

    QList<qw::base::CFColor> palette = qw::base::tonalPalette(keyColor);

    // Check standard tonal values: 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 95, 99, 100
    EXPECT_NEAR(palette[0].tone(), 0.0f, 1.0f);
    EXPECT_NEAR(palette[1].tone(), 10.0f, 1.0f);
    EXPECT_NEAR(palette[5].tone(), 50.0f, 1.0f);
    EXPECT_NEAR(palette[12].tone(), 100.0f, 1.0f);
}

TEST(ColorHelperTest, TonalPalette_PreservesHue) {
    qw::base::CFColor keyColor(180.0f, 50.0f, 50.0f);

    QList<qw::base::CFColor> palette = qw::base::tonalPalette(keyColor);

    // All colors should have the same hue
    for (const auto& color : palette) {
        EXPECT_NEAR(color.hue(), 180.0f, 5.0f);
    }
}

TEST(ColorHelperTest, TonalPalette_PreservesChroma) {
    qw::base::CFColor keyColor(180.0f, 50.0f, 50.0f);

    QList<qw::base::CFColor> palette = qw::base::tonalPalette(keyColor);

    // All colors should have approximately the same chroma
    // (may vary slightly due to tone constraints)
    for (const auto& color : palette) {
        EXPECT_GT(color.chroma(), 30.0f);
        EXPECT_LT(color.chroma(), 70.0f);
    }
}

TEST(ColorHelperTest, TonalPalette_LightToDark) {
    qw::base::CFColor keyColor(180.0f, 50.0f, 50.0f);

    QList<qw::base::CFColor> palette = qw::base::tonalPalette(keyColor);

    // Tone should increase from index 0 to 12
    for (int i = 1; i < palette.size(); ++i) {
        EXPECT_GT(palette[i].tone(), palette[i - 1].tone());
    }
}

TEST(ColorHelperTest, TonalPalette_FirstColorIsNearBlack) {
    qw::base::CFColor keyColor(QColor(100, 150, 200));

    QList<qw::base::CFColor> palette = qw::base::tonalPalette(keyColor);

    EXPECT_NEAR(palette[0].tone(), 0.0f, 1.0f);
    int total = palette[0].native_color().red() + palette[0].native_color().green() +
                palette[0].native_color().blue();
    EXPECT_LT(total, 50);
}

TEST(ColorHelperTest, TonalPalette_LastColorIsNearWhite) {
    qw::base::CFColor keyColor(QColor(100, 150, 200));

    QList<qw::base::CFColor> palette = qw::base::tonalPalette(keyColor);

    EXPECT_NEAR(palette[12].tone(), 100.0f, 1.0f);
    int total = palette[12].native_color().red() + palette[12].native_color().green() +
                palette[12].native_color().blue();
    EXPECT_GT(total, 700);
}

TEST(ColorHelperTest, TonalPalette_MidTone) {
    qw::base::CFColor keyColor(180.0f, 50.0f, 50.0f);

    QList<qw::base::CFColor> palette = qw::base::tonalPalette(keyColor);

    // Index 5 should be tone 50
    EXPECT_NEAR(palette[5].tone(), 50.0f, 2.0f);
}

TEST(ColorHelperTest, TonalPalette_DifferentKeyColors) {
    qw::base::CFColor red(0.0f, 80.0f, 50.0f);
    qw::base::CFColor blue(240.0f, 80.0f, 50.0f);

    QList<qw::base::CFColor> redPalette = qw::base::tonalPalette(red);
    QList<qw::base::CFColor> bluePalette = qw::base::tonalPalette(blue);

    // Palettes should be different
    EXPECT_NE(redPalette[6].native_color(), bluePalette[6].native_color());
}

// =============================================================================
// Test Suite 5: Integration Tests
// =============================================================================

TEST(ColorHelperIntegration, BlendThenContrast) {
    qw::base::CFColor black(Qt::black);
    qw::base::CFColor white(Qt::white);

    auto blended = qw::base::blend(black, white, 0.5f);

    // Contrast with white should be lower than black-white
    float originalContrast = qw::base::contrastRatio(black, white);
    float blendedContrast = qw::base::contrastRatio(blended, white);

    EXPECT_LT(blendedContrast, originalContrast);
}

TEST(ColorHelperIntegration, ElevationThenContrast) {
    qw::base::CFColor surface(QColor(240, 240, 240));
    qw::base::CFColor primary(QColor(30, 30, 30));

    auto elevated = qw::base::elevationOverlay(surface, primary, 3);

    // Elevated surface should have less contrast with primary
    float originalContrast = qw::base::contrastRatio(surface, primary);
    float elevatedContrast = qw::base::contrastRatio(elevated, primary);

    EXPECT_LT(elevatedContrast, originalContrast);
}

TEST(ColorHelperIntegration, TonalPaletteThenContrast) {
    qw::base::CFColor keyColor(180.0f, 50.0f, 50.0f);
    auto palette = qw::base::tonalPalette(keyColor);

    // First and last colors should have high contrast
    float contrast = qw::base::contrastRatio(palette[0], palette[12]);

    EXPECT_GT(contrast, 10.0f);
}

// No main() needed - using GTest::gtest_main
