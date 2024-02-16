#include "CppUTest/TestHarness.h"
#include "practice.hpp"

TEST_GROUP(plus)
{
    void setup(){};

    void teardown(){};
};

TEST(plus, InputPlusValue)
{
    int ret = plus_sm(5, 6);
    CHECK_EQUAL(11, ret);
}
