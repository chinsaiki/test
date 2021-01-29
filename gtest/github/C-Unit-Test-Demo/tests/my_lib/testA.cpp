#include <gtest/gtest.h>


// Google Test is a C++ framework -> use extern "C" to include C header files
extern "C" {
    #include "my_lib/A.h"
}


TEST(ATest, CanCreate) 
{
    A_Handle a = A_create(0);
    EXPECT_NE(nullptr, a);
}


TEST(ATest, MultiplyReturnsExpectedResult) 
{
    int initValue = 17;
    A_Handle a = A_create(initValue);

    int factor = 5;
    int expectedResult = factor * initValue;

    int result = A_multiplyWithInitial(a, factor);

    EXPECT_EQ(expectedResult, result);
}