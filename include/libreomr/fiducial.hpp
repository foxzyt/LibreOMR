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

#ifndef LIBREOMR_FIDUCIAL_HPP
#define LIBREOMR_FIDUCIAL_HPP

#include "libreomr/export.h"
#include "libreomr/geometry.hpp"
#include "libreomr/image.hpp"
#include <optional>
#include <vector>

namespace libreomr {

enum class FiducialType {
    SolidSquare,
    Bullseye
};

struct LIBREOMR_API FiducialConfig {
    FiducialType type{FiducialType::SolidSquare};
    float min_area_fraction{0.00005f};
    float max_area_fraction{0.03f};
    float min_aspect_ratio{0.70f};
    float max_aspect_ratio{1.42f};
    float min_solidity{0.75f};
    float search_margin_fraction{0.28f};
};

class LIBREOMR_API FiducialDetector {
public:
    static std::optional<Quad2D> detectCorners(const Image& gray_image, const FiducialConfig& config = {});
    static std::vector<Point2Df> findMarkerCandidates(const Image& binary_image, const FiducialConfig& config = {});
};

} // namespace libreomr

#endif /* LIBREOMR_FIDUCIAL_HPP */
