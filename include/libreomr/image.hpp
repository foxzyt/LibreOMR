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

#ifndef LIBREOMR_IMAGE_HPP
#define LIBREOMR_IMAGE_HPP

#include "libreomr/export.h"
#include "libreomr/geometry.hpp"
#include "libreomr/types.h"
#include <cstdint>
#include <string>
#include <vector>

namespace libreomr {

class LIBREOMR_API Image {
public:
    Image();
    Image(uint32_t width, uint32_t height, uint32_t channels = 1);
    Image(uint32_t width, uint32_t height, uint32_t channels, const uint8_t* raw_data);
    ~Image() = default;

    Image(const Image& other);
    Image(Image&& other) noexcept;
    Image& operator=(const Image& other);
    Image& operator=(Image&& other) noexcept;

    [[nodiscard]] bool empty() const noexcept { return data_.empty(); }
    [[nodiscard]] uint32_t width() const noexcept { return width_; }
    [[nodiscard]] uint32_t height() const noexcept { return height_; }
    [[nodiscard]] uint32_t channels() const noexcept { return channels_; }
    [[nodiscard]] size_t stride() const noexcept { return static_cast<size_t>(width_) * channels_; }
    [[nodiscard]] size_t sizeBytes() const noexcept { return data_.size(); }

    [[nodiscard]] uint8_t* data() noexcept { return data_.data(); }
    [[nodiscard]] const uint8_t* data() const noexcept { return data_.data(); }
    [[nodiscard]] const std::vector<uint8_t>& buffer() const noexcept { return data_; }

    [[nodiscard]] uint8_t& at(uint32_t x, uint32_t y, uint32_t channel = 0);
    [[nodiscard]] const uint8_t& at(uint32_t x, uint32_t y, uint32_t channel = 0) const;

    [[nodiscard]] uint8_t sampleBilinear(double x, double y, uint32_t channel = 0) const;

    void fill(uint8_t value);
    void clear();

    [[nodiscard]] Image toGrayscale() const;
    [[nodiscard]] Image toRGB() const;
    [[nodiscard]] Image clone() const;
    [[nodiscard]] Image crop(const Recti& rect) const;

    [[nodiscard]] Image warpPerspective(const Matrix3x3& homography, uint32_t out_width, uint32_t out_height, uint8_t background_val = 255) const;
    [[nodiscard]] Image rotate(double degrees, uint8_t background_val = 255) const;

    [[nodiscard]] Image boxBlur(int radius) const;
    [[nodiscard]] Image gaussianBlur5x5() const;

    void drawLine(Point2Di p1, Point2Di p2, const std::vector<uint8_t>& color, int thickness = 1);
    void drawCircle(Point2Di center, int radius, const std::vector<uint8_t>& color, int thickness = 1);
    void drawFilledCircle(Point2Di center, int radius, const std::vector<uint8_t>& color);
    void drawRect(const Recti& rect, const std::vector<uint8_t>& color, int thickness = 1);
    void drawFilledRect(const Recti& rect, const std::vector<uint8_t>& color);
    void drawQuad(const Quad2D& quad, const std::vector<uint8_t>& color, int thickness = 2);
    void drawText(Point2Di pos, const std::string& text, const std::vector<uint8_t>& color, int scale = 1);

    static Image loadFromFile(const std::string& filepath);
    static Image loadFromMemory(const uint8_t* buffer, size_t length);
    bool saveToFile(const std::string& filepath) const;
    [[nodiscard]] std::vector<uint8_t> encodePng() const;

private:
    uint32_t width_{0};
    uint32_t height_{0};
    uint32_t channels_{0};
    std::vector<uint8_t> data_;
};

} // namespace libreomr

#endif /* LIBREOMR_IMAGE_HPP */
