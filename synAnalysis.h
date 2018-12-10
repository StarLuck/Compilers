#ifndef _SYNANALYSIS_H
#define _SYNANALYSIS_H

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <list>
#include <vector>
#include "lexAnalysis.h"
#include "translate.h"

using namespace std;

class synAna {
private:
	FILE *synresult;//�﷨��������ļ�ָ��
	FILE *interresult;//�м��������ļ�ָ��
	list<Token>::iterator it;
	list<Token> tokenList;
	vartable curtable;//��ǰ���ű�
	bool has_return;  //�����Ƿ�return
	int curvarno;//��ǰ�м�������
	list<int> truelist;//���������
	list<int> falselist;//�ٳ�������
	list<int> nextlist;//��һ���
	int M1, M2, N;//���ڻ���
	bool is_boolexp;//�Ƿ��ǲ������ʽ
	vector<int> label_arr;//��ת��ַ����
public:
	vartable maintable;//�ܷ��ű�
	vector<intermediate_code> intercode;
public:
	synAna(FILE *synResult, FILE *interResult, lexAnalysis lex);
	void program();//��������
private:
	void declaration_chain();	//������
	void declaration();			//����������ȫ�ֱ����ͺ�������
	void function();			//����
	void type(char word[]);				//�������ͣ��������ߺ���
	void fparameter();			//�β�
	void parameter_list();		//�β��б�
	void param();				//����
	void statement_block();		//����
	void innerdec();			//�ڲ�����
	void statement_chain();		//��䴮
	void innervardec();			//�ڲ���������
	void statement();			//���
	void if_statement();		//if���
	void while_statement();		//while���
	void return_statement();	//return���
	void assignment_statement();//��ֵ���
	variable expression();			//���ʽ
	variable plus_expression();		//�ӷ����ʽ
	variable term();				//��
	variable factor();				//����
	variable call(char word[]);				//call
	int argument_list(const char* funcname);		//ʵ���б�
	int argument(const char* funcname);			//ʵ��
	void ERROR(int linenum, char errinfo[]);				//����
};

#endif