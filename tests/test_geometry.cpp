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

#include "test_framework.hpp"
#include "libreomr/geometry.hpp"

using namespace libreomr;

TEST_CASE(TestPoint2DOperations) {
    Point2Df p1(10.0f, 20.0f);
    Point2Df p2(40.0f, 60.0f);

    Point2Df p3 = p1 + p2;
    ASSERT_EQ(p3.x, 50.0f);
    ASSERT_EQ(p3.y, 80.0f);

    ASSERT_NEAR(p1.distanceTo(p2), 50.0, 1e-4);
}

TEST_CASE(TestRectOperations) {
    Rectf r(10.0f, 20.0f, 100.0f, 50.0f);
    ASSERT_EQ(r.area(), 5000.0f);
    ASSERT_EQ(r.left(), 10.0f);
    ASSERT_EQ(r.right(), 110.0f);
    ASSERT_EQ(r.top(), 20.0f);
    ASSERT_EQ(r.bottom(), 70.0f);

    Point2Df inside(50.0f, 40.0f);
    Point2Df outside(5.0f, 10.0f);
    ASSERT_TRUE(r.contains(inside));
    ASSERT_FALSE(r.contains(outside));
}

TEST_CASE(TestQuad2DOrderPoints) {
    // Unordered points: bottom-right, top-left, bottom-left, top-right
    std::array<Point2Df, 4> unordered = {
        Point2Df(100.0f, 200.0f),
        Point2Df(0.0f, 0.0f),
        Point2Df(0.0f, 200.0f),
        Point2Df(100.0f, 0.0f)
    };

    Quad2D ordered = Quad2D::orderPoints(unordered);
    ASSERT_EQ(ordered.top_left.x, 0.0f);
    ASSERT_EQ(ordered.top_left.y, 0.0f);
    ASSERT_EQ(ordered.top_right.x, 100.0f);
    ASSERT_EQ(ordered.top_right.y, 0.0f);
    ASSERT_EQ(ordered.bottom_right.x, 100.0f);
    ASSERT_EQ(ordered.bottom_right.y, 200.0f);
    ASSERT_EQ(ordered.bottom_left.x, 0.0f);
    ASSERT_EQ(ordered.bottom_left.y, 200.0f);
}

TEST_CASE(TestMatrix3x3Homography) {
    // Mapping a unit square [0,0]-[100,100] to a scaled/translated quad [50,50]-[250,250]
    Quad2D src(Point2Df(0, 0), Point2Df(100, 0), Point2Df(100, 100), Point2Df(0, 100));
    Quad2D dst(Point2Df(50, 50), Point2Df(250, 50), Point2Df(250, 250), Point2Df(50, 250));

    bool success = false;
    Matrix3x3 h = Matrix3x3::computeHomography(src, dst, &success);
    ASSERT_TRUE(success);

    Point2Df mapped_tl = h.transform(src.top_left);
    Point2Df mapped_br = h.transform(src.bottom_right);

    ASSERT_NEAR(mapped_tl.x, dst.top_left.x, 0.01);
    ASSERT_NEAR(mapped_tl.y, dst.top_left.y, 0.01);
    ASSERT_NEAR(mapped_br.x, dst.bottom_right.x, 0.01);
    ASSERT_NEAR(mapped_br.y, dst.bottom_right.y, 0.01);

    // Test inverse
    bool inv_ok = false;
    Matrix3x3 inv_h = h.inverse(&inv_ok);
    ASSERT_TRUE(inv_ok);

    Point2Df back_tl = inv_h.transform(dst.top_left);
    ASSERT_NEAR(back_tl.x, src.top_left.x, 0.01);
    ASSERT_NEAR(back_tl.y, src.top_left.y, 0.01);
}
