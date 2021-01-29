/**
 * @file         gtest_main.cpp
 * @brief        main file for all gtest projects.
 */
#include <gtest/gtest.h>
#include <iostream>

GTEST_API_ int main(int argc, char **argv)
{
    std::cout << "Starting Unit Tests from gtest_main.cpp" << std::endl;
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}
