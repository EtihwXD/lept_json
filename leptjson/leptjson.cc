// leptjson.cc: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <assert.h>
#include <string>
#include "leptjson.h"

//为了减少解析函数之间传递多个函数，把数据都放进一个struct
struct LeptContext {
  const char *json;
};

double LeptGetNumber(const LeptValue *v) {
  assert(v != nullptr&&v->type == LEPT_NUMBER);
  return v->number;
}

//跳过所有空白，空白包括空格，\n,\t,\r
//接受指向LeptContext的指针，因为要改变它（跳过空白其实是重新指向空白后面的字符串）
//这个函数理论上不会产生错误，所以不需要返回错误码
static void LeptParseWhitespace(LeptContext *c) {
  const char *p = c->json;
  while (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
    ++p;
  c->json = p;
}

int LeptParseLiteral(LeptType type, const char *literal,
                     LeptContext *c, LeptValue *v) {
  size_t i = 0;
  for (i = 0; literal[i] != '\0'; ++i)
    if (c->json[i] != literal[i])
      return LEPT_PARSE_INVALID_VALUE;
  c->json += i;
  v->type = type;
  return LEPT_PARSE_OK;
}

//处理number
int LeptParseNumber(LeptContext *c, LeptValue *v) {
  char *end;
  //依据json的格式进行合法性校验
  const char *p = c->json;
  if (*p == '-') ++p;
  if (*p == '0') {
    ++p;
    if (*p <= '9' && *p >= '0')
      return LEPT_PARSE_INVALID_VALUE;
  }
  else {
    if (*p > '9' || *p < '1')
      return LEPT_PARSE_INVALID_VALUE;
    for (/*empty*/; (*p <= '9' && *p >= '0'); ++p) /*empty*/;
  }
  if (*p == '.') {
    ++p;
    if (*p > '9' || *p < '0')
      return LEPT_PARSE_INVALID_VALUE;
    for (/*empty*/; (*p <= '9' && *p >= '0'); ++p) /*empty*/;
  }
  if (*p == 'e' || *p == 'E') {
    ++p;
    if (*p == '+' || *p == '-') ++p;
    if (*p > '9' || *p < '0')
      return LEPT_PARSE_INVALID_VALUE;
    for (/*empty*/; (*p <= '9' && *p >= '0'); ++p) /*empty*/;
  }
  //开始处理数据
  errno = 0;//错误号
  v->number = strtod(c->json,&end);//若无法转换则返回0并将第二个参数设为第一个参数
  //排除以'+'开头的数据、nan、inf
  if (errno == ERANGE) {//结果过大
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_NUMBER_TOO_BIG;
  }
  if (c->json == end && v->number == HUGE_VAL)
    return LEPT_PARSE_INVALID_VALUE;
  v->type = LEPT_NUMBER;
  c->json = end;
  return LEPT_PARSE_OK;
}

//处理value
int LeptParseValue(LeptContext *c, LeptValue *v) {
  switch (*c->json) {
    case 'n': return LeptParseLiteral(LEPT_NULL, "null", c, v);
    case 'f': return LeptParseLiteral(LEPT_FALSE, "false", c, v);
    case 't': return LeptParseLiteral(LEPT_TRUE, "true", c, v);
    case '\0': return LEPT_PARSE_EXPECT_VALUE;
    default: return LeptParseNumber(c, v);
  }
}

//处理JSON-test = ws value ws 的情况
int LeptParse(LeptValue *v, const char *json) {
  LeptContext c;
  assert(v != nullptr);
  c.json = json;
  v->type = LEPT_NULL;
  LeptParseWhitespace(&c);
  int ret = 0;
  if ((ret = LeptParseValue(&c, v)) == LEPT_PARSE_OK) { //处理最后的ws
    LeptParseWhitespace(&c);
    if (*c.json != '\0') {
      v->type = LEPT_NULL;
      return LEPT_PARSE_ROOT_NOT_SINGULAR;
    }
  }
  return ret;
}

