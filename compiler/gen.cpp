#include "gen.h"

BBlock CurrentBB = NULL;	//當前的基本块

BBlock CreateBBlock(void)
{
	BBlock bb = new bblock;

	bb->prev = NULL;
	bb->next = NULL;
	bb->insth.opcode = $NOP;
	bb->insth.prev = bb->insth.next = &bb->insth;
	bb->ninst = 0;

	bb->nsucc = 0;
	bb->npred = 0;
	bb->ref = 0;

	return bb;
}

void StartBBlock(BBlock bb)
{
	if (CurrentBB == NULL) {
		CurrentBB = bb;
	}
	else {
		IRInst lasti;

		CurrentBB->next = bb;
		bb->prev = CurrentBB;
		lasti = CurrentBB->insth.prev;

		if (lasti->opcode != $JMP && lasti->opcode != $IJMP)	//不是轉跳指令
		{
			CurrentBB->succs = bb;
			bb->prev = CurrentBB;
		}

		CurrentBB = bb;
	}
}

void AppendInst(IRInst inst)
{

	if (CurrentBB != NULL) {
		CurrentBB->insth.prev->next = inst;
		inst->prev = CurrentBB->insth.prev;
		inst->next = &CurrentBB->insth;
		CurrentBB->insth.prev = inst;

		CurrentBB->ninst++;
	}
	else {
		//error

	}
}

void GenerateMove(Symbol dst, Symbol src)
{
	IRInst inst = new irinst;

	dst->ref++;
	src->ref++;
	inst->opcode = $MOV;
	inst->opds[0] = dst;
	inst->opds[1] = src;
	inst->opds[2] = NULL;
	AppendInst(inst);
}

void GenerateBranch(BBlock dstBB, int opcode, Symbol src1, Symbol src2)
{
	IRInst inst = new irinst;

	dstBB->ref++;
	src1->ref++;
	if (src2) src2->ref++;
	CurrentBB->succs = dstBB;

	inst->opcode = opcode;
	inst->opds[0] = (Symbol)dstBB;
	inst->opds[1] = src1;
	inst->opds[2] = src2;

	AppendInst(inst);
}

void GenerateJump(BBlock dstBB)
{
	IRInst inst = new irinst;

	dstBB->ref++;
	CurrentBB->succs = dstBB;

	inst->opcode = $JMP;
	inst->opds[0] = (Symbol)dstBB;
	inst->opds[1] = inst->opds[2] = NULL;

	AppendInst(inst);
}

void GenerateAssign(Symbol dst, int opcode, Symbol src1, Symbol src2)
{
	IRInst inst = new irinst;

	dst->ref++;
	src1->ref++;
	if (src2) src2->ref++;
	
	inst->opcode = opcode;
	inst->opds[0] = dst;
	inst->opds[1] = src1;
	inst->opds[2] = src2;

	AppendInst(inst);
}
void GenerateFunctionCall(Symbol recv, Symbol faddr, ParameterList args)
{
	IRInst inst = new irinst;

	if (recv) recv->ref++;
	faddr->ref++;
	vector<Symbol>::iterator it = args->args.begin();
	for (; it != args->args.end(); it++) {
		(*it)->ref++;
	}

	inst->opcode = $CALL;
	inst->opds[0] = recv;
	inst->opds[1] = faddr;
	inst->opds[2] = (Symbol)args;
	AppendInst(inst);
}

void GenerateReturn(Symbol src)
{
	IRInst inst = new irinst;
	
	src->ref++;
	inst->opcode = $RETU;
	inst->opds[0] = src;
	inst->opds[1] = inst->opds[2] = NULL;
	AppendInst(inst);
}

void GenerateRet()
{
	IRInst inst = new irinst;

	inst->opcode = $RET;
	inst->opds[0] = inst->opds[1] = inst->opds[2] = NULL;
	AppendInst(inst);
}