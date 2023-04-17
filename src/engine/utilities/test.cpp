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

TEST(FilesystemTest, FilterFileType, []() {
    EXPECT_EQ(filesystem->read_directory(tests->data(), "directory").size(), 1);
    EXPECT_EQ(filesystem->read_directory(tests->data(), "regular").size(), 13);
    EXPECT_EQ(filesystem->read_directory(tests->data(), "foobar").size(), 0);
});

TEST(FilesystemTest, PresentWorkingDirectory, []() {
    auto pwd = filesystem->pwd();
    EXPECT_FALSE(pwd.empty());
});

TEST(FilesystemTest, Exists, []() {
    EXPECT_TRUE(filesystem->exists(tests->data("filesystem") + "foobar.txt"));
});

TEST(FilesystemTest, Manipulation, []() {
    auto testfile_source = tests->data("filesystem") + "foobar.txt";

    auto testfile_copy = tests->data("filesystem") + "foobar-copy-" + utilities::uuid() + ".txt";
    auto testfile_move = tests->data("filesystem") + "foobar-move-" + utilities::uuid() + ".txt";

    EXPECT_TRUE(filesystem->cp(testfile_source, testfile_copy));
    EXPECT_TRUE(filesystem->exists(testfile_copy));

    EXPECT_TRUE(filesystem->mv(testfile_copy, testfile_move));
    EXPECT_FALSE(filesystem->exists(testfile_copy));
    EXPECT_TRUE(filesystem->exists(testfile_move));

    /// TODO: determine why this is failing
    EXPECT_FALSE(filesystem->rm(testfile_move));

    /// For not use this as an opportunity to test failure
    auto error = filesystem->error();

    EXPECT_EQ(error.first, 5);
    EXPECT_STREQ(error.second.c_str(), "Access is denied.\r\n");

    // EXPECT_FALSE(filesystem->exists(testfile_move));
});

TEST(FilesystemTest, ReadDirectory, []() {
    auto contents = filesystem->read_directory(tests->data());

    EXPECT_TRUE(contents.size() > 0);
});

TEST(UtilitiesTest, Indices, []() {
    std::vector<std::string> list;
    list.push_back("one");
    list.push_back("two");
    list.push_back("three");

    auto index = utilities::indices(list);
    EXPECT_EQ(index.size(), 3);
    EXPECT_EQ(index[0], 0);
    EXPECT_EQ(index[1], 1);
    EXPECT_EQ(index[2], 2);
});

TEST(UtilitiesTest, RangeElements, []() {
    auto range = utilities::range(3);
    EXPECT_EQ(range.size(), 3);
    EXPECT_EQ(range[0], 0);
    EXPECT_EQ(range[1], 1);
    EXPECT_EQ(range[2], 2);
});

TEST(UtilitiesTest, RangeStartEnd, []() {
    auto range = utilities::range(1, 3);
    EXPECT_EQ(range.size(), 3);
    EXPECT_EQ(range[0], 1);
    EXPECT_EQ(range[1], 2);
    EXPECT_EQ(range[2], 3);
});

TEST(UtilitiesTest, NumericToString, []() {
    EXPECT_STREQ(utilities::type_cast<std::string>('a'), "a");
    EXPECT_STREQ(utilities::type_cast<std::string>(12345), "12345");
    // Severity	Code	Description	Project	File	Line	Suppression State
    // Error	C2440	'<function-style-cast>': cannot convert from '__int64' to 'utilities::type_cast<std::string>'	windows-client.Executable	C:\Projects\private\template\src\engine\tests\utilities.cpp	32	
    //    EXPECT_STREQ(utilities::type_cast<std::string>(2147483648), "2147483648");
    EXPECT_STREQ(utilities::type_cast<std::string>(4.0009), "4.000900");
});

TEST(UtilitiesTest, StringToNumeric, []() {
    EXPECT_EQ((double)utilities::type_cast<double>("4.000900"), 4.0009);
    EXPECT_EQ((unsigned long)utilities::type_cast<unsigned long>("2147483652"), 2147483647);
});

TEST(UtilitiesTest, RightTrim, []() {
    EXPECT_STREQ(utilities::rtrim("  FOOBAR  ").c_str(), "  FOOBAR");
});

TEST(UtilitiesTest, LeftTrim, []() {
    EXPECT_STREQ(utilities::ltrim("  FOOBAR  ").c_str(), "FOOBAR  ");
});

TEST(UtilitiesTest, Trim, []() {
    EXPECT_STREQ(utilities::trim("  FOOBAR  ").c_str(), "FOOBAR");
});

TEST(UtilitiesTest, LowerCase, []() {
    EXPECT_STREQ(utilities::lc("  FOOBAR  ").c_str(), "  foobar  ");
});

TEST(UtilitiesTest, UpperCase, []() {
    EXPECT_STREQ(utilities::uc("  foobar  ").c_str(), "  FOOBAR  ");
});

