#include "reg.h"


Symbol Reg::CreateReg(string name, int no)
{
	Symbol reg;

	reg = new symbol;
	reg->kind = SK_Register;
	reg->name = reg->aname = name;
	reg->val = no;
	reg->reg = reg;

	return reg;
}

void Reg::ClearRegs()
{
	int i;

	for (i = ZERO; i <= RA; i++)
	{
		if (Regs[i])
			SpillReg(Regs[i]);
	}
}

void Reg::SpillReg(Symbol reg)
{
	Symbol p;

	// 寄存器指向的符号
	p = reg->link;

	if (p)
	{
		// 令该寄存器清空, 令其指向NULL

		p->reg = NULL;

	}
	reg->link = NULL;
}

int Reg::SelectSpillReg()
{
	Symbol p;
	int i;
	int reg = NO_REG;
	int mref = INT_MAX;

	// 找出寄存器中引用数最小的
	for (i = T0; i <= T9; i++)
	{
		if(i >T7 && i<T8 )
			continue;
		if (Regs[i] == NULL || (1 << i & UsedRegs))
			continue;

		// 找到它指向的符号
		p = Regs[i]->link;
		if (p->ref < mref)
		{
			mref = p->ref;
			reg = i;
		}
	}
	return reg;
}

int Reg::FindEmptyReg()
{
	int i;
	for (i = T0; i <= T7; i++)
	{
		if (Regs[i] != NULL && Regs[i]->link == NULL && !(1 << i & UsedRegs))
			return i;
	}
	
	for (i = T8; i <= T9; i++)
	{
		if (Regs[i] != NULL && Regs[i]->link == NULL && !(1 << i & UsedRegs))
			return i;
	}
	return NO_REG;
}


/*
	@ 函數名稱	: GetRegInternal
	@ 參數		: 
	@ 作用		: 查找可用的寄存器
	@ 返回值		: 返回可用的寄存器
*/

Symbol Reg::GetRegInternal()
{
	int i;

	// 尝试找到一个空寄存器，没有则返回NO_REG
	i = FindEmptyReg();
	// 若沒有空余的寄存器
	if (i == NO_REG)
	{
		i = SelectSpillReg();
		SpillReg(Regs[i]);
	}

	UsedRegs |= 1 << i;	//设置标志位

	return Regs[i];
}
