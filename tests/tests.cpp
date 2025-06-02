#include <gtest/gtest.h>

#include "../src/smartArray/SmartArray.h"

// Test working google tests
TEST(HelloTest, BasicAssertions) {
    SmartArray<int> test;

    EXPECT_EQ(test.testGTest(), 0);
}