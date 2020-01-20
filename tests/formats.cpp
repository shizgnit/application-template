#include "gtest/gtest.h"

#include "tests.hpp"

class FormatTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    // void TearDown() override {}
};

TEST(FormatTest, WAV) {
    format::wav test(testDataPath() + "GLaDOS.wav");
    EXPECT_EQ(test.size, 238200);
    EXPECT_EQ(test.properties.sample_rate, 44100);

}

TEST(FormatTest, PNG) {
    format::png test(testDataPath() + "marvin.png");
    EXPECT_EQ(test.properties.width, 295);
    EXPECT_EQ(test.properties.height, 281);
}

