#include <iostream>
#include <gtest/gtest.h>



TEST(AddTest, HandlesPositiveInput) {
    EXPECT_EQ(1 + 2, 5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
