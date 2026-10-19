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

#include "libreomr/geometry.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace libreomr {

Quad2D::Quad2D(Point2Df tl, Point2Df tr, Point2Df br, Point2Df bl)
    : top_left(tl), top_right(tr), bottom_right(br), bottom_left(bl) {}

Rectf Quad2D::boundingBox() const {
    float min_x = std::min({top_left.x, top_right.x, bottom_right.x, bottom_left.x});
    float max_x = std::max({top_left.x, top_right.x, bottom_right.x, bottom_left.x});
    float min_y = std::min({top_left.y, top_right.y, bottom_right.y, bottom_left.y});
    float max_y = std::max({top_left.y, top_right.y, bottom_right.y, bottom_left.y});
    return {min_x, min_y, max_x - min_x, max_y - min_y};
}

double Quad2D::area() const {
    return 0.5 * std::abs(
        (top_left.x * top_right.y - top_right.x * top_left.y) +
        (top_right.x * bottom_right.y - bottom_right.x * top_right.y) +
        (bottom_right.x * bottom_left.y - bottom_left.x * bottom_right.y) +
        (bottom_left.x * top_left.y - top_left.x * bottom_left.y)
    );
}

libreomr_quad_t Quad2D::toC() const {
    return {top_left.toC(), top_right.toC(), bottom_right.toC(), bottom_left.toC()};
}

Quad2D Quad2D::fromC(const libreomr_quad_t& q) {
    return {
        Point2Df::fromC(q.top_left),
        Point2Df::fromC(q.top_right),
        Point2Df::fromC(q.bottom_right),
        Point2Df::fromC(q.bottom_left)
    };
}

Quad2D Quad2D::orderPoints(const std::array<Point2Df, 4>& pts) {
    Point2Df tl = pts[0];
    Point2Df tr = pts[0];
    Point2Df br = pts[0];
    Point2Df bl = pts[0];

    float min_sum = std::numeric_limits<float>::max();
    float max_sum = -std::numeric_limits<float>::max();
    float min_diff = std::numeric_limits<float>::max();
    float max_diff = -std::numeric_limits<float>::max();

    for (const auto& pt : pts) {
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

    return {tl, tr, br, bl};
}

Matrix3x3::Matrix3x3() : m{1, 0, 0, 0, 1, 0, 0, 0, 1} {}

Matrix3x3::Matrix3x3(const std::array<double, 9>& elements) : m(elements) {}

Matrix3x3 Matrix3x3::identity() {
    return Matrix3x3();
}

Matrix3x3 Matrix3x3::translation(double tx, double ty) {
    return Matrix3x3({1.0, 0.0, tx,
                      0.0, 1.0, ty,
                      0.0, 0.0, 1.0});
}

Matrix3x3 Matrix3x3::scaling(double sx, double sy) {
    return Matrix3x3({sx,  0.0, 0.0,
                      0.0, sy,  0.0,
                      0.0, 0.0, 1.0});
}

Matrix3x3 Matrix3x3::rotation(double radians) {
    double c = std::cos(radians);
    double s = std::sin(radians);
    return Matrix3x3({c,   -s,  0.0,
                      s,    c,  0.0,
                      0.0, 0.0, 1.0});
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& o) const {
    std::array<double, 9> res{};
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            res[r * 3 + c] = m[r * 3 + 0] * o.m[0 * 3 + c] +
                             m[r * 3 + 1] * o.m[1 * 3 + c] +
                             m[r * 3 + 2] * o.m[2 * 3 + c];
        }
    }
    return Matrix3x3(res);
}

Point2Df Matrix3x3::transform(const Point2Df& pt) const {
    double x = static_cast<double>(pt.x);
    double y = static_cast<double>(pt.y);
    double w = m[6] * x + m[7] * y + m[8];

    if (std::abs(w) < 1e-12) {
        return pt;
    }

    double inv_w = 1.0 / w;
    double nx = (m[0] * x + m[1] * y + m[2]) * inv_w;
    double ny = (m[3] * x + m[4] * y + m[5]) * inv_w;
    return {static_cast<float>(nx), static_cast<float>(ny)};
}

