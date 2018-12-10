#ifndef _LEXANALYSIS_H
#define _LEXANALYSIS_H

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <list>

using namespace std;

enum token_type {
	keyword, ID, num, assignment, plus, multiple, relop, semicolon,
	comma, lbracket, rbracket, lbrace, rbrace
};

typedef struct Token {
	char word[100];		//��
	token_type type;	//����
	int linenum;		//����
}Token;

class lexAnalysis {
public:
	list<Token> tokenList;	//�ʷ����������
private:
	FILE *fin;//�����ļ�
	FILE *fout;//�ʷ��������ļ�
	int curline;//��ǰ����
public:
	lexAnalysis();
	lexAnalysis(FILE *fin, FILE *fout);
	void scanwords();//ɨ�������ļ������д���
	int clearnotes(char buffer[], int finish_note);//���ע��
	void getwords(char buffer[]);//���������
	void wordresult(char word[]);//������ʷ������
};

#endif