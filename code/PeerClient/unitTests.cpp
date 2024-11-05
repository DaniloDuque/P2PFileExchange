#include <iostream>
#include <gtest/gtest.h>
#include "fileReader.cpp"



TEST(SizeTest, CountSizeOfOutput) {
    std::string s = "testDirectory";
    EXPECT_EQ(fileDirectoryReader(s).size(), 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
