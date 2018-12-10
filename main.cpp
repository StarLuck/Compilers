#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <list>

#include "base.h"
#include "lexAnalysis.h"
#include "SynAnalysis.h"
#include "asmGen.h"

using namespace std;

int main()
{
	FILE *fin = fopen("in.txt", "r");
	if (!fin) {
		printf("��Դ���ļ�ʧ��\n");
		return 0;
	}
	FILE *lexout = fopen("lexAnaResult.txt", "w+");
	if (!lexout) {
		printf("�򿪴ʷ���������ļ�ʧ��\n");
		return 0;
	}
	FILE *synResult = fopen("synResult.txt", "w+");
	if (!synResult) {
		printf("���﷨��������ļ�ʧ��\n");
		return 0;
	}
	FILE *interResult = fopen("interCode.txt", "w+");
	if (!interResult) {
		printf("���м�������ļ�ʧ��\n");
		return 0;
	}
	FILE *asmResult = fopen("asmCode.asm", "w+");
	if (!asmResult) {
		printf("�򿪻��������ļ�ʧ��\n");
		return 0;
	}
	printf("�ʷ���������ļ���lexAnaResult.txt\n�﷨��������ļ���synResult.txt\n�м�����ļ���interCode.txt\nĿ������ļ���asmCode.asm\n\n\n");

	lexAnalysis lexresult = lexAnalysis(fin, lexout);//�ʷ�����
	lexresult.scanwords();

	synAna syn(synResult, interResult, lexresult);//�﷨�������м��������
	syn.program();

	ASMCODE asmCode(asmResult, syn.maintable, syn.intercode);//Ŀ���������
	asmCode.generate_code();
	getchar();
	if (fin)
		fclose(fin);
	if (lexout)
		fclose(lexout);
	if (synResult)
		fclose(synResult);
	if (interResult)
		fclose(interResult);
	if (asmResult)
		fclose(asmResult);
	return 0;
}