#ifndef __REG_H_
#define __REG_H_
#include "symbol.h"

#define NO_REG	-1


enum {ZERO,AT,V0,V1,A0,A1,A2,A3,T0,T1,T2,T3,T4,T5,T6,T7,S0,S1,S2,S3,S4,S5,S6,S7,T8,T9,GP,SP,FP,RA};						

class Reg {
protected:
	Symbol Regs[RA+1];
	int UsedRegs;

public:
	Symbol	CreateReg(string name, int no);

	void	ClearRegs();
	void	SpillReg(Symbol reg);
	int		SelectSpillReg();
	int		FindEmptyReg();

	Symbol	GetRegInternal();

};
#endif
