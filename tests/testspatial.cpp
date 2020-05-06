#include "gtest/gtest.h"

#include "tests.hpp"

class SpatialTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    // void TearDown() override {}
};

TEST(SpatialTest, Vector) {
    spatial::vector foo(1.0, 1.0, 1.0);

    auto value = foo.length();

    EXPECT_TRUE(value);
}

TEST(SpatialTest, Matrix) {
    spatial::matrix foo = { {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0} };

    //EXPECT_EQ();
}

TEST(SpatialTest, Position) {

    type::info inf = { { "something" }, type::format::FORMAT_FNT };

    //EXPECT_EQ();
}

TEST(SpatialTest, RayDistance) {

    spatial::ray ray;

    ray.point(-1.0f, -1.0f, -1.0f, 0.0f);
    ray.direction(1.0f, 1.0f, 1.0f, 0.0f);

    spatial::vector point(0.0f, 0.0f, 1.0f, 0.0f);

    auto value = ray.distance(point);
    EXPECT_EQ(value, 0.816496611f);
}

TEST(SpatialTest, PlaneIntersection) {

    spatial::triangle triangle;

    triangle.vertices.resize(3);

    triangle.vertices[0].coordinate(0.0f, 0.0f, 0.0f, 0.0f);
    triangle.vertices[1].coordinate(2.0f, 0.0f, 0.0f, 0.0f);
    triangle.vertices[2].coordinate(0.0f, 2.0f, 0.0f, 0.0f);

    spatial::ray ray;

    ray.point(1.0f, 1.0f, 2.0f, 0.0f);
    ray.direction(1.0f, 1.0f, 1.0f, 0.0f);

    spatial::plane plane;

    plane.point(0.0f, 0.0f, 0.0f, 0.0f);
    plane.normal = triangle.normal();

    auto intersects = ray.intersects(plane);

    EXPECT_TRUE(intersects);

    auto intersection = ray.intersection(plane);

    EXPECT_EQ(intersection, spatial::vector(-1.0f, -1.0f, 0.0f));
}

TEST(SpatialTest, TriangleIntersection) {

    spatial::triangle triangle;

    triangle.vertices.resize(3);

    triangle.vertices[0].coordinate(0.0f, 0.0f, 0.0f, 0.0f);
    triangle.vertices[1].coordinate(2.0f, 0.0f, 0.0f, 0.0f);
    triangle.vertices[2].coordinate(0.0f, 2.0f, 0.0f, 0.0f);

    spatial::ray ray;

    ray.point(1.0f, 1.0f, 2.0f, 0.0f);
    ray.direction(1.0f, 1.0f, 1.0f, 0.0f);

    auto intersects = ray.intersects(triangle);

    EXPECT_TRUE(intersects);

    spatial::plane plane;

    plane.point(0.0f, 0.0f, 0.0f, 0.0f);
    plane.normal = triangle.normal();

    auto intersection = ray.intersection(triangle);

    EXPECT_EQ(intersection, spatial::vector(-1.0f, -1.0f, 0.0f));
}