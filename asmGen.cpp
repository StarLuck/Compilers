#include "asmGen.h"

ASMCODE::ASMCODE()
{

}

ASMCODE::ASMCODE(FILE *fout, vartable table, vector<intermediate_code> code)
{
	this->asmresult = fout;
	this->maintable = table;
	this->intercode = code;
}

void ASMCODE::generate_code()//��ʼ����Ŀ�����
{
	printf("��ʼ����Ŀ�����(80x86)...\n\n");
	/* ��ʼ����ջ */
	char stack[100] = "\t.386\n_STACK\tSEGMENT STACK USE16\n\tDB 3FFEh DUP(?)\nTOS\tDW ?\n_STACK\tENDS\n";
	print_asmcode(stack);
	/* ���ݶ� */
	generate_datasegment();
	/* ����� */
	generate_codesegment_head();
	/* ɨ����Ԫʽ */
	char code[50];
	char endp[20];
	int i;
	curtable = maintable;
	should_pop = 0;
	for (i = 0; i < intercode.size(); i++) {
		if (intercode[i].is_label) {
			char label[20];
			sprintf(label, "L%d:\n", i);
			print_asmcode(label);
		}
		if (list_stack.size() > 0) {
			vector<reglist>::iterator it = list_stack.begin();
			while (it != list_stack.end()) {
				if (it->jaddr == i) {
					if (strcmp(intercode[i - 1].op, "j")) {
						for (list<reginfo>::iterator it = used_reg_list.begin(); it != used_reg_list.end(); it++) {
							if (it->offset<0)
								write_to_mem(*it);
						}
					}
					empty_reg_list = it->emptylist;
					used_reg_list = it->usedlist;
					list_stack.erase(it);
					break;
				}
				it++;
			}
		}

		if (!strcmp(intercode[i].op, "define")) {//��������
			if (!strcmp(intercode[i].result.varname, "main")) {
				print_asmcode("Start:\n");
				generate_instruction(MOV, EAX, "_DATA");
				generate_instruction(MOV, "DS", EAX);
				print_asmcode("\tCLI\n");
				generate_instruction(MOV, EAX, "_STACK");
				generate_instruction(MOV, "SS", EAX);
				generate_instruction(MOV, ESP, "Offset TOS");
				print_asmcode("\tSTI\n");

			}
			else {
				sprintf(code, "%s\tPROC NEAR\n", intercode[i].result.varname);
				print_asmcode(code);
				sprintf(endp, "%s\tENDP\n", intercode[i].result.varname);
			}
			/* ��ʼ���ռĴ����������ʹ�üĴ��� */
			init_empty_reg_list();
			clear_used_reg_list();
			/* ����EBP��ESP */
			generate_instruction(PUSH, EBP);
			generate_instruction(MOV, EBP, ESP);
			/* ������ʱ�����ռ� */
			curtable = find_func_in_table(maintable, intercode[i].result.varname);
			int offset = curtable.offset;
			generate_instruction(SUB, ESP, offset);

		}
		else if (!strcmp(intercode[i].op, "=")) {//��ֵ
			reginfo resultreg = is_in_reg(intercode[i].result.varname);
			if (resultreg.offset == 0) {//���ڼĴ�����
				resultreg = get_empty_reg(intercode[i].result, 1);
			}
			if (intercode[i].arg1.is_var == 1) {//����
				reginfo emptyreg = is_in_reg(intercode[i].arg1.varname);
				if (emptyreg.offset == 0) {//���ڼĴ�����
					emptyreg = get_empty_reg(intercode[i].arg1);
				}
				generate_instruction(MOV, resultreg.regname, emptyreg.regname);
			}
			else {//������
				generate_instruction(MOV, resultreg.regname, intercode[i].arg1.varname);
			}
		}
		else if (!strcmp(intercode[i].op, "+") || !strcmp(intercode[i].op, "-")) {//�Ӽ�����
			reginfo argreg1, argreg2;
			int offset;
			char addr[10];
			reginfo resultreg = is_in_reg(intercode[i].result.varname);
			if (resultreg.offset == 0) {//���ڼĴ�����
				resultreg = get_empty_reg(intercode[i].result, 1);
			}
			if (intercode[i].arg1.is_var == 1) {//��һ������Ϊ����
				argreg1 = is_in_reg(intercode[i].arg1.varname);
				if (argreg1.offset == 0) {
					argreg1 = get_empty_reg(intercode[i].arg1);
				}
				generate_instruction(MOV, resultreg.regname, argreg1.regname);
			}
			else {//��һ������Ϊ������
				generate_instruction(MOV, resultreg.regname, intercode[i].arg1.varname);
			}
			if (intercode[i].arg2.is_var == 1) {//�ڶ�������Ϊ����
				argreg2 = is_in_reg(intercode[i].arg2.varname);
				if (argreg2.offset == 0) {
					argreg2 = get_empty_reg(intercode[i].arg2);
				}
				if (!strcmp(intercode[i].op, "+"))
					generate_instruction(ADD, resultreg.regname, argreg2.regname);
				else
					generate_instruction(SUB, resultreg.regname, argreg2.regname);
			}
			else {//�ڶ�������Ϊ������
				if (!strcmp(intercode[i].op, "+"))
					generate_instruction(ADD, resultreg.regname, intercode[i].arg2.varname);
				else
					generate_instruction(SUB, resultreg.regname, intercode[i].arg2.varname);
			}
		}
		else if (!strcmp(intercode[i].op, "*") || !strcmp(intercode[i].op, "/")) {
			reginfo reg_eax = get_spec_reg(EAX);
			int is_in_eax = 0;
			if (!strcmp(reg_eax.regname, EAX)) {
				if (strcmp(reg_eax.varname, intercode[i].arg1.varname))//arg1����AX��
					write_to_mem(reg_eax);
				else
					is_in_eax = 1;
			}
			else {
				list<reginfo>::iterator it = empty_reg_list.begin();
				while (it != empty_reg_list.end()) {
					if (!strcmp(it->regname, EAX)) {
						break;
					}
					it++;
				}
				reg_eax = *it;
				empty_reg_list.erase(it);
			}
			if (is_in_eax == 0 && intercode[i].arg1.is_var == 1) {
				reginfo resultreg = is_in_reg(intercode[i].arg1.varname);
				if (resultreg.offset == 0) {//���ڼĴ�����
					int offset = pos_of_var(intercode[i].arg1.varname);
					char addr[10];
					if (offset == 0)
						generate_instruction(MOV, EAX, intercode[i].arg1.varname);
					else {
						get_addr(offset, addr);
						generate_instruction(MOV, EAX, addr);
					}

					reg_eax.offset = offset;
				}
				else {
					generate_instruction(MOV, EAX, resultreg.regname);
					reg_eax.offset = resultreg.offset;
				}
				strcpy(reg_eax.varname, intercode[i].arg1.varname);
			}
			else if (intercode[i].arg1.is_var == 0)
				generate_instruction(MOV, EAX, intercode[i].arg1.varname);

			reginfo argreg2 = is_in_reg(intercode[i].arg2.varname);
			if (argreg2.offset == 0) {
				argreg2 = get_empty_reg(intercode[i].arg2);
			}
			generate_instruction(PUSH, EAX);
			should_pop = 1;
			if (!strcmp(intercode[i].op, "*"))
				generate_instruction(IMUL, argreg2.regname);
			else
				generate_instruction(IDIV, argreg2.regname);
			strcpy(reg_eax.varname, intercode[i].result.varname);
			reg_eax.offset = pos_of_var(intercode[i].result.varname);
			used_reg_list.push_back(reg_eax);
		}
		else if (!strcmp(intercode[i].op, "j")) {
			char jaddr[20];
			sprintf(jaddr, "L%s", intercode[i].result.varname);
			for (list<reginfo>::iterator it = used_reg_list.begin(); it != used_reg_list.end(); it++) {
				if (it->is_temp == 0 && it->offset<0)
					write_to_mem(*it);
			}
			int toj = atoi(intercode[i].result.varname);
			if (toj > i) {
				reglist rlist;
				rlist.emptylist = empty_reg_list;
				rlist.usedlist = used_reg_list;
				rlist.jaddr = toj;
				list_stack.push_back(rlist);
			}
			else {
				if (should_pop == 1)
					generate_instruction(POP, EAX);
			}
			should_pop = 0;
			generate_instruction(JMP, jaddr);
		}
		else if (!strcmp(intercode[i].op, "j<") || !strcmp(intercode[i].op, "j<=") || !strcmp(intercode[i].op, "j>") || !strcmp(intercode[i].op, "j>=")
			|| !strcmp(intercode[i].op, "j==") || !strcmp(intercode[i].op, "j!=")) {
			reginfo argreg1, argreg2;
			argreg1 = is_in_reg(intercode[i].arg1.varname);
			if (argreg1.offset == 0) {
				argreg1 = get_empty_reg(intercode[i].arg1);
			}
			if (intercode[i].arg2.is_var == 1) {
				argreg2 = is_in_reg(intercode[i].arg2.varname);
				if (argreg2.offset == 0) {
					argreg2 = get_empty_reg(intercode[i].arg2);
				}
				generate_instruction(CMP, argreg1.regname, argreg2.regname);
			}
			else {
				generate_instruction(CMP, argreg1.regname, intercode[i].arg2.varname);
			}

			char jaddr[20];
			sprintf(jaddr, "L%s", intercode[i].result.varname);

			if (!strcmp(intercode[i].op, "j<"))
				generate_instruction(JL, jaddr);
			else if (!strcmp(intercode[i].op, "j<="))
				generate_instruction(JLE, jaddr);
			else if (!strcmp(intercode[i].op, "j>"))
				generate_instruction(JG, jaddr);
			else if (!strcmp(intercode[i].op, "j>="))
				generate_instruction(JGE, jaddr);
			else if (!strcmp(intercode[i].op, "j=="))
				generate_instruction(JE, jaddr);
			else if (!strcmp(intercode[i].op, "j!="))
				generate_instruction(JNE, jaddr);
		}
		else if (!strcmp(intercode[i].op, "RET")) {//��������
			if (!strcmp(curtable.funcname, "main")) {
				generate_codesegment_stop();
			}
			else {
				if (curtable.rettype == INT) {//��Ҫ����
											  /* ָ�����ؽ���洢��EAX�Ĵ��� */
					int is_in_eax = 0;
					reginfo reg_eax = get_spec_reg(EAX);
					if (!strcmp(reg_eax.regname, EAX)) {
						if (strcmp(reg_eax.varname, intercode[i].arg1.varname))//arg1����AX��
							write_to_mem(reg_eax);
						else
							is_in_eax = 1;
					}
					else {
						list<reginfo>::iterator it = empty_reg_list.begin();
						while (it != empty_reg_list.end()) {
							if (!strcmp(it->regname, EAX)) {
								break;
							}
							it++;
						}
						reg_eax = *it;
						used_reg_list.erase(it);
					}
					if (is_in_eax == 0) {
						reginfo resultreg = is_in_reg(intercode[i].arg1.varname);
						if (resultreg.offset == 0) {//���ڼĴ�����
							int offset = pos_of_var(intercode[i].arg1.varname);
							char addr[10];
							if (offset == 0)
								generate_instruction(MOV, EAX, intercode[i].arg1.varname);
							else {
								get_addr(offset, addr);
								generate_instruction(MOV, EAX, addr);
							}

							reg_eax.offset = offset;
						}
						else {
							generate_instruction(MOV, EAX, resultreg.regname);
							reg_eax.offset = resultreg.offset;
						}
						strcpy(reg_eax.varname, intercode[i].arg1.varname);
					}

					used_reg_list.push_back(reg_eax);
				}
				/* �ָ�ջ֡ */
				generate_instruction(MOV, ESP, EBP);
				generate_instruction(POP, EBP);
				char sp_offset[10] = "";
				if (curtable.param_num > 0)
					sprintf(sp_offset, "%d", curtable.param_num * 2);
				generate_instruction(RET, sp_offset);
				print_asmcode(endp);
			}
			curtable = maintable;
		}
		else if (!strcmp(intercode[i].op, "call")) {//���ú���
			reginfo reg_eax = get_spec_reg(EAX);
			if (!strcmp(reg_eax.regname, EAX)) {
				write_to_mem(reg_eax);
			}

			generate_instruction(CALL, intercode[i].arg1.varname);
			strcpy(reg_eax.varname, intercode[i].result.varname);
			reg_eax.offset = pos_of_var(intercode[i].result.varname);
			used_reg_list.push_back(reg_eax);
		}
		else if (!strcmp(intercode[i].op, "push")) {//ѹ��
			reginfo reg = is_in_reg(intercode[i].arg2.varname);
			if (reg.offset == 0) {//���ڼĴ�����
				reg = get_empty_reg(intercode[i].arg2);
			}
			generate_instruction(PUSH, reg.regname);
		}
	}
	generate_codesegment_tail();
	printf("Ŀ������������...\n\n");
}

