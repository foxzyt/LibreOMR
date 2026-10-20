/*
 * LibreOMR - Robust Optical Mark Recognition Library
 * Copyright (C) 2026 Bernardo Alvim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "test_framework.hpp"
#include "libreomr/connected_components.hpp"
#include "libreomr/fiducial.hpp"

using namespace libreomr;

TEST_CASE(TestConnectedComponentsExtraction) {
    Image img(100, 100, 1);
    img.fill(255); // background white

    // Draw two separate black squares
    img.drawFilledRect(Recti(10, 10, 10, 10), {0});
    img.drawFilledRect(Recti(50, 50, 20, 20), {0});

    std::vector<Blob> blobs = ConnectedComponents::extract(img, 0);
    ASSERT_EQ(blobs.size(), 2u);

    // Verify areas
    bool found_small = false;
    bool found_large = false;
    for (const auto& b : blobs) {
        if (b.area == 100u) found_small = true;
        if (b.area == 400u) found_large = true;
    }
    ASSERT_TRUE(found_small);
    ASSERT_TRUE(found_large);
}

TEST_CASE(TestFiducialCornerDetection) {
    uint32_t w = 800;
    uint32_t h = 1000;
    Image page(w, h, 1);
    page.fill(255); // white page

    // Place 4 solid black 24x24 markers near 4 corners
    int marker_size = 24;
    int margin = 40;

    Point2Df expected_tl(margin + marker_size / 2.0f, margin + marker_size / 2.0f);
    Point2Df expected_tr(w - margin - marker_size / 2.0f, margin + marker_size / 2.0f);
    Point2Df expected_br(w - margin - marker_size / 2.0f, h - margin - marker_size / 2.0f);
    Point2Df expected_bl(margin + marker_size / 2.0f, h - margin - marker_size / 2.0f);

    page.drawFilledRect(Recti(margin, margin, marker_size, marker_size), {0});
    page.drawFilledRect(Recti(w - margin - marker_size, margin, marker_size, marker_size), {0});
    page.drawFilledRect(Recti(w - margin - marker_size, h - margin - marker_size, marker_size, marker_size), {0});
    page.drawFilledRect(Recti(margin, h - margin - marker_size, marker_size, marker_size), {0});

    FiducialConfig config;
    auto detected = FiducialDetector::detectCorners(page, config);
    ASSERT_TRUE(detected.has_value());

    ASSERT_NEAR(detected->top_left.x, expected_tl.x, 2.0);
    ASSERT_NEAR(detected->top_left.y, expected_tl.y, 2.0);
    ASSERT_NEAR(detected->top_right.x, expected_tr.x, 2.0);
    ASSERT_NEAR(detected->top_right.y, expected_tr.y, 2.0);
    ASSERT_NEAR(detected->bottom_right.x, expected_br.x, 2.0);
    ASSERT_NEAR(detected->bottom_right.y, expected_br.y, 2.0);
    ASSERT_NEAR(detected->bottom_left.x, expected_bl.x, 2.0);
    ASSERT_NEAR(detected->bottom_left.y, expected_bl.y, 2.0);
}

TEST_CASE(TestFiducialDetectionWithDeskBackground) {
    // Simulate a photo of an exam sheet lying on a table/desk
    uint32_t total_w = 1200;
    uint32_t total_h = 1400;
    Image photo(total_w, total_h, 1);
    photo.fill(85); // dark desk background

    // Place a white page in the center
    int page_x = 200;
    int page_y = 200;
    int page_w = 800;
    int page_h = 1000;
    photo.drawFilledRect(Recti(page_x, page_y, page_w, page_h), {245});

    // Draw fiducials inside the page
    int marker_size = 24;
    int margin = 40;

    int m_tl_x = page_x + margin;
    int m_tl_y = page_y + margin;
    int m_tr_x = page_x + page_w - margin - marker_size;
    int m_tr_y = page_y + margin;
    int m_br_x = page_x + page_w - margin - marker_size;
    int m_br_y = page_y + page_h - margin - marker_size;
    int m_bl_x = page_x + margin;
    int m_bl_y = page_y + page_h - margin - marker_size;

    photo.drawFilledRect(Recti(m_tl_x, m_tl_y, marker_size, marker_size), {0});
    photo.drawFilledRect(Recti(m_tr_x, m_tr_y, marker_size, marker_size), {0});
    photo.drawFilledRect(Recti(m_br_x, m_br_y, marker_size, marker_size), {0});
    photo.drawFilledRect(Recti(m_bl_x, m_bl_y, marker_size, marker_size), {0});

    // Add some random dark specks on the desk
    photo.drawFilledRect(Recti(50, 50, 15, 15), {10});
    photo.drawFilledRect(Recti(1100, 1300, 12, 12), {15});

    FiducialConfig config;
    config.allow_table_background = true;
    auto detected = FiducialDetector::detectCorners(photo, config);
    ASSERT_TRUE(detected.has_value());

    float exp_tl_x = m_tl_x + marker_size / 2.0f;
    float exp_tl_y = m_tl_y + marker_size / 2.0f;
    float exp_tr_x = m_tr_x + marker_size / 2.0f;
    float exp_tr_y = m_tr_y + marker_size / 2.0f;
    float exp_br_x = m_br_x + marker_size / 2.0f;
    float exp_br_y = m_br_y + marker_size / 2.0f;
    float exp_bl_x = m_bl_x + marker_size / 2.0f;
    float exp_bl_y = m_bl_y + marker_size / 2.0f;

    ASSERT_NEAR(detected->top_left.x, exp_tl_x, 3.0);
    ASSERT_NEAR(detected->top_left.y, exp_tl_y, 3.0);
    ASSERT_NEAR(detected->top_right.x, exp_tr_x, 3.0);
    ASSERT_NEAR(detected->top_right.y, exp_tr_y, 3.0);
    ASSERT_NEAR(detected->bottom_right.x, exp_br_x, 3.0);
    ASSERT_NEAR(detected->bottom_right.y, exp_br_y, 3.0);
    ASSERT_NEAR(detected->bottom_left.x, exp_bl_x, 3.0);
    ASSERT_NEAR(detected->bottom_left.y, exp_bl_y, 3.0);
}

TEST_CASE(TestFiducialAdaptiveThresholdHighDPI) {
    // Test that 300 DPI high-resolution markers (e.g. 100x100 to 133x133) are not
    // hollowed out by the adaptive threshold fallback under severe lighting shadows.
    uint32_t w = 2000;
    uint32_t h = 2500;
    Image img(w, h, 1);

    // Severe horizontal shadow gradient (235 on left to 55 on right)
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            uint8_t bg = static_cast<uint8_t>(235 - (180.0 * x / w));
            img.at(x, y, 0) = bg;
        }
    }

    int marker_size = 110;
    int margin = 120;

    int tl_x = margin;
    int tl_y = margin;
    int tr_x = w - margin - marker_size;
    int tr_y = margin;
    int br_x = w - margin - marker_size;
    int br_y = h - margin - marker_size;
    int bl_x = margin;
    int bl_y = h - margin - marker_size;

    // Draw solid black markers (value 15)
    img.drawFilledRect(Recti(tl_x, tl_y, marker_size, marker_size), {15});
    img.drawFilledRect(Recti(tr_x, tr_y, marker_size, marker_size), {15});
    img.drawFilledRect(Recti(br_x, br_y, marker_size, marker_size), {15});
    img.drawFilledRect(Recti(bl_x, bl_y, marker_size, marker_size), {15});

    FiducialConfig config;
    auto detected = FiducialDetector::detectCorners(img, config);
    ASSERT_TRUE(detected.has_value());

    float exp_tl_x = tl_x + marker_size / 2.0f;
    float exp_tl_y = tl_y + marker_size / 2.0f;
    float exp_tr_x = tr_x + marker_size / 2.0f;
    float exp_tr_y = tr_y + marker_size / 2.0f;
    float exp_br_x = br_x + marker_size / 2.0f;
    float exp_br_y = br_y + marker_size / 2.0f;
    float exp_bl_x = bl_x + marker_size / 2.0f;
    float exp_bl_y = bl_y + marker_size / 2.0f;

    ASSERT_NEAR(detected->top_left.x, exp_tl_x, 5.0);
    ASSERT_NEAR(detected->top_left.y, exp_tl_y, 5.0);
    ASSERT_NEAR(detected->top_right.x, exp_tr_x, 5.0);
    ASSERT_NEAR(detected->top_right.y, exp_tr_y, 5.0);
    ASSERT_NEAR(detected->bottom_right.x, exp_br_x, 5.0);
    ASSERT_NEAR(detected->bottom_right.y, exp_br_y, 5.0);
    ASSERT_NEAR(detected->bottom_left.x, exp_bl_x, 5.0);
    ASSERT_NEAR(detected->bottom_left.y, exp_bl_y, 5.0);
}

