#ifndef _TRANSLATE_H
#define _TRANSLATE_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <list>
#include <string.h>

using namespace std;

#define INT_SIZE 2
#define VOID 1
#define INT 2

struct variable {
	char varname[100];	//������
	int is_var;			//�Ƿ������0��ʾ��������1��ʾ������2��ʾ��ת������ת��ַ��
	int is_temp;		//�Ƿ�����ʱ����
};

struct vartable {
	char funcname[100];//������
	int offset;		   //��Ҫ����Ĵ洢�ռ�
	int param_num;	   //�βθ���
	int rettype;	   //��������
	vector<variable> param;//�β�
	vector<variable> vararr;//����
	vector<vartable> subtables;//�ӱ�
};

struct intermediate_code {
	int codeno;
	char op[10];
	variable arg1;
	variable arg2;
	variable result;
	bool is_label;//�Ƿ�����ת��ַ
};

vartable enterproc(vartable &parenttable, char name[]);//Ϊ�����������ӱ�
void enter(vartable &parenttable, char varname[], int is_var, int is_param, int is_temp = 0);//��ӱ���
void generate_intermediate_code(vector<intermediate_code> &intercode, const char* op, variable arg1, variable arg2, variable result, bool islabel = false);//�����м����
variable find_in_table(vartable table, const char* varname);//���ұ����Ƿ��Ѷ���
vartable find_func_in_table(vartable maintable, const char* funcname);//���Һ����Ƿ��Ѷ���
void update_table(vartable &maintable, vartable subtable);//����maintable���ӱ�
void backpatch(vector<intermediate_code> &intercode, list<int> &List, int M);//����

#endif