/*
================================================================================
  Copyright (c) 2023, Pandemos
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/


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
    std::vector<type::object> objs;
    test_obj >> objs;
    EXPECT_EQ(objs.size(), 1);
    EXPECT_EQ(objs[0].texture.color->raster.size(), 33554432);
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
    std::vector<type::object> fbxs;
    test_fbx_bones >> fbxs;
    EXPECT_EQ(fbxs.size(), 1);
    //EXPECT_EQ(fbxs[0].texture.map.raster.size(), 33554432);
}

#endif