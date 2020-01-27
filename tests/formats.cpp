#include "gtest/gtest.h"

#include "tests.hpp"

class FormatTest : public ::testing::Test {
protected:
    void SetUp() override {
        assets->init((void *)testDataPath().c_str());
    }

    // void TearDown() override {}
};

TEST_F(FormatTest, WAV) {
    format::wav test(testDataPath() + "GLaDOS.wav");
    EXPECT_EQ(test.size, 238200);
    EXPECT_EQ(test.properties.sample_rate, 44100);
}

TEST_F(FormatTest, PNG) {
    format::png test(testDataPath() + "marvin.png");
    EXPECT_EQ(test.properties.width, 295);
    EXPECT_EQ(test.properties.height, 281);
    EXPECT_EQ(test.raster.size(), 2652640);

    type::image stream;

    test >> stream;
    EXPECT_EQ(stream.properties.width, 295);
    EXPECT_EQ(stream.properties.height, 281);
    EXPECT_EQ(stream.raster.size(), 2652640);
}

TEST_F(FormatTest, FNT) {
    format::fnt test(testDataPath() + "arial.fnt");
}