TEST(UtilitiesTest, Dirname, []() {
    EXPECT_STREQ(utilities::dirname("/foo/bar/baz.txt").c_str(), "/foo/bar");
    EXPECT_STREQ(utilities::dirname("c:\\foo\\bar\\baz.txt").c_str(), "c:\\foo\\bar");
});


/*
// This doesn't work, but currently don't intend to use this method
TEST(UtilitiesTest, Format, []() {
    EXPECT_STREQ(utilities::format("  %s%i  ", "FOOBAR", 1).c_str(), "  FOOBAR1  ");
});
*/

TEST(UtilitiesTest, Join, []() {
    EXPECT_STREQ(utilities::join(", ", { "FOO", "BAR" } ).c_str(), "FOO, BAR");
});

TEST(UtilitiesTest, Tokenize, []() {
    auto tokens = utilities::tokenize("FOO, BAR,BAZ", ",");

    EXPECT_EQ(tokens.size(), 3);
    EXPECT_STREQ(tokens[0].c_str(), "FOO");
    EXPECT_STREQ(tokens[1].c_str(), " BAR");
    EXPECT_STREQ(tokens[2].c_str(), "BAZ");
});

TEST(UtilitiesTest, Base, []() {
    EXPECT_STREQ(utilities::base("12", 16, 10).c_str(), "C");
});

TEST(UtilitiesTest, UUID, []() {
    auto uuid = utilities::uuid();
    EXPECT_EQ(uuid.length() > 0, true);
});

/* // Tests left to add
std::string join(std::string, std::vector<std::string> arguments);
std::vector<std::string> split(std::string expression, std::string buffer, int limit = 0);

std::vector<char*> segment(char* input, char delimiter = '\0');
std::vector<std::string> tokenize(std::string input, std::string delimiter = "");

std::string substitute(std::string input, std::string expression, std::string replacement, bool global = true);
std::string substitute(std::string input, std::vector<std::string>& replacement);

std::string base(std::string in, int target_ordinal, int current_ordinal = 10);
std::string base64(std::string in);
*/

std::vector<platform::input::event> events;

TEST(InputTest, PointerDirect, []() {
    platform::input* input = new implementation::universal::input();

    events.clear();

    input->handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event& ev) {
        events.push_back(ev);
        }, 1);

    input->raise({ platform::input::POINTER, platform::input::MOVE, 1, 2, 0.0f, { 1.0f, 1.0f, 1.0f } });

    EXPECT_EQ(events.size(), 1);
    delete input;
});

TEST(InputTest, PointerDrag, []() {
    platform::input* input = new implementation::universal::input();

    events.clear();

    input->handler(platform::input::POINTER, platform::input::DRAG, [](const platform::input::event& ev) {
        events.push_back(ev);
        }, 1);

    input->handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event& ev) {
        events.push_back(ev);
        }, 1);

    input->raise({ platform::input::POINTER, platform::input::DOWN, 1, 2, 0.0f, { 0.0f, 0.0f, 0.0f } });
    input->raise({ platform::input::POINTER, platform::input::MOVE, 1, 3, 0.0f, { 1.0f, 1.0f, 1.0f } });
    input->raise({ platform::input::POINTER, platform::input::MOVE, 1, 4, 0.0f, { 1.0f, 1.0f, 1.0f } });
    input->raise({ platform::input::POINTER, platform::input::UP, 1, 2, 0.0f, { 0.0f, 0.0f, 0.0f } });

    // Should not be a DRAG action without a index
    input->raise({ platform::input::POINTER, platform::input::MOVE, 1, 4, 0.0f, { 1.0f, 1.0f, 1.0f } });

    EXPECT_EQ(events.size(), 3);
    delete input;
});

TEST(InputTest, PointerDoubleTap, []() {
    platform::input* input = new implementation::universal::input();

    events.clear();

    input->handler(platform::input::POINTER, platform::input::DOWN, [](const platform::input::event& ev) {
        events.push_back(ev);
        }, 1);

    input->handler(platform::input::POINTER, platform::input::DOUBLE, [](const platform::input::event& ev) {
        events.push_back(ev);
        }, 1);

    input->raise({ platform::input::POINTER, platform::input::DOWN, 1, 2, 0.0f, { 0.0f, 0.0f, 0.0f } });
    input->raise({ platform::input::POINTER, platform::input::UP, 1, 2, 0.0f, { 0.0f, 0.0f, 0.0f } });

    input->raise({ platform::input::POINTER, platform::input::DOWN, 1, 3, 0.0f, { 0.0f, 0.1f, 0.0f } });
    input->raise({ platform::input::POINTER, platform::input::UP, 1, 3, 0.0f, { 0.0f, 0.1f, 0.0f } });

    EXPECT_EQ(events.size(), 2);
    delete input;
});

TEST(SpatialTest, Vector, []() {
    spatial::vector foo(1.0, 1.0, 1.0);

    auto value = foo.length();

    EXPECT_TRUE(value);
});

