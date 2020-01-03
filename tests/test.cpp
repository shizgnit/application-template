#include "gtest/gtest.h"

class SampleTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    // void TearDown() override {}
};

TEST(SampleTest, TestName) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}


// data does not persist in memory... stream to gpu/etc.