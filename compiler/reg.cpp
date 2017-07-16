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

	// �Ĵ���ָ��ķ���
	p = reg->link;

	if (p)
	{
		// ��üĴ������, ����ָ��NULL

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

	// �ҳ��Ĵ�������������С��
	for (i = T0; i <= T9; i++)
	{
		if(i >T7 && i<T8 )
			continue;
		if (Regs[i] == NULL || (1 << i & UsedRegs))
			continue;

		// �ҵ���ָ��ķ���
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
	@ �������Q	: GetRegInternal
	@ ����		: 
	@ ����		: ���ҿ��õļĴ���
	@ ����ֵ		: ���ؿ��õļĴ���
*/

Symbol Reg::GetRegInternal()
{
	int i;

	// �����ҵ�һ���ռĴ�����û���򷵻�NO_REG
	i = FindEmptyReg();
	// ���]�п���ļĴ���
	if (i == NO_REG)
	{
		i = SelectSpillReg();
		SpillReg(Regs[i]);
	}

	UsedRegs |= 1 << i;	//���ñ�־λ

	return Regs[i];
}
