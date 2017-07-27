#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "compiler.h"
#include "template.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

/*
	@ 输出到文件，输入缓冲区，刷新缓冲区
	@ PutString
	@ PutChar
	@ Flush
	@ PutASMCode
*/

char tmp1[1024];
	
void Compiler::SetUpASMfile(const char *output)
{

	char name[256], *p = SourceFileName;
	while (*p != '.') {
		name[p - SourceFileName] = *p++;
	}
	name[p - SourceFileName] = '\0';

	if (output == NULL)
		sprintf(ASMFileName, "%s.s", name);
	else
		sprintf(ASMFileName, "%s", output);

	if (ASMFileName) {
		ASMFile = fopen(ASMFileName, "w");
	}
}

void Compiler::PutString(const char *s)
{
	int len = strlen(s);
	int i;
	char *p;

	if (len > BUF_LEN)
	{
		fwrite(s, 1, len, ASMFile);
		return;
	}

	if (len > BUF_LEN - BufferSize)
	{
		Flush();
	}

	p = OutBuffer + BufferSize;
	for (i = 0; i < len; ++i)
	{
		p[i] = s[i];
	}
	BufferSize += len;
}

void Compiler::PutChar(const char c)
{
	if (BufferSize + 1 > BUF_LEN)  
	{                              
		Flush();                   
	}                              
	OutBuffer[BufferSize++] = c;
}

void Compiler::Flush(void)
{
	if (BufferSize != 0)
	{
		fwrite(OutBuffer, 1, BufferSize, ASMFile);
	}
	BufferSize = 0;
}

void Compiler::PutASMCode(const char *str, Symbol opds[])
{
	int i;
	const char *fmt = str;
	PutChar('\t');
	while (*fmt)
	{
		switch (*fmt)
		{
		case ';':
			PutString("\n\t");
			break;

		case '%':

			fmt++;
			if (*fmt == '%')
			{
				PutChar('%');
			}
			else
			{
				i = *fmt - '0';
				if (opds[i]->reg != NULL)
				{
					PutString((opds[i]->reg->name).c_str());
				}
				else
				{
					PutString(GetAccessName(opds[i]).c_str());
				}
			}
			break;

		default:
			PutChar(*fmt);
			break;
		}
		fmt++;
	}
	PutChar('\n');
}

/*
	@ 初始化寄存器
	- SetupRegisters

	@ 分配寄存器
	- AllocateReg
*/
void Compiler::SetupRegisters()
{
	/*

	*/
	int i;
	for (i = 0; i <= RA; i++) {
		Regs[i] = NULL;
	}
	Regs[ZERO] = CreateReg("$zero", ZERO);

	Regs[V0] = CreateReg("$v0", V0);
	Regs[V1] = CreateReg("$v1", V1);

	Regs[A0] = CreateReg("$a0", A0);
	Regs[A1] = CreateReg("$a1", A1);
	Regs[A2] = CreateReg("$a2", A2);
	Regs[A3] = CreateReg("$a3", A3);

	Regs[T0] = CreateReg("$t0", T0);
	Regs[T1] = CreateReg("$t1", T1);
	Regs[T2] = CreateReg("$t2", T2);
	Regs[T3] = CreateReg("$t3", T3);
	Regs[T4] = CreateReg("$t4", T4);
	Regs[T5] = CreateReg("$t5", T5);
	Regs[T6] = CreateReg("$t6", T6);
	Regs[T7] = CreateReg("$t7", T7);

	Regs[T8] = CreateReg("$t8", T8);
	Regs[T9] = CreateReg("$t9", T9);

	
	
}

