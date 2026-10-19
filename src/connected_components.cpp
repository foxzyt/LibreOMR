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

#include "libreomr/connected_components.hpp"
#include <algorithm>
#include <unordered_map>
#include <vector>

namespace libreomr {

namespace {

class DisjointSet {
public:
    explicit DisjointSet(size_t initial_capacity = 1024) {
        parent_.reserve(initial_capacity);
        parent_.push_back(0); // 0 is background
    }

    int32_t add() {
        auto id = static_cast<int32_t>(parent_.size());
        parent_.push_back(id);
        return id;
    }

    int32_t find(int32_t i) {
        if (parent_[i] == i) return i;
        return parent_[i] = find(parent_[i]);
    }

    void unite(int32_t i, int32_t j) {
        int32_t root_i = find(i);
        int32_t root_j = find(j);
        if (root_i != root_j) {
            if (root_i < root_j) {
                parent_[root_j] = root_i;
            } else {
                parent_[root_i] = root_j;
            }
        }
    }

private:
    std::vector<int32_t> parent_;
};

} // namespace

std::vector<Blob> ConnectedComponents::extract(const Image& binary_image, uint8_t foreground_val) {
    std::vector<Blob> blobs;
    if (binary_image.empty()) return blobs;

    uint32_t w = binary_image.width();
    uint32_t h = binary_image.height();

    std::vector<int32_t> labels(static_cast<size_t>(w) * h, 0);
    DisjointSet dsu(w * h / 16);

    // Pass 1: Labeling with 8-connectivity
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            if (binary_image.at(x, y, 0) != foreground_val) {
                continue;
            }

            int32_t neighbors[4] = {0, 0, 0, 0};
            int count = 0;

            // Left
            if (x > 0) {
                int32_t l = labels[y * w + (x - 1)];
                if (l > 0) neighbors[count++] = l;
            }
            // Top-left
            if (x > 0 && y > 0) {
                int32_t l = labels[(y - 1) * w + (x - 1)];
                if (l > 0) neighbors[count++] = l;
            }
            // Top
            if (y > 0) {
                int32_t l = labels[(y - 1) * w + x];
                if (l > 0) neighbors[count++] = l;
            }
            // Top-right
            if (x + 1 < w && y > 0) {
                int32_t l = labels[(y - 1) * w + (x + 1)];
                if (l > 0) neighbors[count++] = l;
            }

            if (count == 0) {
                int32_t new_label = dsu.add();
                labels[y * w + x] = new_label;
            } else {
                int32_t min_label = neighbors[0];
                for (int i = 1; i < count; ++i) {
                    dsu.unite(min_label, neighbors[i]);
                    if (neighbors[i] < min_label) {
                        min_label = neighbors[i];
                    }
                }
                labels[y * w + x] = min_label;
            }
        }
    }

    // Pass 2: Canonicalize roots & map to contiguous IDs
    std::unordered_map<int32_t, int32_t> root_to_index;

    struct ComponentAccumulator {
        uint32_t area{0};
        int32_t min_x{99999999};
        int32_t max_x{-1};
        int32_t min_y{99999999};
        int32_t max_y{-1};
        double sum_x{0.0};
        double sum_y{0.0};
        bool touches_border{false};
    };

    std::vector<ComponentAccumulator> accums;

    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            int32_t lbl = labels[y * w + x];
            if (lbl <= 0) continue;

            int32_t root = dsu.find(lbl);
            auto it = root_to_index.find(root);
            int32_t idx = 0;
            if (it == root_to_index.end()) {
                idx = static_cast<int32_t>(accums.size());
                root_to_index[root] = idx;
                accums.emplace_back();
            } else {
                idx = it->second;
            }

            auto& acc = accums[idx];
            acc.area++;
            acc.sum_x += x;
            acc.sum_y += y;
            if (static_cast<int32_t>(x) < acc.min_x) acc.min_x = static_cast<int32_t>(x);
            if (static_cast<int32_t>(x) > acc.max_x) acc.max_x = static_cast<int32_t>(x);
            if (static_cast<int32_t>(y) < acc.min_y) acc.min_y = static_cast<int32_t>(y);
            if (static_cast<int32_t>(y) > acc.max_y) acc.max_y = static_cast<int32_t>(y);

            if (x == 0 || x == w - 1 || y == 0 || y == h - 1) {
                acc.touches_border = true;
            }
        }
    }

    // Assemble final Blobs
    blobs.reserve(accums.size());
    for (size_t i = 0; i < accums.size(); ++i) {
        const auto& acc = accums[i];
        if (acc.area == 0) continue;

        Blob b;
        b.label = static_cast<int32_t>(i + 1);
        b.area = acc.area;
        int bw = acc.max_x - acc.min_x + 1;
        int bh = acc.max_y - acc.min_y + 1;
        b.bbox = Recti(acc.min_x, acc.min_y, bw, bh);
        b.centroid = Point2Df(static_cast<float>(acc.sum_x / acc.area), static_cast<float>(acc.sum_y / acc.area));
        b.aspect_ratio = (bh > 0) ? (static_cast<float>(bw) / static_cast<float>(bh)) : 1.0f;
        b.solidity = (bw * bh > 0) ? (static_cast<float>(acc.area) / static_cast<float>(bw * bh)) : 0.0f;
        b.touches_border = acc.touches_border;

        blobs.push_back(b);
    }

    return blobs;
}

} // namespace libreomr
