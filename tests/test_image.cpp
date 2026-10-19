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
#include "libreomr/image.hpp"
#include "libreomr/binarization.hpp"

using namespace libreomr;

TEST_CASE(TestImageAllocationAndPixelAccess) {
    Image img(100, 50, 1);
    ASSERT_EQ(img.width(), 100u);
    ASSERT_EQ(img.height(), 50u);
    ASSERT_EQ(img.channels(), 1u);
    ASSERT_EQ(img.sizeBytes(), 5000u);

    img.fill(120);
    ASSERT_EQ(img.at(10, 10, 0), 120);

    img.at(42, 24, 0) = 200;
    ASSERT_EQ(img.at(42, 24, 0), 200);
}

TEST_CASE(TestGrayscaleConversion) {
    Image rgb(10, 10, 3);
    // Fill with pure white (255, 255, 255)
    rgb.fill(255);
    Image gray = rgb.toGrayscale();
    ASSERT_EQ(gray.channels(), 1u);
    ASSERT_EQ(gray.at(5, 5, 0), 255);

    // Set one pixel to red (255, 0, 0) -> luminance approx 0.299 * 255 = 76
    rgb.at(2, 2, 0) = 255;
    rgb.at(2, 2, 1) = 0;
    rgb.at(2, 2, 2) = 0;
    Image gray2 = rgb.toGrayscale();
    ASSERT_NEAR(gray2.at(2, 2, 0), 76, 2);
}

TEST_CASE(TestOtsuBinarization) {
    // Create a bimodal image: half black (30), half white (220)
    Image bimodal(100, 100, 1);
    for (uint32_t y = 0; y < 100; ++y) {
        for (uint32_t x = 0; x < 100; ++x) {
            bimodal.at(x, y, 0) = (x < 50) ? 30 : 220;
        }
    }

    uint8_t threshold = Binarizer::computeOtsuThreshold(bimodal);
    // Optimal threshold should comfortably separate 30 and 220 (around 120-130)
    ASSERT_TRUE(threshold > 30 && threshold < 220);

    Image binary = Binarizer::otsu(bimodal);
    ASSERT_EQ(binary.at(10, 10, 0), 0);   // Dark region -> 0
    ASSERT_EQ(binary.at(80, 80, 0), 255); // Light region -> 255
}

TEST_CASE(TestMorphologicalOperations) {
    Image bin(20, 20, 1);
    bin.fill(255); // background white

    // Draw a single 4x4 black square in center
    bin.drawFilledRect(Recti(8, 8, 4, 4), {0});
    ASSERT_EQ(bin.at(9, 9, 0), 0);

    // Dilation expands black foreground (0)
    Image dilated = Binarizer::dilate(bin, 1);
    ASSERT_EQ(dilated.at(7, 7, 0), 0);
    ASSERT_EQ(dilated.at(12, 12, 0), 0);

    // Erosion shrinks black foreground (0)
    Image eroded = Binarizer::erode(bin, 1);
    // Outer edges should become background (255)
    ASSERT_EQ(eroded.at(8, 8, 0), 255);
}