void Compiler::AllocateReg(IRInst inst, int index)
{
	Symbol reg;
	Symbol p;

	p = inst->opds[index];


	if (p->kind != SK_Temp)
		return;


	if (p->reg != NULL)
	{
		UsedRegs |= 1 << p->reg->val;
		return;
	}
	
	if (index == 0 && SRC1->ref == 1 && SRC1->reg != NULL)
	{
		reg = SRC1->reg;
		reg->link = NULL;

		p->link = reg->link;
		reg->link = p;
		p->reg = reg;
		return;
	}

	reg = GetRegInternal();
	if (index != 0)
	{
		Move(reg, p);
	}

	p->link = reg->link;
	reg->link = p;
	p->reg = reg;
}

Symbol Compiler::PutInReg(Symbol p)
{
	Symbol reg;
	if (p->reg != NULL)
	{
		return p->reg;
	}
	reg = GetRegInternal();
	Load(reg, p);
	return reg;
}


string Compiler::GetAccessName(Symbol p)
{
	if (p->aname != "")
		return p->aname;

	switch (p->kind)
	{
	case SK_Constant:
		p->aname = p->name;
		break;
	case SK_Variable:
	case SK_Temp:
		if (p->level == 0)
			p->aname = "_zzn_" + p->name;
		else {
			char tmp[100];
			sprintf(tmp, "%d($fp)", ((VariableSymbol)p)->offset);
			p->aname = tmp;
		}
		break;
	case SK_Label:
		p->aname = "_zzn_" + p->name;
		break;
	case SK_Function:
		p->aname = "_zzn_" + p->name;
		break;
	}

	return p->aname;
}

void Compiler::Export(Symbol p)
{
	char tmp[1024];
	sprintf(tmp, "\t.globl\t_zzn_%s\n", p->name.c_str());
	PutString(tmp);
	/*
	sprintf(tmp, "\t.ent\t%s\n", p->name.c_str());
	PutString(tmp);
	
	sprintf(tmp, "\t.type\t%s, @function\n", p->name.c_str());
	PutString(tmp);
	*/
}


void Compiler::LayoutFrame(FunctionSymbol fsym, int fstParamPos)
{
	int offset;

	offset = fstParamPos * PRESERVE_REGS;
	vector<Symbol>::iterator it = fsym->params.begin();
	for (; it != fsym->params.end(); it++) {
		((VariableSymbol)(*it))->offset = offset;
		offset += PRESERVE_REGS;
	}
	offset = fsym->arguments * PRESERVE_REGS;
	it = fsym->locals.begin();
	for (; it != fsym->locals.end(); it++) {
		((VariableSymbol)(*it))->offset = offset;
		offset += PRESERVE_REGS;
	}
}

void Compiler::Move(Symbol dst, Symbol src)
{
	Symbol opds[2];

	opds[0] = dst;
	opds[1] = src;

	PutASMCode(MIPS_MOVE, opds);
	
}

void Compiler::Save(Symbol dst, Symbol src)
{
	Symbol opds[2];

	if (dst->reg != NULL) {
		SpillReg(dst->reg);
	}

	opds[0] = dst;
	opds[1] = src;
	PutASMCode(MIPS_SW, opds);
}

void Compiler::Load(Symbol dst, Symbol src)
{
	Symbol opds[2];

	opds[0] = dst;
	opds[1] = src;
	PutASMCode(MIPS_LW, opds);
}

void Compiler::PushArgument(Symbol arg, int n)
{
	if (n > 4) {
		Symbol opds[2];
		opds[0] = Regs[A0 + n];
		opds[1] = arg;
		if (arg->kind == SK_Variable)
			PutASMCode(MIPS_LW, opds);
		else
			PutASMCode(MIPS_MOVE, opds);
	}
	else {

	}
}


/*
	函数定义
	@ EmitPrologue() 保护栈结构
	@ EmitEpilogue() 恢复栈结构
*/

void Compiler::EmitPrologue(int varsize)
{

	int offset = varsize * 4 ;
	sprintf(tmp1,"\taddiu\t$sp,$sp,-%d\n", offset);
	PutString(tmp1);
	
	sprintf(tmp1,"\tsw\t$31,%d($sp)\n", offset - 4);
	PutString(tmp1);
	
	sprintf(tmp1,"\tsw\t$fp,%d($sp)\n", offset - 8);
	PutString(tmp1);
	
	sprintf(tmp1,"\tmove\t$fp,$sp\n");
	PutString(tmp1);
}