void ASMCODE::print_asmcode(const char* code)//���������
{
	fprintf(asmresult, code);
	fflush(asmresult);
}

void ASMCODE::generate_datasegment()//�������ݶ�
{
	char data[500] = "_DATA\tSEGMENT 'DATA' USE16\n";
	char var[50] = "";
	for (int i = 0; i < maintable.vararr.size(); i++) {//���������ʼ��Ϊ0
		sprintf(var, "%s\tDD 0\n", maintable.vararr[i].varname);
		strcat(data, var);
	}
	strcat(data, "_DATA\tENDS\n");
	print_asmcode(data);
}

void ASMCODE::generate_codesegment_head()//�����ͷ
{
	char codehead[100] = "_TEXT\tSEGMENT 'CODE' USE16\n\tASSUME CS: _TEXT, DS: _DATA\n";
	print_asmcode(codehead);
}

void ASMCODE::generate_codesegment_stop()//��ֹ����
{
	generate_instruction(MOV, EAX, "4C00h");
	generate_instruction(Int, "21h");
}

void ASMCODE::generate_codesegment_tail()//�����β
{
	char codestail[100] = "_TEXT ENDS\n\tEND Start\n";
	print_asmcode(codestail);
}

void ASMCODE::generate_instruction(const char* op, const char* dst, const char* src)//˫������ָ��
{
	char code[50];
	sprintf(code, "\t%s\t%s, %s\n", op, dst, src);
	print_asmcode(code);
}

