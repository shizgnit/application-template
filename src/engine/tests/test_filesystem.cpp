#include "engine.hpp"

#if defined __PLATFORM_SUPPORTS_GOOGLETEST

#include <gtest/gtest.h>

TEST(FilesystemTest, PresentWorkingDirectory) {
    auto pwd = filesystem->pwd();
    EXPECT_FALSE(pwd.empty());
}

TEST(FilesystemTest, Exists) {
    EXPECT_TRUE(filesystem->exists(test->getTestDataPath() + "filesystem/foobar.txt"));
}

TEST(FilesystemTest, Manipulation) {
    auto testfile_source = test->getTestDataPath() + "filesystem/foobar.txt";
    auto testfile_copy = test->getTestDataPath() + "filesystem/.foobar-copy-" + utilities::uuid() + ".txt";
    auto testfile_move = test->getTestDataPath() + "filesystem/.foobar-move-" + utilities::uuid() + ".txt";

    EXPECT_TRUE(filesystem->cp(testfile_source, testfile_copy));
    EXPECT_TRUE(filesystem->exists(testfile_copy));

    EXPECT_TRUE(filesystem->mv(testfile_copy, testfile_move));
    EXPECT_FALSE(filesystem->exists(testfile_copy));
    EXPECT_TRUE(filesystem->exists(testfile_move));

    EXPECT_TRUE(filesystem->rm(testfile_move));
}

TEST(FilesystemTest, ReadDirectory) {
    auto contents = filesystem->read_directory(test->getTestDataPath());
    EXPECT_TRUE(contents.size() > 0);

    EXPECT_EQ(filesystem->read_directory(test->getTestDataPath() + "filesystem").size(), 1);
    EXPECT_EQ(filesystem->read_directory(test->getTestDataPath() + "filesystem/").size(), 3);
}

#endif
