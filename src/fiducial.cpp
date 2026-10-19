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

#include "libreomr/fiducial.hpp"
#include "libreomr/binarization.hpp"
#include "libreomr/connected_components.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace libreomr {

std::vector<Point2Df> FiducialDetector::findMarkerCandidates(const Image& binary_image, const FiducialConfig& config) {
    std::vector<Point2Df> candidates;
    if (binary_image.empty()) return candidates;

    uint32_t w = binary_image.width();
    uint32_t h = binary_image.height();
    double total_area = static_cast<double>(w) * h;

    std::vector<Blob> blobs = ConnectedComponents::extract(binary_image, 0); // 0 = black foreground

    float min_area = static_cast<float>(config.min_area_fraction * total_area);
    float max_area = static_cast<float>(config.max_area_fraction * total_area);

    float margin_x = w * config.search_margin_fraction;
    float margin_y = h * config.search_margin_fraction;

    for (const auto& blob : blobs) {
        if (blob.touches_border) continue;

        if (blob.area < min_area || blob.area > max_area) continue;
        if (blob.aspect_ratio < config.min_aspect_ratio || blob.aspect_ratio > config.max_aspect_ratio) continue;
        if (blob.solidity < config.min_solidity) continue;

        // Check if candidate is located in margin corners
        bool in_left = blob.centroid.x <= margin_x;
        bool in_right = blob.centroid.x >= (w - margin_x);
        bool in_top = blob.centroid.y <= margin_y;
        bool in_bottom = blob.centroid.y >= (h - margin_y);

        if ((in_left || in_right) && (in_top || in_bottom)) {
            candidates.push_back(blob.centroid);
        }
    }

    return candidates;
}

std::optional<Quad2D> FiducialDetector::detectCorners(const Image& gray_image, const FiducialConfig& config) {
    if (gray_image.empty()) return std::nullopt;

    Image gray = (gray_image.channels() > 1) ? gray_image.toGrayscale() : gray_image;
    Image binary = Binarizer::otsu(gray);

    std::vector<Point2Df> candidates = findMarkerCandidates(binary, config);
    if (candidates.size() < 4) {
        // Fallback: try with adaptive threshold if standard Otsu missed faint/shadowed corners
        Image adapt_binary = Binarizer::adaptiveThreshold(gray, 31, 10);
        candidates = findMarkerCandidates(adapt_binary, config);
        if (candidates.size() < 4) {
            return std::nullopt;
        }
    }

    float w = static_cast<float>(gray.width());
    float h = static_cast<float>(gray.height());

    Point2Df target_tl(0.0f, 0.0f);
    Point2Df target_tr(w, 0.0f);
    Point2Df target_br(w, h);
    Point2Df target_bl(0.0f, h);

    auto findClosest = [&](const Point2Df& target) -> Point2Df {
        Point2Df best = candidates[0];
        double min_dist = std::numeric_limits<double>::max();
        for (const auto& pt : candidates) {
            double d = pt.distanceTo(target);
            if (d < min_dist) {
                min_dist = d;
                best = pt;
            }
        }
        return best;
    };

    Point2Df tl = findClosest(target_tl);
    Point2Df tr = findClosest(target_tr);
    Point2Df br = findClosest(target_br);
    Point2Df bl = findClosest(target_bl);

    // Verify all 4 points are distinct
    if (tl.distanceTo(tr) < 20.0 || tl.distanceTo(bl) < 20.0 ||
        br.distanceTo(tr) < 20.0 || br.distanceTo(bl) < 20.0) {
        return std::nullopt;
    }

    Quad2D quad(tl, tr, br, bl);
    if (quad.area() < (w * h * 0.20)) {
        // Quad area is unreasonably small
        return std::nullopt;
    }

    return quad;
}

} // namespace libreomr
