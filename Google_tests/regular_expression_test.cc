#include "gtest/gtest.h"

#include "tokenizer/regular_expression.h"

class RegularExpressionTest : public ::testing::Test {
 protected:
  tokenizer::RegularExpression regex1;
  tokenizer::RegularExpression regex2;
  tokenizer::RegularExpression regex3;
  tokenizer::RegularExpression regex4;
  tokenizer::RegularExpression regex5;
  tokenizer::RegularExpression regex6;
  tokenizer::RegularExpression regex7;
  tokenizer::RegularExpression regex8;

  void SetUp() override {
    regex1 = tokenizer::RegularExpression("a");
    regex2 = tokenizer::RegularExpression("a|b");
    regex3 = tokenizer::RegularExpression("ab");
    regex4 = tokenizer::RegularExpression("a*");
    regex5 = tokenizer::RegularExpression("(a|b|c)def");
    regex6 = tokenizer::RegularExpression("(a|b)(m|n)");
    regex7 = tokenizer::RegularExpression("(a|b)(c|d)*");
  }
};

TEST_F(RegularExpressionTest, TestSingleLetter) {
  EXPECT_EQ(regex1.match("a"), "a");
  EXPECT_EQ(regex1.match("b"), "");
  EXPECT_EQ(regex1.match("abc"), "a");
}

TEST_F(RegularExpressionTest, TestSimpleUnion) {
  EXPECT_EQ(regex2.match("ab"), "a");
  EXPECT_EQ(regex2.match("ba"), "b");
  EXPECT_EQ(regex2.match("cab"), "");
}

TEST_F(RegularExpressionTest, TestSimpleConcatenation) {
  EXPECT_EQ(regex3.match("ab"), "ab");
  EXPECT_EQ(regex3.match("a"), "");
}

TEST_F(RegularExpressionTest, TestSimpleStar) {
  EXPECT_EQ(regex4.match("aaaa"), "aaaa");
  EXPECT_EQ(regex4.match("aabbdeb"), "aa");
  EXPECT_EQ(regex4.match("cdef"), "");
}

TEST_F(RegularExpressionTest, TestUnionAndConcatenation1) {
  EXPECT_EQ(regex5.match("adef"), "adef");
  EXPECT_EQ(regex5.match("bdefagsadf"), "bdef");
  EXPECT_EQ(regex5.match("bdfdsfadf"), "");
}

TEST_F(RegularExpressionTest, TestUnionAndConcatenation2) {
  EXPECT_EQ(regex6.match("am"), "am");
  EXPECT_EQ(regex6.match("anafdsaf"), "an");
  EXPECT_EQ(regex6.match("bmasdfasf"), "bm");
  EXPECT_EQ(regex6.match("mdsaf"), "");
}

TEST_F(RegularExpressionTest, TestUnionConcantenationAndStar) {
  EXPECT_EQ(regex7.match("acdcd"), "acdcd");
  EXPECT_EQ(regex7.match("bcddcqer"), "bcddc");
  EXPECT_EQ(regex7.match("amfs"), "a");
  EXPECT_EQ(regex7.match("dfsadf"), "");
}