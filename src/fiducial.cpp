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

    std::vector<Point2Df> margin_candidates;
    std::vector<Point2Df> all_candidates;

    for (const auto& blob : blobs) {
        if (blob.touches_border) continue;

        if (blob.area < min_area || blob.area > max_area) continue;
        if (blob.aspect_ratio < config.min_aspect_ratio || blob.aspect_ratio > config.max_aspect_ratio) continue;
        if (blob.solidity < config.min_solidity) continue;

        all_candidates.push_back(blob.centroid);

        // Check if candidate is located in margin corners
        bool in_left = blob.centroid.x <= margin_x;
        bool in_right = blob.centroid.x >= (w - margin_x);
        bool in_top = blob.centroid.y <= margin_y;
        bool in_bottom = blob.centroid.y >= (h - margin_y);

        if ((in_left || in_right) && (in_top || in_bottom)) {
            margin_candidates.push_back(blob.centroid);
        }
    }

    if (margin_candidates.size() >= 4) {
        return margin_candidates;
    }

    if (config.allow_table_background) {
        return all_candidates;
    }

    return margin_candidates;
}

std::optional<Quad2D> FiducialDetector::detectCorners(const Image& gray_image, const FiducialConfig& config) {
    if (gray_image.empty()) return std::nullopt;

    Image gray = (gray_image.channels() > 1) ? gray_image.toGrayscale() : gray_image;
    Image binary = Binarizer::otsu(gray);

    std::vector<Point2Df> candidates = findMarkerCandidates(binary, config);
    if (candidates.size() < 4) {
        // Fallback: try with adaptive threshold if standard Otsu missed faint/shadowed corners
        Image adapt_binary = Binarizer::adaptiveThreshold(gray, 41, 10);
        candidates = findMarkerCandidates(adapt_binary, config);
        if (candidates.size() < 4) {
            Image adapt_binary2 = Binarizer::adaptiveThreshold(gray, 65, 15);
            candidates = findMarkerCandidates(adapt_binary2, config);
            if (candidates.size() < 4) {
                return std::nullopt;
            }
        }
    }

    float w = static_cast<float>(gray.width());
    float h = static_cast<float>(gray.height());

    // Filter candidates by surrounding paper background if we have more than 4 candidates
    std::vector<Point2Df> filtered_candidates = candidates;
    if (candidates.size() > 4) {
        std::vector<std::pair<float, Point2Df>> scored;
        for (const auto& pt : candidates) {
            int cx = static_cast<int>(std::round(pt.x));
            int cy = static_cast<int>(std::round(pt.y));
            uint8_t center_val = gray.at(std::clamp(cx, 0, static_cast<int>(gray.width()) - 1),
                                         std::clamp(cy, 0, static_cast<int>(gray.height()) - 1), 0);

            // Sample an outer ring around the marker candidate
            int ring_r = 14;
            int bg_sum = 0;
            int bg_cnt = 0;
            for (int dy = -ring_r; dy <= ring_r; dy += 4) {
                for (int dx = -ring_r; dx <= ring_r; dx += 4) {
                    if (std::abs(dx) >= 8 || std::abs(dy) >= 8) {
                        int sx = std::clamp(cx + dx, 0, static_cast<int>(gray.width()) - 1);
                        int sy = std::clamp(cy + dy, 0, static_cast<int>(gray.height()) - 1);
                        bg_sum += gray.at(sx, sy, 0);
                        bg_cnt++;
                    }
                }
            }
            float bg_avg = bg_cnt > 0 ? static_cast<float>(bg_sum) / bg_cnt : 255.0f;
            float contrast = bg_avg - center_val;
            scored.push_back({contrast, pt});
        }

        // Sort by contrast descending
        std::sort(scored.begin(), scored.end(), [](const auto& a, const auto& b) {
            return a.first > b.first;
        });

        filtered_candidates.clear();
        for (size_t i = 0; i < std::min<size_t>(scored.size(), 12); ++i) {
            filtered_candidates.push_back(scored[i].second);
        }
    }

    // Determine the 4 extreme diagonal corners of the bounding quad
    Point2Df tl = filtered_candidates[0];
    Point2Df tr = filtered_candidates[0];
    Point2Df br = filtered_candidates[0];
    Point2Df bl = filtered_candidates[0];

    float min_sum = std::numeric_limits<float>::max();
    float max_sum = -std::numeric_limits<float>::max();
    float min_diff = std::numeric_limits<float>::max();
    float max_diff = -std::numeric_limits<float>::max();

    for (const auto& pt : filtered_candidates) {
        float sum = pt.x + pt.y;
        float diff = pt.y - pt.x;

        if (sum < min_sum) {
            min_sum = sum;
            tl = pt;
        }
        if (sum > max_sum) {
            max_sum = sum;
            br = pt;
        }
        if (diff < min_diff) {
            min_diff = diff;
            tr = pt;
        }
        if (diff > max_diff) {
            max_diff = diff;
            bl = pt;
        }
    }

    // Verify all 4 points are distinct
    if (tl.distanceTo(tr) < 20.0 || tl.distanceTo(bl) < 20.0 ||
        br.distanceTo(tr) < 20.0 || br.distanceTo(bl) < 20.0) {
        return std::nullopt;
    }

    Quad2D quad(tl, tr, br, bl);
    if (quad.area() < (w * h * 0.05)) {
        // Quad area is unreasonably small (< 5% of photo)
        return std::nullopt;
    }

    return quad;
}

} // namespace libreomr
