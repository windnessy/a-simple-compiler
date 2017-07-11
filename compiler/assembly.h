#ifndef __ASSEMBLY_H_
#define __ASSEMBLY_H_

#include "gen.h"
#include "symbol.h"

#include <string>
#include <stdio.h>
#include <vector>

using namespace std;

#define BUF_LEN  4096

enum {
	DATA, CODE
};

class Assembly {
private:
	char ASMFileName[256];
	FILE *ASMFile;

	char OutBuffer[BUF_LEN];
	int BufferSize;

private:
	void EmitTranslationUnit();
	void BeginProgram();
	void Segment(int seg);
	void EmitGlobals();
	void EmitFunctions();
	void EndProgram();

	void PutString(char *s);
	void Flush();

	void EmitFunction(FunctionSymbol p);
	void DefineGlobal(Symbol p);
	void DefineLabel(Symbol p);
	void Export(Symbol p);
public:
	void output();
};

#endif