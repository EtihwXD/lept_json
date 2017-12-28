#pragma once
#ifndef LEPTJSON_H_
#define LEPTJSON_H_
#include <string>
#include <memory>

//JSON的数据类型
using LeptType = enum {
  LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER,
  LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT
};

//JSON的结构（树形）
struct LeptValue{
  LeptType type;
  union {
    double number;  //存储number类型
    struct {        //存储string类型
      char *string;
      size_t length;    //存储字符数
    }str;
    struct {        //存储array类型
      LeptValue *e;     //并非链表，是数组
      size_t size;      //存储元素的个数
    }arr;
  };
};

//LeptParse的返回值
enum {
  LEPT_PARSE_OK = 0,
  LEPT_PARSE_EXPECT_VALUE,
  LEPT_PARSE_INVALID_VALUE,
  LEPT_PARSE_ROOT_NOT_SINGULAR,
  LEPT_PARSE_NUMBER_TOO_BIG,
  LEPT_PARSE_MISS_QUOTATION_MARK,
  LEPT_PARSE_INVALID_STRING_ESCAPE,
  LEPT_PARSE_INVALID_STRING_CHAR,
  LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET
};

//API
//解析json
//接收c风格字符串文本和根节点指针
//返回值为上文的枚举值：
//  无错误会返回LEPT_PARSE_OK
//  若JSON只有空白，返回LEPT_PARSE_EXPECT_VALUE
//  若一个值之后，在空白之后还有字符，返回LEPT_PARSE_ROOT_NOT_SINGULAR
//  若值不是null、true、false,返回LEPT_PARSE_INVALID_VALUE
int LeptParse(LeptValue *v, const char *json);

//访问结果，获取其类型
LeptType LeptGetType(const LeptValue *v);

int LeptGetBoolean(const LeptValue* v);
void LeptSetBoolean(LeptValue* v, int b);

//当LeptValue的type是LEPT_NUMBER时，获取存储的double值
//接受树节点
//返回double类型的值
double LeptGetNumber(const LeptValue *v);  
void LeptSetNumber(LeptValue *v, double n);

const char* LeptGetString(const LeptValue* v);
size_t LeptGetStringLength(const LeptValue* v);
void LeptSetString(LeptValue* v, const char* s, size_t len);

#endif  //LEPTJSON_H_
