#pragma once
#ifndef LEPTJSON_H_
#define LEPTJSON_H_
#include <string>
#include <memory>

//JSON����������
using LeptType = enum {
  LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER,
  LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT
};

//JSON�Ľṹ�����Σ�
struct LeptValue{
  LeptType type;
  union {
    double number;  //�洢number����
    struct {        //�洢string����
      char *string;
      size_t length;    //�洢�ַ���
    }str;
    struct {        //�洢array����
      LeptValue *e;     //��������������
      size_t size;      //�洢Ԫ�صĸ���
    }arr;
  };
};

//LeptParse�ķ���ֵ
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
//����json
//����c����ַ����ı��͸��ڵ�ָ��
//����ֵΪ���ĵ�ö��ֵ��
//  �޴���᷵��LEPT_PARSE_OK
//  ��JSONֻ�пհף�����LEPT_PARSE_EXPECT_VALUE
//  ��һ��ֵ֮���ڿհ�֮�����ַ�������LEPT_PARSE_ROOT_NOT_SINGULAR
//  ��ֵ����null��true��false,����LEPT_PARSE_INVALID_VALUE
int LeptParse(LeptValue *v, const char *json);

//���ʽ������ȡ������
LeptType LeptGetType(const LeptValue *v);

int LeptGetBoolean(const LeptValue* v);
void LeptSetBoolean(LeptValue* v, int b);

//��LeptValue��type��LEPT_NUMBERʱ����ȡ�洢��doubleֵ
//�������ڵ�
//����double���͵�ֵ
double LeptGetNumber(const LeptValue *v);  
void LeptSetNumber(LeptValue *v, double n);

const char* LeptGetString(const LeptValue* v);
size_t LeptGetStringLength(const LeptValue* v);
void LeptSetString(LeptValue* v, const char* s, size_t len);

#endif  //LEPTJSON_H_
