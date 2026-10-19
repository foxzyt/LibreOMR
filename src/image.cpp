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

#include "libreomr/image.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>

namespace libreomr {

Image::Image() = default;

Image::Image(uint32_t width, uint32_t height, uint32_t channels)
    : width_(width), height_(height), channels_(channels),
      data_(static_cast<size_t>(width) * height * channels, 255) {}

Image::Image(uint32_t width, uint32_t height, uint32_t channels, const uint8_t* raw_data)
    : width_(width), height_(height), channels_(channels) {
    size_t total = static_cast<size_t>(width) * height * channels;
    if (raw_data) {
        data_.assign(raw_data, raw_data + total);
    } else {
        data_.resize(total, 255);
    }
}

Image::Image(const Image& other)
    : width_(other.width_), height_(other.height_), channels_(other.channels_), data_(other.data_) {}

Image::Image(Image&& other) noexcept
    : width_(other.width_), height_(other.height_), channels_(other.channels_), data_(std::move(other.data_)) {
    other.width_ = 0;
    other.height_ = 0;
    other.channels_ = 0;
}

Image& Image::operator=(const Image& other) {
    if (this != &other) {
        width_ = other.width_;
        height_ = other.height_;
        channels_ = other.channels_;
        data_ = other.data_;
    }
    return *this;
}

Image& Image::operator=(Image&& other) noexcept {
    if (this != &other) {
        width_ = other.width_;
        height_ = other.height_;
        channels_ = other.channels_;
        data_ = std::move(other.data_);
        other.width_ = 0;
        other.height_ = 0;
        other.channels_ = 0;
    }
    return *this;
}

uint8_t& Image::at(uint32_t x, uint32_t y, uint32_t channel) {
    if (x >= width_ || y >= height_ || channel >= channels_) {
        throw std::out_of_range("Image pixel index out of bounds");
    }
    return data_[(static_cast<size_t>(y) * width_ + x) * channels_ + channel];
}

const uint8_t& Image::at(uint32_t x, uint32_t y, uint32_t channel) const {
    if (x >= width_ || y >= height_ || channel >= channels_) {
        throw std::out_of_range("Image pixel index out of bounds");
    }
    return data_[(static_cast<size_t>(y) * width_ + x) * channels_ + channel];
}

uint8_t Image::sampleBilinear(double x, double y, uint32_t channel) const {
    if (x < 0.0 || x >= width_ - 1 || y < 0.0 || y >= height_ - 1) {
        // Clamp to nearest border pixel
        int cx = std::clamp(static_cast<int>(std::round(x)), 0, static_cast<int>(width_ - 1));
        int cy = std::clamp(static_cast<int>(std::round(y)), 0, static_cast<int>(height_ - 1));
        return at(static_cast<uint32_t>(cx), static_cast<uint32_t>(cy), channel);
    }

    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    double dx = x - x0;
    double dy = y - y0;

    double p00 = at(x0, y0, channel);
    double p10 = at(x1, y0, channel);
    double p01 = at(x0, y1, channel);
    double p11 = at(x1, y1, channel);

    double top = p00 * (1.0 - dx) + p10 * dx;
    double bottom = p01 * (1.0 - dx) + p11 * dx;
    double val = top * (1.0 - dy) + bottom * dy;

    return static_cast<uint8_t>(std::clamp(std::round(val), 0.0, 255.0));
}

void Image::fill(uint8_t value) {
    std::fill(data_.begin(), data_.end(), value);
}

void Image::clear() {
    width_ = 0;
    height_ = 0;
    channels_ = 0;
    data_.clear();
}

Image Image::toGrayscale() const {
    if (channels_ == 1) {
        return clone();
    }

    Image gray(width_, height_, 1);
    for (uint32_t y = 0; y < height_; ++y) {
        for (uint32_t x = 0; x < width_; ++x) {
            uint8_t r = at(x, y, 0);
            uint8_t g = at(x, y, 1);
            uint8_t b = at(x, y, 2);
            // ITU-R standard luminosity weights
            uint8_t lum = static_cast<uint8_t>(std::clamp(
                0.299 * r + 0.587 * g + 0.114 * b, 0.0, 255.0));
            gray.at(x, y, 0) = lum;
        }
    }
    return gray;
}

Image Image::toRGB() const {
    if (channels_ == 3) {
        return clone();
    }

    Image rgb(width_, height_, 3);
    if (channels_ == 1) {
        for (uint32_t y = 0; y < height_; ++y) {
            for (uint32_t x = 0; x < width_; ++x) {
                uint8_t val = at(x, y, 0);
                rgb.at(x, y, 0) = val;
                rgb.at(x, y, 1) = val;
                rgb.at(x, y, 2) = val;
            }
        }
    } else if (channels_ == 4) {
        for (uint32_t y = 0; y < height_; ++y) {
            for (uint32_t x = 0; x < width_; ++x) {
                rgb.at(x, y, 0) = at(x, y, 0);
                rgb.at(x, y, 1) = at(x, y, 1);
                rgb.at(x, y, 2) = at(x, y, 2);
            }
        }
    }
    return rgb;
}

Image Image::clone() const {
    return Image(*this);
}

Image Image::crop(const Recti& rect) const {
    int rx = std::max(0, rect.x);
    int ry = std::max(0, rect.y);
    int rw = std::min(static_cast<int>(width_) - rx, rect.width);
    int rh = std::min(static_cast<int>(height_) - ry, rect.height);

    if (rw <= 0 || rh <= 0) {
        return Image(0, 0, channels_);
    }

    Image cropped(static_cast<uint32_t>(rw), static_cast<uint32_t>(rh), channels_);
    for (int y = 0; y < rh; ++y) {
        for (int x = 0; x < rw; ++x) {
            for (uint32_t c = 0; c < channels_; ++c) {
                cropped.at(x, y, c) = at(rx + x, ry + y, c);
            }
        }
    }
    return cropped;
}

Image Image::warpPerspective(const Matrix3x3& homography, uint32_t out_width, uint32_t out_height, uint8_t background_val) const {
    Image out(out_width, out_height, channels_);
    out.fill(background_val);

    bool inv_ok = false;
    Matrix3x3 inv_h = homography.inverse(&inv_ok);
    if (!inv_ok) {
        return out;
    }

    for (uint32_t y = 0; y < out_height; ++y) {
        for (uint32_t x = 0; x < out_width; ++x) {
            Point2Df src_pt = inv_h.transform({static_cast<float>(x), static_cast<float>(y)});
            if (src_pt.x >= 0.0f && src_pt.x < static_cast<float>(width_) &&
                src_pt.y >= 0.0f && src_pt.y < static_cast<float>(height_)) {
                for (uint32_t c = 0; c < channels_; ++c) {
                    out.at(x, y, c) = sampleBilinear(src_pt.x, src_pt.y, c);
                }
            }
        }
    }
    return out;
}

Image Image::rotate(double degrees, uint8_t background_val) const {
    double rad = degrees * 3.14159265358979323846 / 180.0;
    double cx = width_ / 2.0;
    double cy = height_ / 2.0;

    Matrix3x3 to_origin = Matrix3x3::translation(-cx, -cy);
    Matrix3x3 rot = Matrix3x3::rotation(rad);
    Matrix3x3 from_origin = Matrix3x3::translation(cx, cy);

    Matrix3x3 transform = from_origin * rot * to_origin;
    return warpPerspective(transform, width_, height_, background_val);
}

Image Image::boxBlur(int radius) const {
    if (radius <= 0) return clone();

    Image result(width_, height_, channels_);
    int kernel_size = 2 * radius + 1;

    // Horizontal pass
    Image temp(width_, height_, channels_);
    for (uint32_t c = 0; c < channels_; ++c) {
        for (uint32_t y = 0; y < height_; ++y) {
            int sum = 0;
            // Initialize window
            for (int k = -radius; k <= radius; ++k) {
                int px = std::clamp(k, 0, static_cast<int>(width_ - 1));
                sum += at(px, y, c);
            }
            temp.at(0, y, c) = static_cast<uint8_t>(sum / kernel_size);

            for (uint32_t x = 1; x < width_; ++x) {
                int prev_px = std::clamp(static_cast<int>(x) - radius - 1, 0, static_cast<int>(width_ - 1));
                int next_px = std::clamp(static_cast<int>(x) + radius, 0, static_cast<int>(width_ - 1));
                sum += at(next_px, y, c) - at(prev_px, y, c);
                temp.at(x, y, c) = static_cast<uint8_t>(sum / kernel_size);
            }
        }
    }

    // Vertical pass
    for (uint32_t c = 0; c < channels_; ++c) {
        for (uint32_t x = 0; x < width_; ++x) {
            int sum = 0;
            for (int k = -radius; k <= radius; ++k) {
                int py = std::clamp(k, 0, static_cast<int>(height_ - 1));
                sum += temp.at(x, py, c);
            }
            result.at(x, 0, c) = static_cast<uint8_t>(sum / kernel_size);

            for (uint32_t y = 1; y < height_; ++y) {
                int prev_py = std::clamp(static_cast<int>(y) - radius - 1, 0, static_cast<int>(height_ - 1));
                int next_py = std::clamp(static_cast<int>(y) + radius, 0, static_cast<int>(height_ - 1));
                sum += temp.at(x, next_py, c) - temp.at(x, prev_py, c);
                result.at(x, y, c) = static_cast<uint8_t>(sum / kernel_size);
            }
        }
    }

    return result;
}

Image Image::gaussianBlur5x5() const {
    static const int kernel[5][5] = {
        {1,  4,  6,  4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1,  4,  6,  4, 1}
    };
    static const int kernel_sum = 256;

    Image out(width_, height_, channels_);
    for (uint32_t c = 0; c < channels_; ++c) {
        for (int y = 0; y < static_cast<int>(height_); ++y) {
            for (int x = 0; x < static_cast<int>(width_); ++x) {
                int acc = 0;
                for (int ky = -2; ky <= 2; ++ky) {
                    int py = std::clamp(y + ky, 0, static_cast<int>(height_ - 1));
                    for (int kx = -2; kx <= 2; ++kx) {
                        int px = std::clamp(x + kx, 0, static_cast<int>(width_ - 1));
                        acc += at(px, py, c) * kernel[ky + 2][kx + 2];
                    }
                }
                out.at(x, y, c) = static_cast<uint8_t>(acc / kernel_sum);
            }
        }
    }
    return out;
}

void Image::drawLine(Point2Di p1, Point2Di p2, const std::vector<uint8_t>& color, int thickness) {
    int x0 = p1.x, y0 = p1.y;
    int x1 = p2.x, y1 = p2.y;
    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (true) {
        if (thickness <= 1) {
            if (x0 >= 0 && x0 < static_cast<int>(width_) && y0 >= 0 && y0 < static_cast<int>(height_)) {
                for (size_t c = 0; c < std::min(color.size(), static_cast<size_t>(channels_)); ++c) {
                    at(x0, y0, static_cast<uint32_t>(c)) = color[c];
                }
            }
        } else {
            drawFilledCircle({x0, y0}, thickness / 2, color);
        }

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Image::drawCircle(Point2Di center, int radius, const std::vector<uint8_t>& color, int thickness) {
    if (radius <= 0) return;

    for (int t = 0; t < thickness; ++t) {
        int r = radius - t;
        if (r < 0) break;
        int x = 0;
        int y = r;
        int d = 3 - 2 * r;

        auto plot8 = [&](int cx, int cy, int px, int py) {
            int pts[8][2] = {
                {cx + px, cy + py}, {cx - px, cy + py}, {cx + px, cy - py}, {cx - px, cy - py},
                {cx + py, cy + px}, {cx - py, cy + px}, {cx + py, cy - px}, {cx - py, cy - px}
            };
            for (auto& pt : pts) {
                if (pt[0] >= 0 && pt[0] < static_cast<int>(width_) && pt[1] >= 0 && pt[1] < static_cast<int>(height_)) {
                    for (size_t c = 0; c < std::min(color.size(), static_cast<size_t>(channels_)); ++c) {
                        at(pt[0], pt[1], static_cast<uint32_t>(c)) = color[c];
                    }
                }
            }
        };

        plot8(center.x, center.y, x, y);
        while (y >= x) {
            x++;
            if (d > 0) {
                y--;
                d = d + 4 * (x - y) + 10;
            } else {
                d = d + 4 * x + 6;
            }
            plot8(center.x, center.y, x, y);
        }
    }
}

void Image::drawFilledCircle(Point2Di center, int radius, const std::vector<uint8_t>& color) {
    if (radius <= 0) return;
    int r2 = radius * radius;
    int min_y = std::max(0, center.y - radius);
    int max_y = std::min(static_cast<int>(height_) - 1, center.y + radius);

    for (int y = min_y; y <= max_y; ++y) {
        int dy = y - center.y;
        int dx = static_cast<int>(std::sqrt(r2 - dy * dy));
        int min_x = std::max(0, center.x - dx);
        int max_x = std::min(static_cast<int>(width_) - 1, center.x + dx);

        for (int x = min_x; x <= max_x; ++x) {
            for (size_t c = 0; c < std::min(color.size(), static_cast<size_t>(channels_)); ++c) {
                at(x, y, static_cast<uint32_t>(c)) = color[c];
            }
        }
    }
}

void Image::drawRect(const Recti& rect, const std::vector<uint8_t>& color, int thickness) {
    if (rect.width <= 0 || rect.height <= 0) return;
    int r = rect.right() - 1;
    int b = rect.bottom() - 1;
    drawLine({rect.x, rect.y}, {r, rect.y}, color, thickness);
    drawLine({r, rect.y}, {r, b}, color, thickness);
    drawLine({r, b}, {rect.x, b}, color, thickness);
    drawLine({rect.x, b}, {rect.x, rect.y}, color, thickness);
}

void Image::drawFilledRect(const Recti& rect, const std::vector<uint8_t>& color) {
    int min_x = std::max(0, rect.x);
    int max_x = std::min(static_cast<int>(width_), rect.right());
    int min_y = std::max(0, rect.y);
    int max_y = std::min(static_cast<int>(height_), rect.bottom());

    for (int y = min_y; y < max_y; ++y) {
        for (int x = min_x; x < max_x; ++x) {
            for (size_t c = 0; c < std::min(color.size(), static_cast<size_t>(channels_)); ++c) {
                at(x, y, static_cast<uint32_t>(c)) = color[c];
            }
        }
    }
}

void Image::drawQuad(const Quad2D& quad, const std::vector<uint8_t>& color, int thickness) {
    Point2Di tl(static_cast<int>(quad.top_left.x), static_cast<int>(quad.top_left.y));
    Point2Di tr(static_cast<int>(quad.top_right.x), static_cast<int>(quad.top_right.y));
    Point2Di br(static_cast<int>(quad.bottom_right.x), static_cast<int>(quad.bottom_right.y));
    Point2Di bl(static_cast<int>(quad.bottom_left.x), static_cast<int>(quad.bottom_left.y));

    drawLine(tl, tr, color, thickness);
    drawLine(tr, br, color, thickness);
    drawLine(br, bl, color, thickness);
    drawLine(bl, tl, color, thickness);
}

} // namespace libreomr
