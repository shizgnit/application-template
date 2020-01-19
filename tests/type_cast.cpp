#include "gtest/gtest.h"

#include "tests.hpp"

class TypeCastTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    // void TearDown() override {}
};

TEST(TypeCastTest, NumericToString) {
    EXPECT_STREQ(utilities::type_cast<std::string>('a').c_str(), "a");
    EXPECT_STREQ(utilities::type_cast<std::string>(12345).c_str(), "12345");
    EXPECT_STREQ(utilities::type_cast<std::string>(2147483648).c_str(), "2147483648");
    EXPECT_STREQ(utilities::type_cast<std::string>(4.0009).c_str(), "4.000900");
}

TEST(TypeCastTest, StringToNumeric) {
    EXPECT_EQ((double)utilities::type_cast<double>("4.000900"), 4.0009);
    EXPECT_EQ((unsigned long)utilities::type_cast<unsigned long>("2147483652"), 2147483647);
}
