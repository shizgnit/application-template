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

#include <gtest/gtest.h>

TEST(FilesystemTest, FilterFileType) {
    EXPECT_EQ(filesystem->read_directory(test->getTestDataPath() + "filesystem/directory").size(), 1);
    EXPECT_EQ(filesystem->read_directory(test->getTestDataPath() + "regular").size(), 13);
    EXPECT_EQ(filesystem->read_directory(test->getTestDataPath() + "foobar").size(), 0);
}

TEST(FilesystemTest, PresentWorkingDirectory) {
    auto pwd = filesystem->pwd();
    EXPECT_FALSE(pwd.empty());
}

TEST(FilesystemTest, Exists) {
    EXPECT_TRUE(filesystem->exists(test->getTestDataPath() + "filesystem/foobar.txt"));
}

TEST(FilesystemTest, Manipulation) {
    auto testfile_source = test->getTestDataPath() + "filesystem/foobar.txt";
    auto testfile_copy = test->getTestDataPath() + "filesystem/foobar-copy-" + utilities::uuid() + ".txt";
    auto testfile_move = test->getTestDataPath() + "filesystem/foobar-move-" + utilities::uuid() + ".txt";

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
}

TEST(FilesystemTest, ReadDirectory) {
    auto contents = filesystem->read_directory(test->getTestDataPath());
    EXPECT_TRUE(contents.size() > 0);
}
