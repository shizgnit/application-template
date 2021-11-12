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

    EXPECT_EQ(test.glyphs.size(), 256);
    EXPECT_EQ(test.kernings.size(), 91);
    EXPECT_EQ(test.pages.size(), 1);

    EXPECT_EQ(test.pages[0].raster.size(), 2097152);
}

TEST_F(FormatTest, MTL) {
    format::mtl test(testDataPath() + "poly.mtl");

    // Materials return as lists
    std::vector<type::material> mats;
    test >> mats;

    EXPECT_EQ(mats.size(), 1);
    EXPECT_EQ(mats[0].map.raster.size(), 33554432);
}

TEST_F(FormatTest, OBJ) {
    format::obj test(testDataPath() + "untitled.obj");

    // Materials return as lists
    std::vector<type::object> objs;
    test >> objs;

    EXPECT_EQ(objs.size(), 1);
    EXPECT_EQ(objs[0].texture.map.raster.size(), 33554432);
}

TEST_F(FormatTest, FBX) {
    format::fbx test(testDataPath() + "untitled.fbx");

    // Materials return as lists
    //std::vector<type::object> fbxs;
    //test >> fbxs;

    //EXPECT_EQ(fbxs.size(), 1);
    //EXPECT_EQ(fbxs[0].texture.map.raster.size(), 33554432);
}

TEST_F(FormatTest, FBX_Bones) {
    format::fbx test(testDataPath() + "wiggle.fbx");

    // Materials return as lists
    std::vector<type::object> fbxs;
    test >> fbxs;

    EXPECT_EQ(fbxs.size(), 1);
    //EXPECT_EQ(fbxs[0].texture.map.raster.size(), 33554432);
}