void ASMCODE::generate_instruction(const char* op, const char* dst, int src)//˫������ָ��
{
	char code[50];
	sprintf(code, "\t%s\t%s, %d\n", op, dst, src);
	print_asmcode(code);
}

void ASMCODE::generate_instruction(const char* op, const char* src)//��������ָ��
{
	char code[50];
	sprintf(code, "\t%s\t%s\n", op, src);
	print_asmcode(code);
}

void ASMCODE::init_empty_reg_list()//��ʼ�����мĴ���
{
	reginfo reg;
	empty_reg_list.clear();
	strcpy(reg.regname, EAX);
	empty_reg_list.push_back(reg);
	strcpy(reg.regname, EBX);
	empty_reg_list.push_back(reg);
	strcpy(reg.regname, ECX);
	empty_reg_list.push_back(reg);
	strcpy(reg.regname, EDX);
	empty_reg_list.push_back(reg);
}

void ASMCODE::clear_used_reg_list()//���ʹ�õļĴ���
{
	used_reg_list.clear();
}

reginfo ASMCODE::is_in_reg(const char* varname)//�����Ƿ��ڼĴ�����
{
	list<reginfo>::iterator it = used_reg_list.begin();
	while (it != used_reg_list.end()) {
		if (!strcmp(it->varname, varname)) {
			return *it;
		}
		it++;
	}
	reginfo reg;
	reg.offset = 0;
	return reg;
}

