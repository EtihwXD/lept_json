#include "stdafx.h"
#include "leptjson.h"
#include <gtest/gtest.h>

TEST(test_parse_expect_value , input_whitespace) {
  LeptValue v;
  v.type = LEPT_TRUE;
  EXPECT_EQ(LEPT_PARSE_EXPECT_VALUE, LeptParse(&v, ""));
  EXPECT_EQ(LEPT_PARSE_EXPECT_VALUE, LeptParse(&v, " "));
  //ECT_EQ(LEPT_NULL, LeptGetType(&v));
}

TEST(test_parse_invalid_value, input_nil) {
  LeptValue v;
  v.type = LEPT_TRUE;
  EXPECT_EQ(LEPT_PARSE_INVALID_VALUE, LeptParse(&v, "nil"));
}

TEST(test_parse_root_not_singular, input_null_ws_x) {
  LeptValue v;
  v.type = LEPT_TRUE;
  EXPECT_EQ(LEPT_PARSE_ROOT_NOT_SINGULAR, LeptParse(&v, "null x"));
}

TEST(test_parse_null, input_null) {
  LeptValue v;
  v.type = LEPT_TRUE;
  EXPECT_EQ(LEPT_PARSE_OK, LeptParse(&v, "null"));
  //EXPECT_EQ(LEPT_NULL, LeptGetType(&v));
}

TEST(test_parse_true, input_true) {
  LeptValue v;
  v.type = LEPT_FALSE;
  EXPECT_EQ(LEPT_PARSE_OK, LeptParse(&v, "true"));
  //EXPECT_EQ(LEPT_TRUE, LeptGetType(&v));
}

TEST(test_parse_false, input_false) {
  LeptValue v;
  v.type = LEPT_TRUE;
  EXPECT_EQ(LEPT_PARSE_OK, LeptParse(&v, "false"));
  //EXPECT_EQ(LEPT_FALSE, LeptGetType(&v));
}