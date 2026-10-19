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

#include "libreomr/binarization.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

namespace libreomr {

uint8_t Binarizer::computeOtsuThreshold(const Image& gray_image) {
    if (gray_image.empty()) return 128;

    uint32_t w = gray_image.width();
    uint32_t h = gray_image.height();
    uint32_t total_pixels = w * h;
    if (total_pixels == 0) return 128;

    // Compute histogram
    double histogram[256] = {0.0};
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            uint8_t val = gray_image.at(x, y, 0);
            histogram[val] += 1.0;
        }
    }

    // Normalize histogram
    for (int i = 0; i < 256; ++i) {
        histogram[i] /= static_cast<double>(total_pixels);
    }

    double total_mean = 0.0;
    for (int i = 0; i < 256; ++i) {
        total_mean += i * histogram[i];
    }

    double max_variance = -1.0;
    int best_t_start = 128;
    int best_t_end = 128;

    double w0 = 0.0;
    double m0 = 0.0;

    for (int t = 0; t < 256; ++t) {
        w0 += histogram[t];
        if (w0 <= 0.0) continue;

        double w1 = 1.0 - w0;
        if (w1 <= 0.0) break;

        m0 += t * histogram[t];
        double mu0 = m0 / w0;
        double mu1 = (total_mean - m0) / w1;

        double diff = mu0 - mu1;
        double between_class_variance = w0 * w1 * diff * diff;

        if (between_class_variance > max_variance + 1e-9) {
            max_variance = between_class_variance;
            best_t_start = t;
            best_t_end = t;
        } else if (max_variance > 0.0 && std::abs(between_class_variance - max_variance) <= 1e-9) {
            best_t_end = t;
        }
    }

    return static_cast<uint8_t>((best_t_start + best_t_end) / 2);
}

Image Binarizer::threshold(const Image& gray_image, uint8_t thresh, bool invert) {
    uint32_t w = gray_image.width();
    uint32_t h = gray_image.height();
    Image binary(w, h, 1);

    uint8_t fg = invert ? 255 : 0;
    uint8_t bg = invert ? 0 : 255;

    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            uint8_t val = gray_image.at(x, y, 0);
            binary.at(x, y, 0) = (val <= thresh) ? fg : bg;
        }
    }

    return binary;
}

Image Binarizer::otsu(const Image& gray_image, bool invert, uint8_t* out_threshold) {
    Image gray = (gray_image.channels() > 1) ? gray_image.toGrayscale() : gray_image;
    uint8_t t = computeOtsuThreshold(gray);
    if (out_threshold) *out_threshold = t;
    return threshold(gray, t, invert);
}

Image Binarizer::adaptiveThreshold(const Image& gray_image, int window_size, int c_offset, bool invert) {
    Image gray = (gray_image.channels() > 1) ? gray_image.toGrayscale() : gray_image;
    uint32_t w = gray.width();
    uint32_t h = gray.height();

    if (w == 0 || h == 0) return gray;

    // Compute integral image
    std::vector<uint64_t> integral(static_cast<size_t>(w + 1) * (h + 1), 0);
    for (uint32_t y = 0; y < h; ++y) {
        uint64_t row_sum = 0;
        for (uint32_t x = 0; x < w; ++x) {
            row_sum += gray.at(x, y, 0);
            size_t idx = (static_cast<size_t>(y + 1) * (w + 1)) + (x + 1);
            size_t up_idx = (static_cast<size_t>(y) * (w + 1)) + (x + 1);
            integral[idx] = integral[up_idx] + row_sum;
        }
    }

    Image binary(w, h, 1);
    int half_win = window_size / 2;

    uint8_t fg = invert ? 255 : 0;
    uint8_t bg = invert ? 0 : 255;

    for (int y = 0; y < static_cast<int>(h); ++y) {
        int y1 = std::max(0, y - half_win);
        int y2 = std::min(static_cast<int>(h) - 1, y + half_win);

        for (int x = 0; x < static_cast<int>(w); ++x) {
            int x1 = std::max(0, x - half_win);
            int x2 = std::min(static_cast<int>(w) - 1, x + half_win);

            uint32_t count = static_cast<uint32_t>(x2 - x1 + 1) * (y2 - y1 + 1);

            size_t i_tl = static_cast<size_t>(y1) * (w + 1) + x1;
            size_t i_tr = static_cast<size_t>(y1) * (w + 1) + (x2 + 1);
            size_t i_bl = static_cast<size_t>(y2 + 1) * (w + 1) + x1;
            size_t i_br = static_cast<size_t>(y2 + 1) * (w + 1) + (x2 + 1);

            uint64_t sum = integral[i_br] - integral[i_tr] - integral[i_bl] + integral[i_tl];
            int local_mean = static_cast<int>(sum / count);
            int local_thresh = local_mean - c_offset;

            uint8_t pixel_val = gray.at(static_cast<uint32_t>(x), static_cast<uint32_t>(y), 0);
            binary.at(static_cast<uint32_t>(x), static_cast<uint32_t>(y), 0) =
                (pixel_val <= local_thresh) ? fg : bg;
        }
    }

    return binary;
}

