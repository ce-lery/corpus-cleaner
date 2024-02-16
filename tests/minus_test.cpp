#include "CppUTest/TestHarness.h"
#include "practice.hpp"

TEST_GROUP(minus)
{
    void setup(){};

    void teardown(){};
};

TEST(minus, InputPlusValue)
{
    int ret = minus_sm(10, 2);
    CHECK_EQUAL(8, ret);
}