#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_GOOGLETEST

#include <gtest/gtest.h>

TEST(SpatialTest, Vector) {
    spatial::vector foo(1.0, 1.0, 1.0);

    auto value = foo.length();

    EXPECT_TRUE(value);
}

/*
TEST(SpatialTest, Matrix) {
    spatial::matrix foo = { {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0} };
}

TEST(SpatialTest, Lighting) {
    spatial::matrix model = { {4,0,0,0},
                              {0,4,0,0},
                              {0,0,4,0},
                              {0,0,0,1} };
}

TEST(SpatialTest, Position) {
    type::info inf = { { "something" }, type::format::FORMAT_FNT };
}

TEST(SpatialTest, Projection) {
    spatial::matrix model;
    model.translate(400, 400, 0);

    float width = 600.0f;
    float height = 400.0f;

    spatial::matrix ortho;
    ortho.ortho(0, width, 0, height);

    spatial::triangle t1;
    t1.vertices[0](256.0f, 256.0f, 0.0f);
    t1.vertices[1](256.0f, 0.0f, 0.0f);
    t1.vertices[2](0.0f, 0.0f, 0.0f);

    t1.project(model, spatial::matrix(), ortho);

    t1.vertices[0].w = 0.0f;
    t1.vertices[1].w = 0.0f;
    t1.vertices[2].w = 0.0f;

    spatial::vector point(120.0f, 120.0f, 1.0f, 0.0f);

    spatial::vector projected = point.project(spatial::matrix(), spatial::matrix(), ortho);

    projected.w = 0.0f;

    spatial::ray r1(projected - spatial::vector(0, 0, 200), projected - spatial::vector(0, 0, -200));

    EXPECT_TRUE(r1.intersects(t1));
}
*/

TEST(SpatialTest, RayDistance) {

    spatial::ray ray(spatial::vector(-1.0f, -1.0f, -1.0f), spatial::vector(1.0f, 1.0f, 1.0f));

    spatial::vector point(0.0f, 0.0f, 1.0f, 0.0f);

    auto value = ray.distance(point);
    EXPECT_EQ(value, 0.816496611f);
}

TEST(SpatialTest, PlaneIntersection) {

    spatial::triangle triangle;

    triangle.vertices.resize(3);

    triangle.vertices[0](0.0f, 0.0f, 0.0f, 0.0f);
    triangle.vertices[1](2.0f, 0.0f, 0.0f, 0.0f);
    triangle.vertices[2](0.0f, 2.0f, 0.0f, 0.0f);

    spatial::ray ray(spatial::vector(1.0f, 1.0f, 2.0f), spatial::vector(0.0f, 0.0f, -1.0f));

    spatial::plane plane;

    plane.point(0.0f, 0.0f, 0.0f, 0.0f);
    plane.normal = triangle.normal();

    auto intersects = ray.intersects(plane);

    EXPECT_TRUE(intersects);

    auto intersection = ray.intersection(plane);

    EXPECT_EQ(intersection, spatial::vector(1.0f, 1.0f, 0.0f));
}

TEST(SpatialTest, TriangleIntersection) {

    spatial::triangle triangle;

    triangle.vertices.resize(3);

    triangle.vertices[0](0.0f, 0.0f, 0.0f);
    triangle.vertices[1](2.0f, 0.0f, 0.0f);
    triangle.vertices[2](0.0f, 2.0f, 0.0f);

    spatial::ray ray(spatial::vector(1.0f, 1.0f, 2.0f), spatial::vector(0.0f, 0.0f, -1.0f));

    auto intersects = ray.intersects(triangle);

    EXPECT_TRUE(intersects);

    auto intersection = ray.intersection(triangle);

    spatial::vector reference(1.0f, 1.0f, 0.0f);

    EXPECT_EQ(intersection.x, reference.x);
    EXPECT_EQ(intersection.y, reference.y);
    EXPECT_EQ(intersection.z, reference.z);
}

TEST(SpatialTest, TriangleNormal) {
    spatial::triangle t1;

    t1.vertices.resize(3);

    t1.vertices[0](0.0f, 0.0f, 0.0f, 0.0f);
    t1.vertices[1](2.0f, 0.0f, 0.0f, 0.0f);
    t1.vertices[2](0.0f, 2.0f, 0.0f, 0.0f);

    EXPECT_EQ(t1.normal(), spatial::vector(0.0f, 0.0f, 1.0f));

    spatial::triangle t2;

    t2.vertices.resize(3);

    t2.vertices[0](512.0f, 0.0f, 0.0f, 0.0f);
    t2.vertices[1](0.0f, 512.0f, 0.0f, 0.0f);
    t2.vertices[2](0.0f, 0.0f, 0.0f, 0.0f);

    EXPECT_EQ(t2.normal(), spatial::vector(0.0f, 0.0f, 1.0f));

    spatial::triangle t3;

    t3.vertices.resize(3);

    t3.vertices[0](0.0f, 0.0f, 0.0f, 0.0f);
    t3.vertices[1](512.0f, 0.0f, 0.0f, 0.0f);
    t3.vertices[2](0.0f, 512.0f, 0.0f, 0.0f);

    EXPECT_EQ(t3.normal(), spatial::vector(0.0f, 0.0f, 1.0f));
}

TEST(SpatialTest, GeometryReplication) {

    spatial::quad q1(256, 256);

    spatial::quad q2;

    q2 = q1;

    EXPECT_EQ(q2.vertices.size(), 6);

    type::object o1;

    o1 = q1;

    EXPECT_EQ(o1.vertices.size(), 6);
}

TEST(SpatialTest, LookAt) {

    auto result = spatial::matrix().lookat({5.39245, 15.1404, -1959.04, 1}, {-0.453372, 0.862182, -0.226043, 1}, {4.62085, 14.6338, -1959.43, 1});

}

#endif
