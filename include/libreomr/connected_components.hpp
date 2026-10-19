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

#ifndef LIBREOMR_CONNECTED_COMPONENTS_HPP
#define LIBREOMR_CONNECTED_COMPONENTS_HPP

#include "libreomr/export.h"
#include "libreomr/geometry.hpp"
#include "libreomr/image.hpp"
#include <vector>

namespace libreomr {

struct LIBREOMR_API Blob {
    int32_t label{0};
    uint32_t area{0};
    Recti bbox{};
    Point2Df centroid{};
    float aspect_ratio{1.0f};
    float solidity{1.0f};
    bool touches_border{false};
};

class LIBREOMR_API ConnectedComponents {
public:
    static std::vector<Blob> extract(const Image& binary_image, uint8_t foreground_val = 0);
};

} // namespace libreomr

#endif /* LIBREOMR_CONNECTED_COMPONENTS_HPP */