reginfo ASMCODE::get_empty_reg(variable arg, int willchange)//����յļĴ���
{
	reginfo emptyreg;
	int offset;
	char addr[10];
	if (empty_reg_list.size() == 0) {//�޿յļĴ������������õļĴ�����д��ԭ����
		emptyreg = used_reg_list.front();
		write_to_mem(emptyreg);
		used_reg_list.pop_front();
	}
	else {
		emptyreg = empty_reg_list.front();
		empty_reg_list.pop_front();
	}
	offset = pos_of_var(arg.varname);
	if (offset != 0) {//�ֲ�����
		get_addr(offset, addr);
	}
	emptyreg.offset = offset;
	strcpy(emptyreg.varname, arg.varname);
	emptyreg.is_temp = arg.is_temp;
	if (!willchange && offset != 0)
		generate_instruction(MOV, emptyreg.regname, addr);
	else if (!willchange && offset == 0 && emptyreg.is_temp == 0)
		generate_instruction(MOV, emptyreg.regname, arg.varname);
	used_reg_list.push_back(emptyreg);
	return emptyreg;
}

reginfo ASMCODE::get_spec_reg(const char* regname)//����ָ���ļĴ���
{
	list<reginfo>::iterator it = used_reg_list.begin();
	while (it != used_reg_list.end()) {
		if (!strcmp(it->regname, regname)) {
			break;
		}
		it++;
	}
	reginfo reg;
	if (it != used_reg_list.end()) {
		reg = *it;
		used_reg_list.erase(it);
	}
	else
		strcpy(reg.regname, "");
	return reg;
}

void ASMCODE::write_to_mem(reginfo reg)//�Ĵ�������д���ڴ�
{
	int offset;
	char addr[10];
	offset = reg.offset;
	if (offset == 0) {
		generate_instruction(MOV, reg.varname, reg.regname);
	}
	else {
		get_addr(offset, addr);
		generate_instruction(MOV, addr, reg.regname);
	}
}

void ASMCODE::get_addr(int offset, char *addr)//�õ�������ַ
{
	if (offset > 0) {//�β�
		sprintf(addr, "[BP+%d]", offset);
	}
	else {//��ʱ����
		sprintf(addr, "[BP%d]", offset);
	}
}

int ASMCODE::pos_of_var(const char* varname)//����ָ��������ջ�е�ƫ�Ƶ�ַ
{
	int offset = 0;
	for (int i = 0; i < curtable.param.size(); i++) {
		if (!strcmp(varname, curtable.param[i].varname)) {
			offset = 4 + 2 * (curtable.param_num - (i + 1));
			return offset;
		}
	}
	for (int i = 0; i < curtable.vararr.size(); i++) {
		if (!strcmp(varname, curtable.vararr[i].varname)) {
			offset = 2 + 2 * i;
			offset = -offset;
			return offset;
		}
	}
	return offset;
}

