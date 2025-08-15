#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_GOOGLETEST

#include <gtest/gtest.h>

TEST(UtilitiesTest, Indices) {
    std::vector<std::string> list;
    list.push_back("one");
    list.push_back("two");
    list.push_back("three");

    auto index = utilities::indices(list);
    EXPECT_EQ(index.size(), 3);
    EXPECT_EQ(index[0], 0);
    EXPECT_EQ(index[1], 1);
    EXPECT_EQ(index[2], 2);
}

TEST(UtilitiesTest, RangeElements) {
    auto range = utilities::range(3);
    EXPECT_EQ(range.size(), 3);
    EXPECT_EQ(range[0], 0);
    EXPECT_EQ(range[1], 1);
    EXPECT_EQ(range[2], 2);
}

TEST(UtilitiesTest, RangeStartEnd) {
    auto range = utilities::range(1, 3);
    EXPECT_EQ(range.size(), 3);
    EXPECT_EQ(range[0], 1);
    EXPECT_EQ(range[1], 2);
    EXPECT_EQ(range[2], 3);
}

TEST(UtilitiesTest, NumericToString) {
    EXPECT_STREQ(utilities::type_cast<std::string>('a').c_str(), "a");
    EXPECT_STREQ(utilities::type_cast<std::string>(12345).c_str(), "12345");
    // Severity	Code	Description	Project	File	Line	Suppression State
    // Error	C2440	'<function-style-cast>': cannot convert from '__int64' to 'utilities::type_cast<std::string>'	windows-client.Executable	C:\Projects\private\template\src\engine\tests\utilities.cpp	32	
    //    EXPECT_STREQ(utilities::type_cast<std::string>(2147483648), "2147483648");
    EXPECT_STREQ(utilities::type_cast<std::string>(4.0009).c_str(), "4.000900");
}

TEST(UtilitiesTest, StringToNumeric) {
    EXPECT_EQ((double)utilities::type_cast<double>("4.000900"), 4.0009);
    EXPECT_EQ((unsigned long)utilities::type_cast<unsigned long>("2147483652"), 2147483647);
}

TEST(UtilitiesTest, RightTrim) {
    EXPECT_STREQ(utilities::rtrim("  FOOBAR  ").c_str(), "  FOOBAR");
}

TEST(UtilitiesTest, LeftTrim) {
    EXPECT_STREQ(utilities::ltrim("  FOOBAR  ").c_str(), "FOOBAR  ");
}

TEST(UtilitiesTest, Trim) {
    EXPECT_STREQ(utilities::trim("  FOOBAR  ").c_str(), "FOOBAR");
}

TEST(UtilitiesTest, LowerCase) {
    EXPECT_STREQ(utilities::lc("  FOOBAR  ").c_str(), "  foobar  ");
}

TEST(UtilitiesTest, UpperCase) {
    EXPECT_STREQ(utilities::uc("  foobar  ").c_str(), "  FOOBAR  ");
}

TEST(UtilitiesTest, Dirname) {
    EXPECT_STREQ(utilities::dirname("/foo/bar/baz.txt").c_str(), "/foo/bar");
    EXPECT_STREQ(utilities::dirname("c:\\foo\\bar\\baz.txt").c_str(), "c:\\foo\\bar");
}


/*
// This doesn't work, but currently don't intend to use this method
TEST(UtilitiesTest, Format) {
    EXPECT_STREQ(utilities::format("  %s%i  ", "FOOBAR", 1).c_str(), "  FOOBAR1  ");
}
*/

TEST(UtilitiesTest, Join) {
    EXPECT_STREQ(utilities::join(", ", { "FOO", "BAR" } ).c_str(), "FOO, BAR");
}

TEST(UtilitiesTest, Tokenize) {
    auto tokens = utilities::tokenize("FOO, BAR,BAZ", ",");

    EXPECT_EQ(tokens.size(), 3);
    EXPECT_STREQ(tokens[0].c_str(), "FOO");
    EXPECT_STREQ(tokens[1].c_str(), " BAR");
    EXPECT_STREQ(tokens[2].c_str(), "BAZ");
}

TEST(UtilitiesTest, Base) {
    EXPECT_STREQ(utilities::base("12", 16, 10).c_str(), "C");
}

TEST(UtilitiesTest, UUID) {
    auto uuid = utilities::uuid();
    EXPECT_EQ(uuid.length() > 0, true);
}

#endif