Matrix3x3 Matrix3x3::inverse(bool* success) const {
    double det = m[0] * (m[4] * m[8] - m[5] * m[7]) -
                 m[1] * (m[3] * m[8] - m[5] * m[6]) +
                 m[2] * (m[3] * m[7] - m[4] * m[6]);

    if (std::abs(det) < 1e-12) {
        if (success) *success = false;
        return identity();
    }

    double inv_det = 1.0 / det;
    std::array<double, 9> inv{};

    inv[0] =  (m[4] * m[8] - m[5] * m[7]) * inv_det;
    inv[1] = -(m[1] * m[8] - m[2] * m[7]) * inv_det;
    inv[2] =  (m[1] * m[5] - m[2] * m[4]) * inv_det;

    inv[3] = -(m[3] * m[8] - m[5] * m[6]) * inv_det;
    inv[4] =  (m[0] * m[8] - m[2] * m[6]) * inv_det;
    inv[5] = -(m[0] * m[5] - m[2] * m[3]) * inv_det;

    inv[6] =  (m[3] * m[7] - m[4] * m[6]) * inv_det;
    inv[7] = -(m[0] * m[7] - m[1] * m[6]) * inv_det;
    inv[8] =  (m[0] * m[4] - m[1] * m[3]) * inv_det;

    if (success) *success = true;
    return Matrix3x3(inv);
}

Matrix3x3 Matrix3x3::computeHomography(const Quad2D& src, const Quad2D& dst, bool* success) {
    const Point2Df s_pts[4] = {src.top_left, src.top_right, src.bottom_right, src.bottom_left};
    const Point2Df d_pts[4] = {dst.top_left, dst.top_right, dst.bottom_right, dst.bottom_left};

    double A[8][9] = {0};

    for (int i = 0; i < 4; ++i) {
        double xs = s_pts[i].x;
        double ys = s_pts[i].y;
        double xd = d_pts[i].x;
        double yd = d_pts[i].y;

        int row1 = i * 2;
        int row2 = row1 + 1;

        A[row1][0] = -xs;
        A[row1][1] = -ys;
        A[row1][2] = -1.0;
        A[row1][3] = 0.0;
        A[row1][4] = 0.0;
        A[row1][5] = 0.0;
        A[row1][6] = xs * xd;
        A[row1][7] = ys * xd;
        A[row1][8] = -xd;

        A[row2][0] = 0.0;
        A[row2][1] = 0.0;
        A[row2][2] = 0.0;
        A[row2][3] = -xs;
        A[row2][4] = -ys;
        A[row2][5] = -1.0;
        A[row2][6] = xs * yd;
        A[row2][7] = ys * yd;
        A[row2][8] = -yd;
    }

    for (int col = 0; col < 8; ++col) {
        int pivot_row = col;
        double max_val = std::abs(A[col][col]);
        for (int r = col + 1; r < 8; ++r) {
            double val = std::abs(A[r][col]);
            if (val > max_val) {
                max_val = val;
                pivot_row = r;
            }
        }

        if (max_val < 1e-12) {
            if (success) *success = false;
            return identity();
        }

        if (pivot_row != col) {
            for (int k = col; k <= 8; ++k) {
                std::swap(A[col][k], A[pivot_row][k]);
            }
        }

        double pivot = A[col][col];
        for (int r = col + 1; r < 8; ++r) {
            double factor = A[r][col] / pivot;
            for (int k = col; k <= 8; ++k) {
                A[r][k] -= factor * A[col][k];
            }
        }
    }

    double h[8] = {0};
    for (int r = 7; r >= 0; --r) {
        double sum = A[r][8];
        for (int c = r + 1; c < 8; ++c) {
            sum -= A[r][c] * h[c];
        }
        h[r] = sum / A[r][r];
    }

    if (success) *success = true;

    return Matrix3x3({
        h[0], h[1], h[2],
        h[3], h[4], h[5],
        h[6], h[7], 1.0
    });
}

} // namespace libreomr
