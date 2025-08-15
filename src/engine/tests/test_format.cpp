#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_GOOGLETEST

#include <gtest/gtest.h>

TEST(FormatTest, WAV) {
    format::wav test_wav(test->getTestDataPath() + "GLaDOS.wav");
    EXPECT_EQ(test_wav.size, 238200);
    EXPECT_EQ(test_wav.properties.sample_rate, 44100);
}

TEST(FormatTest, PNG) {
    format::png test_png(test->getTestDataPath() + "marvin.png");
    EXPECT_EQ(test_png.properties.width, 295);
    EXPECT_EQ(test_png.properties.height, 281);
    EXPECT_EQ(test_png.raster.size(), 2652640);

    type::image stream;
    test_png >> stream;
    EXPECT_EQ(stream.properties.width, 295);
    EXPECT_EQ(stream.properties.height, 281);
    EXPECT_EQ(stream.raster.size(), 2652640);
}

TEST(FormatTest, FNT) {
    format::fnt test_fnt(test->getTestDataPath() + "arial.fnt");
    EXPECT_EQ(test_fnt.glyphs.size(), 256);
    EXPECT_EQ(test_fnt.kernings.size(), 91);
    EXPECT_EQ(test_fnt.pages.size(), 1);
    EXPECT_EQ(test_fnt.pages[0].raster.size(), 2097152);
}

TEST(FormatTest, MTL) {
    format::mtl test_mtl(test->getTestDataPath() + "poly.mtl");
    std::vector<type::material> mats;
    //EXPECT_EQ(test_mtl[0].color->raster.size(), 33554432);
}

TEST(FormatTest, OBJ) {
    format::obj test_obj(test->getTestDataPath() + "untitled.obj");
    std::vector<std::shared_ptr<type::object>> objs;
    test_obj >> objs;
    EXPECT_EQ(objs.size(), 0);
    //EXPECT_EQ(objs[0]->texture.color->raster.size(), 33554432);
}

TEST(FormatTest, FBX) {
    format::fbx test_fbx(test->getTestDataPath() + "untitled.fbx");
    //std::vector<type::object> fbxs;
    //test_fbx >> fbxs;
    //EXPECT_EQ(fbxs.size(), 1);
    //EXPECT_EQ(fbxs[0].texture.map.raster.size(), 33554432);
}

TEST(FormatTest, FBX_Bones) {
    format::fbx test_fbx_bones(test->getTestDataPath() + "wiggle.fbx");
    std::vector<std::shared_ptr<type::object>> fbxs;
    test_fbx_bones >> fbxs;
    //EXPECT_EQ(fbxs.size(), 1);
    //EXPECT_EQ(fbxs[0].texture.map.raster.size(), 33554432);
}

#endif