/*
TEST(SpatialTest, Matrix, []() {
    spatial::matrix foo = { {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0} };
});

TEST(SpatialTest, Lighting, []() {
    spatial::matrix model = { {4,0,0,0},
                              {0,4,0,0},
                              {0,0,4,0},
                              {0,0,0,1} };
});

TEST(SpatialTest, Position, []() {
    type::info inf = { { "something" }, type::format::FORMAT_FNT };
});
*/

TEST(SpatialTest, Projection, []() {
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
});

TEST(SpatialTest, RayDistance, []() {

    spatial::ray ray(spatial::vector(-1.0f, -1.0f, -1.0f), spatial::vector(1.0f, 1.0f, 1.0f));

    spatial::vector point(0.0f, 0.0f, 1.0f, 0.0f);

    auto value = ray.distance(point);
    EXPECT_EQ(value, 0.816496611f);
});

TEST(SpatialTest, PlaneIntersection, []() {

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
});

TEST(SpatialTest, TriangleIntersection, []() {

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
    
    /*
    glm::vec3 result;

    const glm::vec3 edge1 = v1 - v0;
    const glm::vec3 edge2 = v2 - v0;
    const glm::vec3 pvec = glm::cross(r1, edge2);
    const float det = glm::dot(edge1, pvec);

    if (det > -0.0001 && det < 0.0001, []() {
        //return false;
    }

    const float invDet = 1.0f / det;

    const glm::vec3 tvec = r0 - v0;

    result.x = glm::dot(tvec, pvec) * invDet;
    if (result.x < 0.0f || result.x > 1.0f, []() {
        //return false;
    }
    const glm::vec3 qvec = glm::cross(tvec, edge1);

    result.y = glm::dot(r1, qvec) * invDet;
    if (result.y < 0.0f || result.x + result.y > 1.0f, []() {
        //return false;
    }

    result.z = glm::dot(edge2, qvec) * invDet;

    //return true;
    */
});

TEST(SpatialTest, TriangleIntersection2, []() {

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
});

TEST(SpatialTest, TriangleNormal, []() {
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
});

TEST(SpatialTest, GeometryReplication, []() {

    spatial::quad q1(256, 256);

    spatial::quad q2;

    q2 = q1;

    EXPECT_EQ(q2.vertices.size(), 6);

    type::object o1;

    o1 = q1;

    EXPECT_EQ(o1.vertices.size(), 6);
});

TEST(SpatialTest, LookAt, []() {

    auto result = spatial::matrix().lookat({5.39245, 15.1404, -1959.04, 1}, {-0.453372, 0.862182, -0.226043, 1}, {4.62085, 14.6338, -1959.43, 1});

});

TEST(FormatTest, WAV, []() {
    format::wav test(tests->data() + "GLaDOS.wav");
    EXPECT_EQ(test.size, 238200);
    EXPECT_EQ(test.properties.sample_rate, 44100);
});

TEST(FormatTest, PNG, []() {
    format::png test(tests->data() + "marvin.png");
    EXPECT_EQ(test.properties.width, 295);
    EXPECT_EQ(test.properties.height, 281);
    EXPECT_EQ(test.raster.size(), 2652640);

    type::image stream;

    test >> stream;
    EXPECT_EQ(stream.properties.width, 295);
    EXPECT_EQ(stream.properties.height, 281);
    EXPECT_EQ(stream.raster.size(), 2652640);
});

TEST(FormatTest, FNT, []() {
    format::fnt test(tests->data() + "arial.fnt");

    EXPECT_EQ(test.glyphs.size(), 256);
    EXPECT_EQ(test.kernings.size(), 91);
    EXPECT_EQ(test.pages.size(), 1);

    EXPECT_EQ(test.pages[0].raster.size(), 2097152);
});

TEST(FormatTest, MTL, []() {
    format::mtl test(tests->data() + "poly.mtl");

    // Materials return as lists
    std::vector<type::material> mats;
    test >> mats;

    EXPECT_EQ(mats.size(), 1);
    EXPECT_EQ(mats[0].color->raster.size(), 33554432);
});

TEST(FormatTest, OBJ, []() {
    format::obj test(tests->data() + "untitled.obj");

    // Materials return as lists
    std::vector<type::object> objs;
    test >> objs;

    EXPECT_EQ(objs.size(), 1);
    EXPECT_EQ(objs[0].texture.color->raster.size(), 33554432);
});

TEST(FormatTest, FBX, []() {
    format::fbx test(tests->data() + "untitled.fbx");

    // Materials return as lists
    //std::vector<type::object> fbxs;
    //test >> fbxs;

    //EXPECT_EQ(fbxs.size(), 1);
    //EXPECT_EQ(fbxs[0].texture.map.raster.size(), 33554432);
});

TEST(FormatTest, FBX_Bones, []() {
    format::fbx test(tests->data() + "wiggle.fbx");

    // Materials return as lists
    std::vector<type::object> fbxs;
    test >> fbxs;

    EXPECT_EQ(fbxs.size(), 1);
    //EXPECT_EQ(fbxs[0].texture.map.raster.size(), 33554432);
});