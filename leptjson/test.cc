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

inline void UnitTest(const char *json, const char *data) {
  LeptValue v;
  v.type = LEPT_NULL;
  EXPECT_EQ(LEPT_PARSE_OK, LeptParse(&v, json));
  ASSERT_STREQ(data,LeptGetString(&v));
  //EXPECT_EQ(LEPT_STRING, LeptGetType(&v));
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

  UnitTest(LEPT_PARSE_NUMBER_TOO_BIG, "1e-10000");  //堆栈下溢 underflow
  LeptValue v;
  LeptParse(&v, "1e-10000");
  EXPECT_EQ(0.0, LeptGetNumber(&v));

  UnitTest(LEPT_PARSE_INVALID_VALUE, "+0");
  UnitTest(LEPT_PARSE_INVALID_VALUE, "02");
  UnitTest(LEPT_PARSE_INVALID_VALUE, "2.df");
  UnitTest(LEPT_PARSE_INVALID_VALUE, "+1");
  UnitTest(LEPT_PARSE_INVALID_VALUE, ".123"); 
  UnitTest(LEPT_PARSE_INVALID_VALUE, "1."); 
  UnitTest(LEPT_PARSE_INVALID_VALUE, "INF");
  UnitTest(LEPT_PARSE_INVALID_VALUE, "inf");
  UnitTest(LEPT_PARSE_INVALID_VALUE, "NAN");
  UnitTest(LEPT_PARSE_INVALID_VALUE, "nan");
  //边界值测试
  //最小的数
  UnitTest(1.0000000000000002, "1.0000000000000002");
  // minimum denormal
  UnitTest(4.9406564584124654e-324, "4.9406564584124654e-324");
  UnitTest(-4.9406564584124654e-324, "-4.9406564584124654e-324");
  //Max subnormal double
  UnitTest(2.2250738585072009e-308, "2.2250738585072009e-308"); 
  UnitTest(-2.2250738585072009e-308, "-2.2250738585072009e-308");
  //Min normal positive double
  UnitTest(2.2250738585072014e-308, "2.2250738585072014e-308"); 
  UnitTest(-2.2250738585072014e-308, "-2.2250738585072014e-308");
  //Max double
  UnitTest(1.7976931348623157e+308, "1.7976931348623157e+308");
  UnitTest(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

TEST(test_parse_get_boolean, input_string) {
  LeptValue v;
  LeptParse(&v, "null");
  EXPECT_EQ(LEPT_NULL, LeptGetBoolean(&v));
  LeptSetBoolean(&v, LEPT_TRUE);
  EXPECT_EQ(1, LeptGetBoolean(&v));
  LeptSetBoolean(&v, LEPT_FALSE);
  EXPECT_EQ(0, LeptGetBoolean(&v));
}

TEST(test_parse_string, input_string) {
  UnitTest("\"sdfsdf\"","sdfsdf");
  UnitTest("\"\\\"\"", "\"");
  UnitTest("\" \"", " ");
  UnitTest("\"\\\\\"", "\\");
  UnitTest("\"\/\"", "/");
  UnitTest("\"\\b\"", "\b");
  UnitTest("\"\\f\"", "\f");
  UnitTest("\"\\n\"", "\n");
  UnitTest("\"\\r\"", "\r");
  UnitTest("\"\\t\"", "\t");
  UnitTest("\"\u00d0\"", "?");
}

TEST(test_parse_invalid_string_escape, input_invalid_string_escap) {
  LeptValue v;
  EXPECT_EQ(LEPT_PARSE_INVALID_STRING_ESCAPE, LeptParse(&v, "\"\\v\""));
  EXPECT_EQ(LEPT_PARSE_INVALID_STRING_ESCAPE, LeptParse(&v, "\"\\'\""));
  EXPECT_EQ(LEPT_PARSE_INVALID_STRING_ESCAPE, LeptParse(&v, "\"\\0\""));
  EXPECT_EQ(LEPT_PARSE_INVALID_STRING_ESCAPE, LeptParse(&v, "\"\\x12\""));
}

TEST(test_parse_invalid_string_char, input_invalid_string_char) {
  LeptValue v;
  EXPECT_EQ(LEPT_PARSE_INVALID_STRING_CHAR, LeptParse(&v, "\"\x01\""));
  EXPECT_EQ(LEPT_PARSE_INVALID_STRING_CHAR, LeptParse(&v, "\"\x1F\""));

}