void Compiler::EmitEpilogue(int varsize)
{
	int offset = 4 * varsize  ;
	
	sprintf(tmp1,"\tmove\t$fp,$sp\n");
	PutString(tmp1);
	
	sprintf(tmp1,"\tlw\t$31,%d($sp)\n", offset - 4);
	PutString(tmp1);
	
	
	sprintf(tmp1,"\tlw\t$fp,%d($sp)\n", offset - 8);
	PutString(tmp1);
	
	sprintf(tmp1,"\taddiu\t$sp,$sp,%d\n", offset);
	PutString(tmp1);
	
	sprintf(tmp1,"\tjr\t$31\n\tnop\n");
	PutString(tmp1);
}

void Compiler::EmitArguments(FunctionSymbol p)
{
	int size = (p->params.size() > 4) ? 4 : p->params.size();
	for (int i = 0; i <size; i++)
	{
		Save(p->params[i], Regs[A0 + i]);
	}
}
/*
	产生汇编指令
	@ EmitMove
	@ EmitAssign
	@ EmitBranch
	@ EmitJump
	@ EmitCall
	@ EmitReturn
*/
void Compiler::EmitMove(IRInst inst)
{
	if (SRC1->kind == SK_Constant)
	{
		SpillReg(Regs[V0]);

		
		Symbol opds[2];
		opds[0] = Regs[V0];
		opds[1] = SRC1;
		PutASMCode(MIPS_LI, opds);

		if (DST->kind != SK_Temp)
		{
			Save(DST, Regs[V0]);
		}
		else {
			DST->reg = Regs[V0];
			Regs[V0]->link = DST;
		}
	}
	else
	{
		AllocateReg(inst, 1);
		AllocateReg(inst, 0);
		if (SRC1->reg == NULL && DST->reg == NULL)
		{
			Load(Regs[V0], SRC1);
			Save(DST, Regs[V0]);
		}
		else
		{
			Save(DST, SRC1);
			SpillReg(SRC1->reg);
		}
	}

	if (DST) DST->ref--;
	if (SRC1) SRC1->ref--;
}

