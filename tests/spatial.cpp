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

    EXPECT_EQ(foo.length(), 1.0);
}

TEST(SpatialTest, Matrix) {
    spatial::matrix foo = { {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0} };

    //EXPECT_EQ();
}

TEST(SpatialTest, Position) {

    type::info inf = { { "something" }, type::format::FORMAT_FNT };

    //EXPECT_EQ();
}
