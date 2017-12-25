#include "stdafx.h"
#include "leptjson.h"
#include <gtest/gtest.h>

//将会大量重复的代码写进函数以供调用
inline void UnitTest(int error, const char *json) {
  LeptValue v;
  v.type = LEPT_FALSE;
  EXPECT_EQ(error, LeptParse(&v, json));
}

inline void UnitTest(double number, const char *json) {
  LeptValue v;
  EXPECT_EQ(LEPT_PARSE_OK, LeptParse(&v, json));
  //EXPECT_EQ(LEPT_NUMBER, LeptGetType(&v));
  EXPECT_EQ(number, LeptGetNumber(&v));
}

TEST(test_parse_expect_value , input_whitespace) {
  UnitTest(LEPT_PARSE_EXPECT_VALUE, "");
  UnitTest(LEPT_PARSE_EXPECT_VALUE, " ");
}

TEST(test_parse_invalid_value, input_nil) {
  UnitTest(LEPT_PARSE_INVALID_VALUE, "nil");
}

TEST(test_parse_root_not_singular, input_null_ws_x) {
  UnitTest(LEPT_PARSE_ROOT_NOT_SINGULAR, " null x");
}

TEST(test_parse_null, input_null) {
  UnitTest(LEPT_PARSE_OK, " null ");
  //EXPECT_EQ(LEPT_NULL, LeptGetType(&v));
}

TEST(test_parse_true, input_true) {
  UnitTest(LEPT_PARSE_OK, " true ");
  //EXPECT_EQ(LEPT_TRUE, LeptGetType(&v));
}

TEST(test_parse_false, input_false) {
  UnitTest(LEPT_PARSE_OK, " false ");
  //EXPECT_EQ(LEPT_FALSE, LeptGetType(&v));
}

TEST(test_parse_number, input_numbers) {
  UnitTest(0.0, "0");
  UnitTest(0.0, "-0");
  UnitTest(0.0, "-0.0");
  UnitTest(1.0, "1");
  UnitTest(-1.0, "-1");
  UnitTest(1.5, "1.5");
  UnitTest(-1.5, "-1.5");
  UnitTest(3.1416, "3.1416");
  UnitTest(1E10, "1E10");
  UnitTest(1e10, "1e10");
  UnitTest(1E+10, "1E+10");
  UnitTest(1E-10, "1E-10");
  UnitTest(-1E10, "-1E10");
  UnitTest(-1e10, "-1e10");
  UnitTest(-1E+10, "-1E+10");
  UnitTest(-1E-10, "-1E-10");
  UnitTest(1.234E+10, "1.234E+10");
  UnitTest(1.234E-10, "1.234E-10");
  //UnitTest(0.0, "1e-10000");  //堆栈下溢 underflow
  UnitTest(LEPT_PARSE_INVALID_VALUE, "+0");
  UnitTest(LEPT_PARSE_INVALID_VALUE, "+1");
  UnitTest(LEPT_PARSE_INVALID_VALUE, ".123"); 
  UnitTest(LEPT_PARSE_INVALID_VALUE, "1."); 
  UnitTest(LEPT_PARSE_INVALID_VALUE, "INF");
  UnitTest(LEPT_PARSE_INVALID_VALUE, "inf");
  UnitTest(LEPT_PARSE_INVALID_VALUE, "NAN");
  UnitTest(LEPT_PARSE_INVALID_VALUE, "nan");
}