void Compiler::EmitAssign(IRInst inst)
{
	switch (OP)
	{
	case $ADD:
	case $SUB:

		AllocateReg(inst, 1);
		AllocateReg(inst, 2);
		AllocateReg(inst, 0);

		if (SRC1->kind == SK_Constant && SRC2->kind == SK_Constant) {

			Symbol opds[2];
			opds[0] = DST;
			opds[1] = SRC1;
			PutASMCode(MIPS_LI, opds);

			opds[0] = DST;
			if (OP == $ADD) opds[1] = SRC1;
			if (OP == $SUB) opds[1] = AddConstant(-SRC1->val);

			PutASMCode(MIPS_ADDIU, opds);

		}
		else if (SRC1->kind == SK_Constant) {
			if (DST->reg != SRC2->reg)
			{
				Load(DST, SRC2);
			}

			Symbol opds[2];
			opds[0] = DST;
			if (OP == $ADD) opds[1] = SRC1;
			if (OP == $SUB) opds[1] = AddConstant(-SRC1->val);

			PutASMCode(MIPS_ADDIU, opds);
		}
		else if (SRC2->kind == SK_Constant) {
			if (DST->reg != SRC1->reg)
			{
				Load(DST, SRC1);
			}

			Symbol opds[2];
			opds[0] = DST;
			if (OP == $ADD) opds[1] = SRC2;
			if (OP == $SUB) opds[1] = AddConstant(-SRC2->val);

			PutASMCode(MIPS_ADDIU, opds);
		}
		else {
			if (DST->reg != SRC1->reg)
			{
				Load(DST, SRC1);
			}

			Symbol reg = PutInReg(SRC2);

			Symbol opds[2];
			opds[0] = DST;
			opds[1] = reg;

			if (OP == $ADD) PutASMCode(MIPS_ADDU, opds);
			if (OP == $SUB) PutASMCode(MIPS_SUBU, opds);
		}
		break;

	case $MUL:
	case $DIV:

		AllocateReg(inst, 1);
		AllocateReg(inst, 2);
		AllocateReg(inst, 0);
		if (SRC1->kind == SK_Constant && SRC2->kind == SK_Constant)
		{
			Symbol opds[2];
			opds[0] = DST;
			opds[1] = SRC1;
			PutASMCode(MIPS_LI, opds);

			Symbol reg = GetRegInternal();
			opds[0] = reg;
			opds[1] = SRC2;
			PutASMCode(MIPS_LI, opds);

			opds[0] = DST;
			opds[1] = reg;
			if (OP == $MUL) PutASMCode(MIPS_MUL, opds);
			if (OP == $DIV) PutASMCode(MIPS_DIV, opds);
		}
		else if (SRC1->kind == SK_Constant)
		{
			if (DST->reg != SRC2->reg)
			{
				Load(DST, SRC2);
			}

			Symbol reg = GetRegInternal();
			Symbol opds[2];
			opds[0] = reg;
			opds[1] = SRC1;
			PutASMCode(MIPS_LI, opds);

			opds[0] = DST;
			opds[1] = reg;
			if (OP == $MUL) PutASMCode(MIPS_MUL, opds);
			if (OP == $DIV) PutASMCode(MIPS_DIV, opds);

		}
		else if (SRC2->kind == SK_Constant)
		{
			if (DST->reg != SRC1->reg)
			{
				Load(DST, SRC1);
			}

			Symbol reg = GetRegInternal();
			Symbol opds[2];
			opds[0] = reg;
			opds[1] = SRC2;
			PutASMCode(MIPS_LI, opds);

			opds[0] = DST;
			opds[1] = reg;
			if (OP == $MUL) PutASMCode(MIPS_MUL, opds);
			if (OP == $DIV) PutASMCode(MIPS_DIV, opds);
		}
		else {
			if (DST->reg != SRC1->reg)
			{
				Load(DST, SRC1);
			}

			Symbol reg = PutInReg(SRC2);

			Symbol opds[2];
			opds[0] = DST;
			opds[1] = reg;

			if (OP == $MUL) PutASMCode(MIPS_MUL, opds);
			if (OP == $DIV) PutASMCode(MIPS_DIV, opds);

		}

		break;
	default:
		break;
	}

	if (DST) DST->ref--;
	if (SRC1) SRC1->ref--;
	if (SRC2) SRC2->ref--;
}