Image Binarizer::erode(const Image& binary_image, int iterations) {
    if (iterations <= 0) return binary_image.clone();

    uint32_t w = binary_image.width();
    uint32_t h = binary_image.height();
    Image current = binary_image.clone();
    Image next(w, h, 1);

    for (int iter = 0; iter < iterations; ++iter) {
        for (int y = 0; y < static_cast<int>(h); ++y) {
            for (int x = 0; x < static_cast<int>(w); ++x) {
                // If any pixel in 3x3 neighborhood is 255 (background), output is 255
                uint8_t max_val = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    int ny = std::clamp(y + dy, 0, static_cast<int>(h) - 1);
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = std::clamp(x + dx, 0, static_cast<int>(w) - 1);
                        uint8_t v = current.at(static_cast<uint32_t>(nx), static_cast<uint32_t>(ny), 0);
                        if (v > max_val) max_val = v;
                    }
                }
                next.at(static_cast<uint32_t>(x), static_cast<uint32_t>(y), 0) = max_val;
            }
        }
        current = next.clone();
    }
    return current;
}

Image Binarizer::dilate(const Image& binary_image, int iterations) {
    if (iterations <= 0) return binary_image.clone();

    uint32_t w = binary_image.width();
    uint32_t h = binary_image.height();
    Image current = binary_image.clone();
    Image next(w, h, 1);

    for (int iter = 0; iter < iterations; ++iter) {
        for (int y = 0; y < static_cast<int>(h); ++y) {
            for (int x = 0; x < static_cast<int>(w); ++x) {
                // If any pixel in 3x3 neighborhood is 0 (foreground black), output is 0
                uint8_t min_val = 255;
                for (int dy = -1; dy <= 1; ++dy) {
                    int ny = std::clamp(y + dy, 0, static_cast<int>(h) - 1);
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = std::clamp(x + dx, 0, static_cast<int>(w) - 1);
                        uint8_t v = current.at(static_cast<uint32_t>(nx), static_cast<uint32_t>(ny), 0);
                        if (v < min_val) min_val = v;
                    }
                }
                next.at(static_cast<uint32_t>(x), static_cast<uint32_t>(y), 0) = min_val;
            }
        }
        current = next.clone();
    }
    return current;
}

Image Binarizer::open(const Image& binary_image, int iterations) {
    return dilate(erode(binary_image, iterations), iterations);
}

Image Binarizer::close(const Image& binary_image, int iterations) {
    return erode(dilate(binary_image, iterations), iterations);
}

Image Binarizer::invert(const Image& binary_image) {
    uint32_t w = binary_image.width();
    uint32_t h = binary_image.height();
    Image out(w, h, 1);
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            out.at(x, y, 0) = 255 - binary_image.at(x, y, 0);
        }
    }
    return out;
}

} // namespace libreomr
