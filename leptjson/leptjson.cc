// leptjson.cc: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <assert.h>
#include <string>
#include "leptjson.h"

#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif // !LEPT_PARSE_STACK_INIT_SIZE

//为了减少解析函数之间传递多个函数，把数据都放进一个struct
struct LeptContext {
  const char *json;
  char* stack;
  size_t size, top;
};

//堆栈的压入前的操作，用于开辟和检查空间
//接受压入字节的大小，返回数据起始的指针
void* LeptContextPush(LeptContext *c, size_t size) {
  void *ret;
  assert(size > 0);
  if (c->top + size >= c->size) {//空间不够则每次扩展至原来的1.5倍
    if (c->size == 0)
      c->size = LEPT_PARSE_STACK_INIT_SIZE;
    while (c->top + size >= c->size)
      c->size += c->size / 2;
    c->stack = (char*)realloc(c->stack, c->size);
  }
  ret = c->stack + c->top;
  c->top += size;
  return ret;
}
//堆栈的弹出
void* LeptContextPop(LeptContext *c, size_t size) {
  assert(c->top >= size);
  return c->stack + (c->top -= size);
}
//解析string时调用的 真正的压栈操作
inline void PutCh(LeptContext *c, char ch) {
  *static_cast<char*>(LeptContextPush(c, sizeof(ch))) = ch;
}

void LeptFree(LeptValue *v) {
  assert(v != nullptr);
  if (v->type == LEPT_STRING)
    delete(v->str.string);
  v->type = LEPT_NULL;
}

//Boolean的get
int LeptGetBoolean(const LeptValue* v) {
  assert(v != nullptr);
  return v->type == LEPT_TRUE;
}
void LeptSetBoolean(LeptValue* v, int b) {
  assert(v != nullptr);
  switch (b) {
    case 1: v->type = LEPT_FALSE; break;
    case 2: v->type = LEPT_TRUE; break;
    default: v->type = LEPT_NULL; break;
  }
}

//number的get
void LeptSetNumber(LeptValue *v, double n) {
  assert(v != nullptr&&v->type == LEPT_NUMBER);
  v->number = n;
}
double LeptGetNumber(const LeptValue *v) {
  assert(v != nullptr&&v->type == LEPT_NUMBER);
  return v->number;
}

//string的get
const char* LeptGetString(const LeptValue* v) {
  assert(v != NULL);
  return v->str.string;
}
size_t LeptGetStringLength(const LeptValue* v) {
  assert(v != NULL);
  return v->str.length;
}
void LeptSetString(LeptValue *v, const char *s, size_t len) {
  assert(v != NULL && (s != NULL || len == 0));
  LeptFree(v);
  v->str.string = new char[len + 1];
  memcpy(v->str.string, s, len);
  v->str.string[len] = '\0';
  v->str.length = len;
  v->type = LEPT_STRING;
}

//array的get
size_t LeptGetArraySize(const LeptValue *v) {
  assert(v != nullptr&&v->type == LEPT_ARRAY);
  return v->arr.size;
}
LeptValue* LeptGetArrayElement(const LeptValue *v, size_t index) {
  assert(v != nullptr&&v->type == LEPT_ARRAY && index < v->arr.size);
  return &v->arr.e[index];
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

//处理string
int LeptParseString(LeptContext *c, LeptValue *v) {
  assert(*c->json == '\"');
  ++c->json;
  size_t head = c->top, len;
  const char *p = c->json;
  while (1) {
    char ch = *p++;
    switch (ch) {
      case '\"':
        len = c->top - head;
        LeptSetString(v, static_cast<const char*>(LeptContextPop(c, len)),len);
        c->json = p;
        return LEPT_PARSE_OK;
      case '\0':
        c->top = head;
        return LEPT_PARSE_MISS_QUOTATION_MARK;
      case '\\':
        switch (*p++) {
          case '\"': PutCh(c, '\"'); break;
          case '\\': PutCh(c, '\\'); break;
          case '/':  PutCh(c, '/');  break;
          case 'b':  PutCh(c, '\b'); break;
          case 'f':  PutCh(c, '\f'); break;
          case 'n':  PutCh(c, '\n'); break;
          case 'r':  PutCh(c, '\r'); break;
          case 't':  PutCh(c, '\t'); break;
          default:        
            c->top = head;
            return LEPT_PARSE_INVALID_STRING_ESCAPE;
        }
        break;
      default:
        if ((unsigned char)ch < 0x20) {
          c->top = head;
          return LEPT_PARSE_INVALID_STRING_CHAR;
        }
        PutCh(c, ch);
    }
  }
}
int LeptParseValue(LeptContext *c, LeptValue *v);//前置声明，下面的函数要使用
int LeptParseArray(LeptContext *c, LeptValue *v) {
  assert(*c->json == '[');
  ++c->json;
  LeptParseWhitespace(c);
  size_t size = 0;
  int ret;
  if (*c->json == ']') {
    ++c->json;
    v->type = LEPT_ARRAY;
    v->arr.e = nullptr;
    v->arr.size = 0;
    return LEPT_PARSE_OK;
  }
  while (1) {
    LeptValue e; 
    e.type = LEPT_NULL;
    LeptParseWhitespace(c);
    if ((ret = LeptParseValue(c, &e)) != LEPT_PARSE_OK) {
      break;
    }
    memcpy(LeptContextPush(c, sizeof(LeptValue)), &e, sizeof(LeptValue));
    ++size;
    LeptParseWhitespace(c);
    if (*c->json == ']') {
      ++c->json;
      v->type = LEPT_ARRAY;
      v->arr.size = size;
      size *= sizeof(LeptValue);
      memcpy(v->arr.e = (LeptValue*)malloc(size),
        LeptContextPop(c, size), size);
      return LEPT_PARSE_OK;
    } else if (*c->json == ',') {
      ++c->json;
    } else {
      ret = LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
      break;
    }
  }
  v->type = LEPT_NULL;
  v->arr.size = 0;
  size *= sizeof(LeptValue);
  LeptFree(static_cast<LeptValue*>(LeptContextPop(c, size)));
  return ret;
}

//处理value
int LeptParseValue(LeptContext *c, LeptValue *v) {
  switch (*c->json) {
    case '[':  return LeptParseArray(c, v);
    case '\"': return LeptParseString(c, v);
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
  c.stack = nullptr;
  c.size = c.top = 0;
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
  assert(c.top == 0);
  free(c.stack);
  return ret;
}