void Compiler::EmitBranch(IRInst inst)
{
	BBlock p = (BBlock)DST;
	DST = p->sym;

	SRC1->ref--;
	SRC2->ref--;
	if (SRC1->kind == SK_Constant && SRC2->kind == SK_Constant) {
		Symbol reg1, reg2;
		Symbol opds[2];

		// load im
		reg1 = GetRegInternal();
		opds[0] = reg1;
		opds[1] = SRC1;

		PutASMCode(MIPS_LI, opds);
		SRC1->reg = reg1;
		reg1->link = SRC1;

		// load im
		reg2 = GetRegInternal();
		opds[0] = reg2;
		opds[1] = SRC2;

		PutASMCode(MIPS_LI, opds);
		SRC2->reg = reg2;
		reg2->link = SRC2;

		if (OP == $JE) {
			PutASMCode(MIPS_BEQ, inst->opds);
		}
		else if (OP == $JNE) {
			PutASMCode(MIPS_BNE, inst->opds);
		}
		else if (OP == $JG) {
			Symbol opds[3];
			opds[0] = SRC2;
			opds[1] = SRC1;
			PutASMCode(MIPS_SLT, opds);

			opds[0] = DST;
			opds[1] = SRC2;
			opds[2] = Regs[ZERO];
			PutASMCode(MIPS_BNE, opds);
		}
		else if (OP == $JL) {
			Symbol opds[3];
			opds[0] = SRC1;
			opds[1] = SRC2;
			PutASMCode(MIPS_SLT, opds);

			opds[0] = DST;
			opds[1] = SRC1;
			opds[2] = Regs[ZERO];
			PutASMCode(MIPS_BNE, opds);
		}
		else if (OP == $JGE) {
			Symbol opds[3];
			opds[0] = SRC1;
			opds[1] = SRC2;
			PutASMCode(MIPS_SLT, opds);

			opds[0] = DST;
			opds[1] = SRC1;
			opds[2] = Regs[ZERO];
			PutASMCode(MIPS_BEQ, opds);
		}
		else if (OP == $JLE) {
			Symbol opds[3];
			opds[0] = SRC2;
			opds[1] = SRC1;
			PutASMCode(MIPS_SLT, opds);

			opds[0] = DST;
			opds[1] = SRC2;
			opds[2] = Regs[ZERO];
			PutASMCode(MIPS_BEQ, opds);
		}
	}
	else if (SRC1->kind == SK_Constant) {
		Symbol reg1, reg2;
		Symbol opds[2];

		if (SRC2->reg == NULL) {
			reg2 = GetRegInternal();
			Load(reg2, SRC2);
			SRC2->reg = reg2;
			reg2->link = SRC2;
		}

		reg1 = GetRegInternal();
		opds[0] = reg1;
		if (OP != $JL && OP != $JGE) {
			opds[1] = SRC1;
		}
		else
			opds[1] = AddConstant(SRC1->val + 1);

		PutASMCode(MIPS_LI, opds);
		SRC1 = reg1;

		Symbol tempS;
		tempS = SRC1;
		SRC1 = SRC2;
		SRC2 = tempS;
		if (OP == $JE) {
			PutASMCode(MIPS_BEQ, inst->opds);
		}
		else if (OP == $JNE) {
			PutASMCode(MIPS_BNE, inst->opds);
		}
		else if (OP == $JG || OP == $JGE) {
			opds[0] = SRC1;
			opds[1] = SRC2;
			PutASMCode(MIPS_SLT, opds);

			SRC2 = Regs[ZERO];
			PutASMCode(MIPS_BNE, inst->opds);
		}
		else if (OP == $JL || OP == $JLE) {
			opds[0] = SRC1;
			opds[1] = SRC2;
			PutASMCode(MIPS_SLT, opds);

			SRC2 = Regs[ZERO];
			PutASMCode(MIPS_BEQ, inst->opds);
		}
	}
	else if (SRC2->kind == SK_Constant) {
		Symbol reg1, reg2;
		Symbol opds[2];

		if (SRC1->reg == NULL) {
			reg1 = GetRegInternal();
			Load(reg1, SRC1);
			SRC1->reg = reg1;
			reg1->link = SRC1;
		}

		reg2 = GetRegInternal();
		opds[0] = reg2;
		if (OP != $JG && OP != $JLE) {
			opds[1] = SRC2;
		}
		else
			opds[1] = AddConstant(SRC2->val + 1);

		PutASMCode(MIPS_LI, opds);
		SRC2 = reg2;
		if (OP == $JE) {
			PutASMCode(MIPS_BEQ, inst->opds);
		}
		else if (OP == $JNE) {
			PutASMCode(MIPS_BNE, inst->opds);
		}
		else if (OP == $JG || OP == $JGE) {
			opds[0] = SRC1;
			opds[1] = SRC2;
			PutASMCode(MIPS_SLT, opds);

			SRC2 = Regs[ZERO];
			PutASMCode(MIPS_BEQ, inst->opds);
		}
		else if (OP == $JL || OP == $JLE) {
			opds[0] = SRC1;
			opds[1] = SRC2;
			PutASMCode(MIPS_SLT, opds);

			SRC2 = Regs[ZERO];
			PutASMCode(MIPS_BNE, inst->opds);
		}
	}
	else {
		Symbol reg1, reg2;
		if (SRC1->reg == NULL) {
			reg1 = GetRegInternal();
			Load(reg1, SRC1);
			SRC1->reg = reg1;
			reg1->link = SRC1;
		}

		if (SRC2->reg == NULL) {
			reg2 = GetRegInternal();
			Load(reg2, SRC2);
			SRC2->reg = reg2;
			reg2->link = SRC2;
		}

		if (OP == $JE) {
			PutASMCode(MIPS_BEQ, inst->opds);
		}
		else if (OP == $JNE) {
			PutASMCode(MIPS_BNE, inst->opds);
		}
		else if (OP == $JG) {
			Symbol opds[3];
			opds[0] = SRC2;
			opds[1] = SRC1;
			PutASMCode(MIPS_SLT, opds);

			opds[0] = DST;
			opds[1] = SRC2;
			opds[2] = Regs[ZERO];
			PutASMCode(MIPS_BNE, opds);
		}
		else if (OP == $JL) {
			Symbol opds[3];
			opds[0] = SRC1;
			opds[1] = SRC2;
			PutASMCode(MIPS_SLT, opds);

			opds[0] = DST;
			opds[1] = SRC1;
			opds[2] = Regs[ZERO];
			PutASMCode(MIPS_BNE, opds);
		}
		else if (OP == $JGE) {
			Symbol opds[3];
			opds[0] = SRC1;
			opds[1] = SRC2;
			PutASMCode(MIPS_SLT, opds);

			opds[0] = DST;
			opds[1] = SRC1;
			opds[2] = Regs[ZERO];
			PutASMCode(MIPS_BEQ, opds);
		}
		else if (OP == $JLE) {
			Symbol opds[3];
			opds[0] = SRC2;
			opds[1] = SRC1;
			PutASMCode(MIPS_SLT, opds);

			opds[0] = DST;
			opds[1] = SRC2;
			opds[2] = Regs[ZERO];
			PutASMCode(MIPS_BEQ, opds);
		}
	}
	ClearRegs();
}

