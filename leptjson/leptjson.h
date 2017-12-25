#pragma once
#ifndef LEPTJSON_H_
#define LEPTJSON_H_

//JSON����������
using LeptType = enum {
  LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER,
  LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT
};

//JSON�Ľṹ�����Σ�
struct LeptValue{
  LeptType type;
  double number;
};

//LeptParse�ķ���ֵ
enum {
  LEPT_PARSE_OK = 0,
  LEPT_PARSE_EXPECT_VALUE,
  LEPT_PARSE_INVALID_VALUE,
  LEPT_PARSE_ROOT_NOT_SINGULAR,
  LEPT_PARSE_NUMBER_TOO_BIG
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

//��LeptValue��type��LEPT_NUMBERʱ����ȡ�洢��doubleֵ
//�������ڵ�
//����double���͵�ֵ
double LeptGetNumber(const LeptValue *v);  


#endif  //LEPTJSON_H_
