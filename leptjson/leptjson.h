#pragma once
#ifndef LEPTJSON_H_
#define LEPTJSON_H_

//JSON的数据类型
using LeptType = enum {
  LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER,
  LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT
};

//JSON的结构（树形）
struct LeptValue{
  LeptType type;
  double number;
};

//LeptParse的返回值
enum {
  LEPT_PARSE_OK = 0,
  LEPT_PARSE_EXPECT_VALUE,
  LEPT_PARSE_INVALID_VALUE,
  LEPT_PARSE_ROOT_NOT_SINGULAR,
  LEPT_PARSE_NUMBER_TOO_BIG
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

//当LeptValue的type是LEPT_NUMBER时，获取存储的double值
//接受树节点
//返回double类型的值
double LeptGetNumber(const LeptValue *v);  


#endif  //LEPTJSON_H_
