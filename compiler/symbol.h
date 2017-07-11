#ifndef __SYMBOL_H_
#define __SYMBOL_H_

#include <string>
#include <map>
#include <vector>

using namespace std;

enum
{
	SK_Constant, SK_Variable, SK_Temp,
	SK_Label, SK_Function, SK_Register
};

#define SYMBOL_COMMON     \
    int		kind;         \
    string	name;         \
    int		level;        \
    int		ref;          \
	int		val;		  \
    struct symbol *reg;   \
    struct symbol *next;

typedef struct bblock *BBlock;

typedef struct symbol
{
	SYMBOL_COMMON
} *Symbol;

typedef struct variableSymbol
{
	SYMBOL_COMMON
	int	idata;
} *VariableSymbol;

typedef struct functionSymbol
{
	SYMBOL_COMMON
	Symbol params;
	Symbol locals;
	int nbblock;
	BBlock entryBB;
	BBlock exitBB;
} *FunctionSymbol;

typedef struct table
{
	vector<Symbol> buckets;
	int level;
	struct table *outer;
} *Table;

void InitSymbolTable(void);
void EnterScope(void);
void ExitScope(void);

Symbol AddVariable(string name);
Symbol AddFunction(string name);
Symbol AddSymbol(Table tbl, Symbol sym);
Symbol AddConstant(int i);
Symbol CreateTemp();
Symbol CreateLabel();
Symbol IntConstant(int i);
Symbol LookupID(string name);
Symbol DoLookupSymbol(Table tbl, string name, int  searchOuter = 1);

#endif