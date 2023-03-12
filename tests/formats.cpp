/*
================================================================================
  Copyright (c) 2023, Dee E. Abbott
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
    EXPECT_EQ(mats[0].map->raster.size(), 33554432);
}

TEST_F(FormatTest, OBJ) {
    format::obj test(testDataPath() + "untitled.obj");

    // Materials return as lists
    std::vector<type::object> objs;
    test >> objs;

    EXPECT_EQ(objs.size(), 1);
    EXPECT_EQ(objs[0].texture.map->raster.size(), 33554432);
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