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

#ifndef LIBREOMR_GEOMETRY_HPP
#define LIBREOMR_GEOMETRY_HPP

#include "libreomr/export.h"
#include "libreomr/types.h"
#include <array>
#include <cmath>
#include <vector>

namespace libreomr {

template <typename T>
struct Point2D {
    T x{0};
    T y{0};

    constexpr Point2D() = default;
    constexpr Point2D(T x_, T y_) : x(x_), y(y_) {}

    constexpr Point2D operator+(const Point2D& other) const { return {x + other.x, y + other.y}; }
    constexpr Point2D operator-(const Point2D& other) const { return {x - other.x, y - other.y}; }
    constexpr Point2D operator*(T scalar) const { return {x * scalar, y * scalar}; }
    constexpr Point2D operator/(T scalar) const { return {x / scalar, y / scalar}; }

    Point2D& operator+=(const Point2D& other) { x += other.x; y += other.y; return *this; }
    Point2D& operator-=(const Point2D& other) { x -= other.x; y -= other.y; return *this; }

    [[nodiscard]] double distanceTo(const Point2D& other) const {
        double dx = static_cast<double>(x - other.x);
        double dy = static_cast<double>(y - other.y);
        return std::sqrt(dx * dx + dy * dy);
    }

    [[nodiscard]] double length() const {
        return std::sqrt(static_cast<double>(x * x + y * y));
    }

    [[nodiscard]] libreomr_point_t toC() const {
        return {static_cast<float>(x), static_cast<float>(y)};
    }

    static Point2D fromC(const libreomr_point_t& pt) {
        return {static_cast<T>(pt.x), static_cast<T>(pt.y)};
    }
};

using Point2Df = Point2D<float>;
using Point2Dd = Point2D<double>;
using Point2Di = Point2D<int32_t>;

template <typename T>
struct Rect {
    T x{0};
    T y{0};
    T width{0};
    T height{0};

    constexpr Rect() = default;
    constexpr Rect(T x_, T y_, T width_, T height_) : x(x_), y(y_), width(width_), height(height_) {}

    [[nodiscard]] constexpr T left() const { return x; }
    [[nodiscard]] constexpr T top() const { return y; }
    [[nodiscard]] constexpr T right() const { return x + width; }
    [[nodiscard]] constexpr T bottom() const { return y + height; }

    [[nodiscard]] Point2D<T> center() const {
        return {x + width / static_cast<T>(2), y + height / static_cast<T>(2)};
    }

    [[nodiscard]] constexpr T area() const { return width * height; }

    [[nodiscard]] constexpr bool contains(const Point2D<T>& pt) const {
        return pt.x >= x && pt.x <= (x + width) && pt.y >= y && pt.y <= (y + height);
    }

    [[nodiscard]] libreomr_rect_t toC() const {
        return {static_cast<float>(x), static_cast<float>(y),
                static_cast<float>(width), static_cast<float>(height)};
    }

    static Rect fromC(const libreomr_rect_t& r) {
        return {static_cast<T>(r.x), static_cast<T>(r.y),
                static_cast<T>(r.width), static_cast<T>(r.height)};
    }
};

using Rectf = Rect<float>;
using Recti = Rect<int32_t>;

struct LIBREOMR_API Quad2D {
    Point2Df top_left;
    Point2Df top_right;
    Point2Df bottom_right;
    Point2Df bottom_left;

    Quad2D() = default;
    Quad2D(Point2Df tl, Point2Df tr, Point2Df br, Point2Df bl);

    [[nodiscard]] Rectf boundingBox() const;
    [[nodiscard]] double area() const;
    [[nodiscard]] libreomr_quad_t toC() const;
    static Quad2D fromC(const libreomr_quad_t& q);

    static Quad2D orderPoints(const std::array<Point2Df, 4>& pts);
};

class LIBREOMR_API Matrix3x3 {
public:
    std::array<double, 9> m{1.0, 0.0, 0.0,
                            0.0, 1.0, 0.0,
                            0.0, 0.0, 1.0};

    Matrix3x3();
    explicit Matrix3x3(const std::array<double, 9>& elements);

    static Matrix3x3 identity();
    static Matrix3x3 translation(double tx, double ty);
    static Matrix3x3 scaling(double sx, double sy);
    static Matrix3x3 rotation(double radians);

    [[nodiscard]] Matrix3x3 operator*(const Matrix3x3& other) const;
    [[nodiscard]] Point2Df transform(const Point2Df& pt) const;
    [[nodiscard]] Matrix3x3 inverse(bool* success = nullptr) const;

    static Matrix3x3 computeHomography(const Quad2D& src, const Quad2D& dst, bool* success = nullptr);
};

} // namespace libreomr

#endif /* LIBREOMR_GEOMETRY_HPP */
