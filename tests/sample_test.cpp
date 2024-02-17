#include <gtest/gtest.h>
#include "sample.hpp"
// bool IsEven(int x) {
//     return x % 2 == 1;
// }
TEST(IsEvenTest, Negative) {
    EXPECT_FALSE(IsEven(-1));
    EXPECT_TRUE(IsEven(-2));
}

TEST(IsEvenTest, Zero) {
    EXPECT_TRUE(IsEven(0));
}

TEST(IsEvenTest, Positive) {
    EXPECT_FALSE(IsEven(1));
    EXPECT_TRUE(IsEven(2));
}

// int main(int argc, char **argv)
// {
// 	//Google Testの初期化
// 	::testing::InitGoogleTest(&argc, argv);
// 	//全テスト実行
// 	return RUN_ALL_TESTS();
// }