void Compiler::EmitJump(IRInst inst)
{
	DST = ((BBlock)DST)->sym;
	PutASMCode(MIPS_B, inst->opds);
}

void Compiler::EmitCall(IRInst inst)
{
	ParameterList argslist = (ParameterList)SRC2;
	Symbol arg;
	int i, stksize = 0;

	for (i = argslist->npara - 1; i >= 0; i--)
	{
		arg = argslist->args[i];
		PushArgument(arg, i);
		stksize += sizeof(int) * 1;
	}
	// call func
	PutASMCode(MIPS_JAL, inst->opds);

	if (DST) DST->ref--;

	DST->reg = Regs[V0];
}


void Compiler::EmitReturn(IRInst inst)
{
	if (DST->reg != Regs[V0])
	{
		if (DST->reg == NULL)
			Load(Regs[V0], DST);
		else 
			Move(Regs[V0], DST);
	}
}

/*

*/
void Compiler::EmitIRInst(IRInst inst) 
{
	switch (inst->opcode)
	{
	case $MOV:
		EmitMove(inst);
		break;
	case $ADD:
	case $SUB:
		EmitAssign(inst);
		break;
	case $MUL:
	case $DIV:
		EmitAssign(inst);
		break;
	case $JE:
	case $JNE:
	case $JG:
	case $JL:
	case $JGE:
	case $JLE:
		EmitBranch(inst);
		break;
	case $JMP:
		EmitJump(inst);
		break;
	case $CALL:
		EmitCall(inst);
		break;
	case $RETU:
		EmitReturn(inst);
	default:
		break;
	}
}

