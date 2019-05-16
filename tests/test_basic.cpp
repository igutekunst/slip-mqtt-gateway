#include "gtest/gtest.h"
extern "C" {
    #include <library.h>
}

TEST(TestBasic, Zero) {
    EXPECT_EQ(0, hello());
}
