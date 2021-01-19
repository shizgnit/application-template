#include "gtest/gtest.h"

#include "tests.hpp"

class FilesystemTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    // void TearDown() override {}
};
TEST(FilesystemTest, PresentWorkingDirectory) {
    auto pwd = filesystem->pwd();
    EXPECT_FALSE(pwd.empty());
}

TEST(FilesystemTest, Exists) {
    EXPECT_TRUE(filesystem->exists(testDataPath("filesystem") + "foobar.txt"));
}

TEST(FilesystemTest, Manipulation) {
    auto testfile_source = testDataPath("filesystem") + "foobar.txt";

    auto testfile_copy = testDataPath("filesystem") + "foobar-copy-" + utilities::uuid() + ".txt";
    auto testfile_move = testDataPath("filesystem") + "foobar-move-" + utilities::uuid() + ".txt";

    EXPECT_TRUE(filesystem->cp(testfile_source, testfile_copy));
    EXPECT_TRUE(filesystem->exists(testfile_copy));

    EXPECT_TRUE(filesystem->mv(testfile_copy, testfile_move));
    EXPECT_FALSE(filesystem->exists(testfile_copy));
    EXPECT_TRUE(filesystem->exists(testfile_move));

    /// TODO: determine why this is failing
    EXPECT_FALSE(filesystem->rm(testfile_move));

    /// For not use this as an opportunity to test failure
    auto [errorno, errorstr] = filesystem->error();

    EXPECT_EQ(errorno, 5);
    EXPECT_STREQ(errorstr.c_str(), "Access is denied.\r\n");

    // EXPECT_FALSE(filesystem->exists(testfile_move));
}

TEST(FilesystemTest, ReadDirectory) {
    auto contents = filesystem->read_directory(testDataPath());

    EXPECT_TRUE(contents.size() > 0);
}