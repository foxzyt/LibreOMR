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
