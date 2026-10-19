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

#ifndef LIBREOMR_BINARIZATION_HPP
#define LIBREOMR_BINARIZATION_HPP

#include "libreomr/export.h"
#include "libreomr/image.hpp"

namespace libreomr {

class LIBREOMR_API Binarizer {
public:
    static uint8_t computeOtsuThreshold(const Image& gray_image);
    static Image threshold(const Image& gray_image, uint8_t threshold, bool invert = false);
    static Image otsu(const Image& gray_image, bool invert = false, uint8_t* out_threshold = nullptr);
    static Image adaptiveThreshold(const Image& gray_image, int window_size = 25, int c_offset = 8, bool invert = false);

    static Image erode(const Image& binary_image, int iterations = 1);
    static Image dilate(const Image& binary_image, int iterations = 1);
    static Image open(const Image& binary_image, int iterations = 1);
    static Image close(const Image& binary_image, int iterations = 1);
    static Image invert(const Image& binary_image);
};

} // namespace libreomr

#endif /* LIBREOMR_BINARIZATION_HPP */
