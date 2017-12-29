#include "stdafx.h"
#define CHECK_MEMORY_LEAKS
#include "leptjson.h"
#include <gtest/gtest.h>

#ifdef CHECK_MEMORY_LEAKS
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define CHECK_MEMORY_LEAKS_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new CHECK_MEMORY_LEAKS_NEW
#endif


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
  LeptFree(&v);
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
  LeptFree(&v);
}

TEST(test_parse_invalid_string_char, input_invalid_string_char) {
  LeptValue v;
  EXPECT_EQ(LEPT_PARSE_INVALID_STRING_CHAR, LeptParse(&v, "\"\x01\""));
  EXPECT_EQ(LEPT_PARSE_INVALID_STRING_CHAR, LeptParse(&v, "\"\x1F\""));
  LeptFree(&v);
}

TEST(test_parse_array, input_array) {
  LeptValue v;
  v.type = LEPT_NULL;
  EXPECT_EQ(LEPT_PARSE_OK, LeptParse(&v, "[null, false, true, 123, \"abc\"]"));
  EXPECT_EQ(5, LeptGetArraySize(&v));
  EXPECT_EQ(LEPT_NULL, LeptGetArrayElement(&v,0)->type);
  EXPECT_EQ(LEPT_FALSE, LeptGetArrayElement(&v, 1)->type);
  EXPECT_EQ(LEPT_TRUE, LeptGetArrayElement(&v, 2)->type);
  EXPECT_EQ(LEPT_NUMBER, LeptGetArrayElement(&v, 3)->type);
  EXPECT_EQ(LEPT_STRING, LeptGetArrayElement(&v, 4)->type);
  EXPECT_EQ(123, LeptGetArrayElement(&v, 3)->number);
  EXPECT_STREQ("abc", LeptGetArrayElement(&v, 4)->str.string);
  LeptFree(&v);

  EXPECT_EQ(LEPT_PARSE_OK, 
    LeptParse(&v, "[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
  EXPECT_EQ(4, LeptGetArraySize(&v));
  EXPECT_EQ(LEPT_ARRAY, LeptGetArrayElement(&v, 0)->type);
  EXPECT_EQ(LEPT_ARRAY, LeptGetArrayElement(&v, 1)->type);
  EXPECT_EQ(LEPT_ARRAY, LeptGetArrayElement(&v, 2)->type);
  EXPECT_EQ(LEPT_ARRAY, LeptGetArrayElement(&v, 3)->type);
  EXPECT_EQ(0, LeptGetArrayElement(&v, 0)->arr.size);
  EXPECT_EQ(1, LeptGetArrayElement(&v, 1)->arr.size);
  EXPECT_EQ(0, LeptGetArrayElement(&v, 1)->arr.e[0].number);
  EXPECT_EQ(0, LeptGetArrayElement(&v, 3)->arr.e[0].number);
  EXPECT_EQ(1, LeptGetArrayElement(&v, 3)->arr.e[1].number);
  EXPECT_EQ(2, LeptGetArrayElement(&v, 3)->arr.e[2].number);
  LeptFree(&v);

  EXPECT_EQ(LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, LeptParse(&v, "[1"));
  LeptFree(&v);
  EXPECT_EQ(LEPT_PARSE_INVALID_VALUE, LeptParse(&v, "[1,]"));
  LeptFree(&v);
  EXPECT_EQ(LEPT_PARSE_INVALID_VALUE, LeptParse(&v, "[\"a\", nul]"));
  LeptFree(&v);
}
//
//TEST(test_parse_object, input_object) {
//  LeptValue v;
//  size_t i;
//  v.type = LEPT_NULL;
//
//  EXPECT_EQ(LEPT_PARSE_OK, LeptParse(&v, " { } "));
//  EXPECT_EQ(0, LeptGetObjectSize(&v));
//  LeptFree(&v);
//
//  EXPECT_EQ(LEPT_PARSE_OK, LeptParse(&v,
//    " { "
//    "\"n\" : null , "
//    "\"f\" : false , "
//    "\"t\" : true , "
//    "\"i\" : 123 , "
//    "\"s\" : \"abc\", "
//    "\"a\" : [ 1, 2, 3 ],"
//    "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
//    " } "
//  ));
//  EXPECT_EQ(7, LeptGetObjectSize(&v));
//  EXPECT_STREQ("n", LeptGetObjectKey(&v, 0));
//  EXPECT_STREQ("f", LeptGetObjectKey(&v, 1));
//  EXPECT_STREQ("t", LeptGetObjectKey(&v, 2));
//  EXPECT_STREQ("i", LeptGetObjectKey(&v, 3));
//  EXPECT_EQ(123.0, LeptGetNumber(&LeptGetObjectValue(&v, 3)));
//  EXPECT_STREQ("s", LeptGetObjectKey(&v, 4));
//  EXPECT_STREQ("abc", LeptGetString(&LeptGetObjectValue(&v, 4)));
//  EXPECT_STREQ("a", LeptGetObjectKey(&v, 5));
//  EXPECT_EQ(3, LeptGetArraySize(&LeptGetObjectValue(&v, 5)));
//  for (i = 0; i < 3; i++) {
//    LeptValue* e = LeptGetArrayElement(&LeptGetObjectValue(&v, 5), i);
//    EXPECT_EQ(i + 1.0, LeptGetNumber(e));
//  }
//  EXPECT_STREQ("o", LeptGetObjectKey(&v, 6));
//  {
//    LeptValue* o = &LeptGetObjectValue(&v, 6);
//    for (i = 0; i < 3; i++) {
//      LeptValue* ov = &LeptGetObjectValue(o, i);
//      EXPECT_TRUE('1' + i == LeptGetObjectKey(o, i)[0]);
//      EXPECT_EQ(1, LeptGetObjectKeyLength(o, i));
//      EXPECT_EQ(i + 1.0, LeptGetNumber(ov));
//    }
//  }
//  LeptFree(&v);
//}
//
//TEST(test_parse_miss_key, input_object_miss_key) {
//  UnitTest(LEPT_PARSE_MISS_KEY, "{:1,");
//  UnitTest(LEPT_PARSE_MISS_KEY, "{1:1,");
//  UnitTest(LEPT_PARSE_MISS_KEY, "{true:1,");
//  UnitTest(LEPT_PARSE_MISS_KEY, "{false:1,");
//  UnitTest(LEPT_PARSE_MISS_KEY, "{null:1,");
//  UnitTest(LEPT_PARSE_MISS_KEY, "{[]:1,");
//  UnitTest(LEPT_PARSE_MISS_KEY, "{{}:1,");
//  UnitTest(LEPT_PARSE_MISS_KEY, "{\"a\":1,");
//}
//
//TEST(test_parse_miss_colon, input_boject_miss_colon) {
//    UnitTest(LEPT_PARSE_MISS_COLON, "{\"a\"}");
//    UnitTest(LEPT_PARSE_MISS_COLON, "{\"a\",\"b\"}");
//}
//
//TEST(test_parse_miss_comma_or_curly_bracket, miss_comma_or_curly_bracket) {
//  UnitTest(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1");
//  UnitTest(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1]");
//  UnitTest(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1 \"b\"");
//  UnitTest(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":{}");
//  UnitTest(LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":{} \"a\":{}");
//}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

#ifdef CHECK_MEMORY_LEAKS
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
  //_CrtSetBreakAlloc(657);
#endif 
  return RUN_ALL_TESTS();
}