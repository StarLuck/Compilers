#ifndef _BASE_H
#define _BASE_H

#include <stdio.h>
#include <iostream>
#include <string.h>

const static char keywords[][7] = { "int", "void", "if", "else", "while", "return" };

#define LETTER 1
#define NUMBER 2
#define PUNCTUATION 3


int charkind(char c);//�ж��ַ�����
int is_letter(char c);//�Ƿ�����ĸ
int is_number(char c);//�Ƿ�������
int is_keyword(char str[]);//�Ƿ��ǹؼ���
int is_signword(char str[]);//�Ƿ��Ǳ�ʶ��
int is_numberstr(char str[]);//�Ƿ�����


#endif