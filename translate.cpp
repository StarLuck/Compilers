#define _CRT_SECURE_NO_WARNINGS
#include "translate.h"

vartable enterproc(vartable &parenttable, char name[]) {//Ϊ�����������ӱ�
	vartable newtable;
	strcpy(newtable.funcname, name);
	newtable.offset = 0;
	newtable.vararr.reserve(10);
	newtable.subtables.reserve(10);
	parenttable.subtables.push_back(newtable);
	return newtable;
}

void enter(vartable &parenttable, char varname[], int is_var, int is_param, int is_temp) {//��ӱ���
	variable newvar;
	strcpy(newvar.varname, varname);
	newvar.is_var = is_var;
	newvar.is_temp = is_temp;
	if (is_param == 0) {
		parenttable.vararr.push_back(newvar);
		parenttable.offset += INT_SIZE;
	}
	else
		parenttable.param.push_back(newvar);
}

void generate_intermediate_code(vector<intermediate_code> &intercode, const char* op, variable arg1, variable arg2, variable result, bool islabel)//�����м����
{
	intermediate_code newcode;
	newcode.codeno = intercode.size();
	strcpy(newcode.op, op);
	newcode.arg1 = arg1;
	newcode.arg2 = arg2;
	newcode.result = result;
	newcode.is_label = islabel;
	intercode.push_back(newcode);
}


variable find_in_table(vartable table, const char* varname)//���ұ����Ƿ��Ѷ���
{
	for (int i = 0; i < table.vararr.size(); i++) {
		if (strcmp(table.vararr[i].varname, varname) == 0) {
			return table.vararr[i];
		}
	}
	for (int i = 0; i < table.param.size(); i++) {
		if (strcmp(table.param[i].varname, varname) == 0) {
			return table.param[i];
		}
	}
	variable nv;
	nv.is_var = -1;
	return nv;
}

vartable find_func_in_table(vartable maintable, const char* funcname)//���Һ����Ƿ��Ѷ���
{
	for (int i = 0; i < maintable.subtables.size(); i++) {
		if (!strcmp(maintable.subtables[i].funcname, funcname)) {
			return maintable.subtables[i];
		}
	}
	vartable table;
	table.param_num = -1;
	return table;
}

void update_table(vartable &maintable, vartable subtable)//����maintable���ӱ�
{
	for (int i = 0; i < maintable.subtables.size(); i++) {
		if (strcmp(maintable.subtables[i].funcname, subtable.funcname) == 0) {
			maintable.subtables[i] = subtable;
			break;
		}
	}
}

void backpatch(vector<intermediate_code> &intercode, list<int> &List, int M)//����
{
	list<int>::iterator it = List.end();
	it--;
	sprintf(intercode[*it].result.varname, "%d", M);
	List.pop_back();
}