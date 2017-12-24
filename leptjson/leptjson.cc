// leptjson.cc: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <assert.h>
#include "leptjson.h"

//为了减少解析函数之间传递多个函数，把数据都放进一个struct
struct LeptContext {
  const char *json;
};


//跳过所有空白，空白包括空格，\n,\t,\r
//接受指向LeptContext的指针，因为要改变它（跳过空白其实是重新指向空白后面的字符串）
//这个函数理论上不会产生错误，所以不需要返回错误码
static void LeptParseWhitespace(LeptContext *c) {
  const char *p = c->json;
  while (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')
    ++p;
  c->json = p;
}

//处理null
static int LeptParseNull(LeptContext *c, LeptValue *v) {
  assert(*c->json == 'n');
  ++c->json;
  if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
    return LEPT_PARSE_INVALID_VALUE;
  c->json += 3;
  v->type = LEPT_NULL;
  return LEPT_PARSE_OK;
}

//处理true
static int LeptParseTrue(LeptContext *c, LeptValue *v) {
  assert(*c->json == 't');
  ++c->json;
  if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
    return LEPT_PARSE_INVALID_VALUE;
  c->json += 3;
  v->type = LEPT_TRUE;
  return LEPT_PARSE_OK;
}

//处理false
static int LeptParseFalse(LeptContext *c, LeptValue *v) {
  assert(*c->json == 'f');
  ++c->json;
  if (c->json[0] != 'a' || c->json[1] != 'l' ||
      c->json[2] != 's' || c->json[3] != 'e')
    return LEPT_PARSE_INVALID_VALUE;
  c->json += 4;
  v->type = LEPT_FALSE;
  return LEPT_PARSE_OK;
}

//处理value
static int LeptParseValue(LeptContext *c, LeptValue *v) {
  switch (*c->json) {
    case 'n': return LeptParseNull(c, v);
    case 'f': return LeptParseFalse(c, v);
    case 't': return LeptParseTrue(c, v);
    case '\0': return LEPT_PARSE_EXPECT_VALUE;
    default: return LEPT_PARSE_INVALID_VALUE;
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
    if (*c.json != '\0')
      return LEPT_PARSE_ROOT_NOT_SINGULAR;
  }
  return ret;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            