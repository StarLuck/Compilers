#define _CRT_SECURE_NO_WARNINGS
#include "lexAnalysis.h"
#include "base.h"

#define MAXLINE 4096
lexAnalysis::lexAnalysis()
{

}
lexAnalysis::lexAnalysis(FILE *fin, FILE *fout)
{
	this->fin = fin;
	this->fout = fout;
	this->curline = 0;
}

void lexAnalysis::scanwords()//ɨ�������ļ������д���
{
	char ch;
	int count = 0; //ÿһ���ַ�����
	int chgline = 0;//����
	char bufferscan[MAXLINE];
	memset(bufferscan, 0, sizeof(bufferscan));
	int finish_note = 2;
	printf("��ʼ�ʷ�����...\n\n");
	while (1)
	{
		ch = fgetc(fin);
		if (ch == '#' || ch == EOF) {//�����ļ�β
			fflush(fout);
			break;
		}
		if (ch == '\n') {//���У���ʼ����
			curline++;
			fprintf(fout, "%d: %s\n", curline, bufferscan);
			finish_note = clearnotes(bufferscan, finish_note);
			if (finish_note == 2) {
				getwords(bufferscan);
			}
			else if (finish_note == 1) {
				finish_note = 2;
			}
			memset(bufferscan, 0, sizeof(bufferscan));
			count = 0;
			continue;
		}
		else {
			bufferscan[count++] = ch;
		}
	}
	printf("�ʷ��������...\n\n");
}

int lexAnalysis::clearnotes(char buffer[], int finish_note)//���ע��
{
	int i, j;
	int flag = 0;//0��ʾ���ַ���״̬��1��ʾ�ַ���״̬
	char note[MAXLINE];
	memset(note, 0, sizeof(note));
	int count = 0;
	int finish = 0;
	for (i = 0; buffer[i] != '\0'; i++) {
		if (finish_note >0) {
			if (buffer[i] == '"') {
				flag = 1 - flag;
				continue;
			}
			if (buffer[i] == '/'&&flag == 0) {
				if (buffer[i + 1] == '/') {//����ע��
					for (j = i; buffer[j] != '\0'; j++) {
						note[count++] = buffer[j];
					}
					note[count] = '\0';
					count = 0;
					buffer[i] = '\0';
					fprintf(fout, " %s \t\t\tע��\n", note);
					finish = 2;
					return finish;
				}
				else if (buffer[i + 1] == '*') {//����ע��
					note[count++] = '/';
					note[count++] = '*';
					for (j = i + 2; buffer[j] != '\0'; j++) {
						note[count++] = buffer[j];
						if (buffer[j] == '*'&&buffer[j + 1] == '/') {
							note[count++] = '/';
							note[count] = '\0';
							count = 0;
							j += 2;
							finish = 1;
							break;
						}
					}
					if (strlen(note)>0)
						fprintf(fout, " %s \t\t\tע��\n", note);
					for (; buffer[j] != '\0'; j++)
						buffer[i] = buffer[j];
					buffer[i] = '\0';
					return finish;
				}
			}
		}//end if
		else {
			note[count++] = buffer[i];
			if (buffer[i] == '*'&&buffer[i + 1] == '/') {//��������ע��
				finish = 1;
				note[count++] = buffer[i + 1];
				note[count] = '\0';
				count = 0;
				i += 1;
			}
		}
	}
	if (strlen(note)>0)
		fprintf(fout, " %s \t\t\tע��\n", note);
	if (finish_note == 2 && finish == 0)
		finish = 2;
	return finish;
}

void lexAnalysis::getwords(char buffer[])//���������
{
	char word[100];
	memset(word, 0, sizeof(word));
	int i, j = 0;
	for (i = 0; buffer[i] != '\0'; i++) {
		int chkind = charkind(buffer[i]);
		switch (chkind)
		{
		case LETTER:case NUMBER:
			while (chkind == LETTER || chkind == NUMBER) {
				word[j++] = buffer[i++];
				chkind = charkind(buffer[i]);
			}
			i--;
			word[j] = '\0';
			wordresult(word);
			j = 0;
			break;
		case PUNCTUATION:
			word[0] = buffer[i];
			if ((buffer[i] == '=' || buffer[i] == '!' || buffer[i] == '<' || buffer[i] == '>') && buffer[i + 1] == '=')
			{
				word[1] = '=';
				word[2] = '\0';
				i++;
			}
			else
				word[1] = '\0';

			wordresult(word);
			j = 0;
			break;
		}
	}
}

void lexAnalysis::wordresult(char word[])//������ʷ������
{
	Token token;
	strcpy(token.word, word);
	token.linenum = curline;
	if (is_keyword(word)) {
		fprintf(fout, " %s \t\t\t�ؼ���\n", word);
		token.type = token_type::keyword;
	}
	else if (is_signword(word)) {
		fprintf(fout, " %s \t\t\t��ʶ��\n", word);
		token.type = token_type::ID;
	}
	else if (is_numberstr(word)) {
		fprintf(fout, " %s \t\t\t��\n", word);
		token.type = token_type::num;
	}
	else if (strlen(word) == 1) {
		switch (word[0])
		{
		case '=':
			fprintf(fout, " %s \t\t\t ��ֵ/�����\n", word);
			token.type = token_type::assignment;
			break;
		case '+':case '-':
			fprintf(fout, " %s \t\t\t �����\n", word);
			token.type = token_type::plus;
			break;
		case '*':case '/':
			fprintf(fout, " %s \t\t\t �����\n", word);
			token.type = token_type::multiple;
			break;
		case '>':case '<':
			fprintf(fout, " %s \t\t\t �����\n", word);
			token.type = token_type::relop;
			break;
			break;
		case ';':
			fprintf(fout, " %s \t\t\t ���\n", word);
			token.type = token_type::semicolon;
			break;
		case ',':
			fprintf(fout, " %s \t\t\t �ָ���\n", word);
			token.type = token_type::comma;
			break;
		case '(':
			fprintf(fout, " %s \t\t\t ������\n", word);
			token.type = token_type::lbracket;
			break;
		case ')':
			fprintf(fout, " %s \t\t\t ������\n", word);
			token.type = token_type::rbracket;
			break;
		case '{':
			fprintf(fout, " %s \t\t\t �������\n", word);
			token.type = token_type::lbrace;
			break;
		case '}':
			fprintf(fout, " %s \t\t\t �Ҵ�����\n", word);
			token.type = token_type::rbrace;
			break;
		}
	}
	else if (!strcmp(word, "==") || !strcmp(word, "!=") || !strcmp(word, ">=") || !strcmp(word, "<=")) {
		fprintf(fout, " %s \t\t\t �����\n", word);
		token.type = token_type::relop;
	}
	else {
		fprintf(fout, " %s \t\t\t �Ƿ���\n", word);
		printf("�ʷ����󣺵�%d�У� %s : �Ƿ���\n", curline, word);
		getchar();
		exit(0);
	}
	if (word[0] != ' '&&word[0] != '\t')
		this->tokenList.push_back(token);
}