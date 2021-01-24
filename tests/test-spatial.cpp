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

float deg_to_radf_2(float deg) {
    return deg * (float)M_PI / 180.0f;
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

    spatial::ray ray(spatial::vector(1.0f, 1.0f, 2.0f), spatial::vector(1.0f, 1.0f, 1.0f));

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

    spatial::ray ray(spatial::vector(1.0f, 1.0f, 2.0f), spatial::vector(1.0f, 1.0f, -1.0f));

    auto intersects = ray.intersects(triangle);

    EXPECT_TRUE(intersects);

    spatial::plane plane;

    plane.point(0.0f, 0.0f, 0.0f, 0.0f);
    plane.normal = triangle.normal();

    auto intersection = ray.intersection(triangle);

    EXPECT_EQ(intersection, spatial::vector(1.0f, 1.0f, 0.0f));
    
    glm::vec3 r0 = glm::vec3( 1.0f, 1.0f, -30.0f );
    glm::vec3 r1 = glm::vec3( 1.0f, 1.0f, 30.0f );

    glm::vec3 dir = glm::normalize(r1 - r0);

    glm::vec3 v0 = glm::vec3( 0.0f, 0.0f, 0.0f );
    glm::vec3 v1 = glm::vec3( 0.0f, 256.0f, 0.0f );
    glm::vec3 v2 = glm::vec3( 256.0f, 256.0f, 0.0f );

    glm::vec3 pos = { 0.0f, 0.0f, 0.0f };

    auto inter = glm::intersectLineTriangle(r0, dir, v0, v1, v2, pos);

    /*
    glm::vec3 result;

    const glm::vec3 edge1 = v1 - v0;
    const glm::vec3 edge2 = v2 - v0;
    const glm::vec3 pvec = glm::cross(r1, edge2);
    const float det = glm::dot(edge1, pvec);

    if (det > -0.0001 && det < 0.0001) {
        //return false;
    }

    const float invDet = 1.0f / det;

    const glm::vec3 tvec = r0 - v0;

    result.x = glm::dot(tvec, pvec) * invDet;
    if (result.x < 0.0f || result.x > 1.0f) {
        //return false;
    }
    const glm::vec3 qvec = glm::cross(tvec, edge1);

    result.y = glm::dot(r1, qvec) * invDet;
    if (result.y < 0.0f || result.x + result.y > 1.0f) {
        //return false;
    }

    result.z = glm::dot(edge2, qvec) * invDet;

    //return true;
    */
}

TEST(SpatialTest, TriangleIntersection2) {

    spatial::triangle triangle;

    triangle.vertices.resize(3);

    triangle.vertices[0](512.0f, 0.0f, 0.0f, 1.0f);
    triangle.vertices[1](0.0f, 512.0f, 0.0f, 1.0f);
    triangle.vertices[2](0.0f, 0.0f, 0.0f, 1.0f);

    spatial::ray ray(spatial::vector(1.0f, 1.0f, 2.0f, 1.0f), spatial::vector(1.0f, 1.0f, -1.0f, 1.0f));

    auto intersects = ray.intersects(triangle);

    EXPECT_TRUE(intersects);

    spatial::plane plane;

    plane.point(0.0f, 0.0f, 0.0f, 0.0f);
    plane.normal = triangle.normal();

    auto intersection = ray.intersection(triangle);

    EXPECT_EQ(intersection, spatial::vector(1.0f, 1.0f, 0.0f));
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