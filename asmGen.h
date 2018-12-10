#ifndef _ASMGEN_H
#define _ASMGEN_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <stack>

#include "synAnalysis.h"

using namespace std;

struct reginfo {//�Ĵ�����Ϣ
	char regname[7];	//�Ĵ�����
	char varname[100];	//�洢�ı�����
	int offset;			//������ջ�е�ƫ����
	int is_temp;		//�Ƿ�����ʱ����
};

struct reglist {
	list<reginfo> emptylist;
	list<reginfo> usedlist;
	int jaddr;
};

class ASMCODE {//������
private://�Ĵ���
	const char* EAX = "AX";
	const char* EBX = "BX";
	const char* ECX = "CX";
	const char* EDX = "DX";
	const char* ESP = "SP";
	const char* EBP = "BP";
	const char* EDI = "DI";
	const char* ESI = "SI";
	const char* EIP = "IP";
	const char* EFLAGS = "FLAGS";
private://ָ��
	const char* MOV = "MOV";
	const char* ADD = "ADD";
	const char* SUB = "SUB";
	const char* NEG = "NEG";
	const char* PUSH = "PUSH";
	const char* POP = "POP";
	const char* PUSHA = "PUSHA";
	const char* POPA = "POPA";
	const char* LEA = "LEA";
	const char* INC = "INC";
	const char* DEC = "DEC";
	const char* IMUL = "IMUL";
	const char* IDIV = "IDIV";
	const char* JMP = "JMP";
	const char* CMP = "CMP";
	const char* JE = "JE";
	const char* JNE = "JNE";
	const char* JG = "JG";
	const char* JGE = "JGE";
	const char* JL = "JL";
	const char* JLE = "JLE";
	const char* CALL = "CALL";
	const char* RET = "RET";
	const char* Int = "INT";
private:
	FILE *asmresult;
	vartable maintable;//�ܷ��ű�
	vartable curtable;//��ǰ���ű�
	vector<intermediate_code> intercode;
	list<reginfo> empty_reg_list;//���мĴ���
	list<reginfo> used_reg_list;//��ռ�üĴ���
	vector<reglist> list_stack;
	int should_pop;
public:
	ASMCODE();
	ASMCODE(FILE *fout, vartable table, vector<intermediate_code> code);
	void generate_code();//��ʼ����Ŀ�����
private:
	void print_asmcode(const char* code);//���������
	void generate_datasegment();//�������ݶ�
	void generate_codesegment_head();//�����ͷ
	void generate_codesegment_stop();//��ֹ����
	void generate_codesegment_tail();//����ν������
	void generate_instruction(const char* op, const char* dst, const char* src);//˫������ָ��
	void generate_instruction(const char* op, const char* dst, int src);//˫������ָ��
	void generate_instruction(const char* op, const char* src);//��������ָ��
	void init_empty_reg_list();//��ʼ�����мĴ���
	void clear_used_reg_list();//���ʹ�õļĴ���
	reginfo is_in_reg(const char* varname);//�����Ƿ��ڼĴ�����
	reginfo get_empty_reg(variable arg, int willchange = 0);//����յļĴ���
	reginfo get_spec_reg(const char* regname);//����ָ���ļĴ���
	void write_to_mem(reginfo reg);//�Ĵ�������д���ڴ�
	void get_addr(int offset, char *addr);//�õ�������ַ
	int pos_of_var(const char* varname);//����ָ��������ջ�е�ƫ�Ƶ�ַ
};

#endif
