#include "gtest/gtest.h"

#include "tests.hpp"

class FormatWavTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    // void TearDown() override {}
};

TEST(FormatWavTest, Read) {
    format::wav test(testDataPath() + "GLaDOS.wav");
    EXPECT_EQ(test.size, 238200);
}