void Compiler::EmitBBlock(BBlock bb)
{
	IRInst inst = bb->insth.next;

	while (inst != &bb->insth)
	{
		UsedRegs = 0;
		EmitIRInst(inst);
		inst = inst->next;
	}
	// 清空寄存器
	ClearRegs();
}

void Compiler::EmitFunction(FunctionSymbol p)
{
	BBlock bb;
	int varsize;

	Export((Symbol)p);
	DefineLabel((Symbol)p);


	varsize = p->locals.size() + p->arguments + 2;
	
	LayoutFrame(p, varsize);
	
	EmitPrologue(varsize);
	EmitArguments(p);

	bb = p->entryBB;
	while (true) 
	{
		if (bb->ref != 0) { DefineLabel(bb->sym); }

		EmitBBlock(bb);

		if (bb != p->exitBB)
			bb = bb->next;
		else
			break;

	}

	EmitEpilogue(varsize);
}

void Compiler::DefineGlobal(Symbol p)
{
	char tmp[1024];
	sprintf(tmp, "\t.comm\t%s,%d,%d\n", p->name.c_str(), sizeof(int), sizeof(int));
	PutString(tmp);
}

void Compiler::DefineLabel(Symbol p)
{
	char tmp[1024];

	sprintf(tmp, "_zzn_%s:\n", p->name.c_str());
	PutString(tmp);
}

/*
	Emit Process
	@ BeginProgram
	@ Data_Segment
	@ EmitGlobals
	@ Text_Segment
	@ EmitFunctions
	@ EndProgram
*/

void Compiler::TranslationUnit(const char* input)
{
	// 初始化化寄存器
	SetupRegisters();

	//开始
	BeginProgram(input);

	//全局变量
	Data_Segment();
	EmitGlobals();

	//代码段
	Text_Segment();
	EmitFunctions();

	//刷新缓冲区
	EndProgram();

	fclose(ASMFile);
}

void Compiler::BeginProgram(const char* input)
{
	// Initialize register symbols to
	// make sure that no register contains data from variables.
	int i = 0;
	for (i = ZERO; i <= RA; i++)
	{
		if (Regs[i] != NULL) {
			Regs[i]->link = NULL;
		}
	}

	PutString("# Code auto-generated by zhin\n");
	char tmp[1024];
	sprintf(tmp,"\t.file\t1 \"%s\"\n", input);
	PutString(tmp);
}

void Compiler::Data_Segment()
{
	PutString(".data\n");
}

void Compiler::EmitGlobals(void)
{
	vector<Symbol>::iterator p = GlobalIDs.buckets.begin();

	for (; p != GlobalIDs.buckets.end(); p++) {
		if ((*p)->kind == SK_Variable) {
			DefineGlobal(*p);
		}
	}
}

void Compiler::Text_Segment()
{
	PutString(".text\n");
}

void Compiler::EmitFunctions(void)
{
	vector<Symbol>::iterator p = GlobalIDs.buckets.begin();

	for (; p != GlobalIDs.buckets.end(); p++) {
		if ((*p)->kind == SK_Function) {
			EmitFunction((FunctionSymbol)*p);
		}
	}
}

void Compiler::EndProgram(void)
{
	Flush();
}


/*
	
*/
void Compiler::analyze(string filename)
{
	strcpy(SourceFileName, filename.c_str());

	ifstream in(filename, ios::in);
	if (!in)
	{
		cout << "can't not open " << filename << endl;
		exit(0);
	}
	istreambuf_iterator<char> beg(in), end;
	string input(beg, end);
	in.close();

	IR::analyze(input);
	//IR::printResult();
}

/*
	@ 名稱: emit()
	@ 作用: 生成汇编代码
	@ 返回:
*/
void Compiler::emit(const char* input, const char *output)
{
	//
	SetUpASMfile(output);

	BufferSize = 0;

	TranslationUnit(input);
}

/*
	@ 外界接口
*/
void Compiler::Compilation(const char *input,const char *output)
{
	Error::filename = (char*)input;

	analyze(input);
	emit(input,output);
}
