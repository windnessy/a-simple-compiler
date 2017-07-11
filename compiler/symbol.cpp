#include "symbol.h"

// normal identifiers in current scope
Table Identifiers;
// normal identifiers in global scope
struct table GlobalIDs;
// all the constants
struct table Constants;

// Level increment; exiting each nesting level, Level decrement
int Level;
// number of temporaries
int TempNum;
// number of labels, see CreateLabel(void)
int LabelNum;

Symbol DoLookupSymbol(Table tbl, string name, int  searchOuter) {
	vector<Symbol>::iterator it;

	do {
		for (it = tbl->buckets.begin(); it != tbl->buckets.end(); it++)
		{
			if ((*it)->name == name) {
				return  *it;
			}
		}
	} while ((tbl = tbl->outer) != NULL && searchOuter);

	return NULL;
}

Symbol LookupID(string name)
{
	return DoLookupSymbol(Identifiers, name);
}

void EnterScope(void)
{
	Table t = new table;

	Level++;

	t->level = Level;
	t->outer = Identifiers;
	Identifiers = t;
}

void ExitScope(void)
{
	Level--;
	Identifiers = Identifiers->outer;
}

Symbol AddVariable(string name)
{
	VariableSymbol p = new variableSymbol;

	p->kind = SK_Variable;
	p->name = name;
	p->ref = 0;

	return AddSymbol(Identifiers, (Symbol)p);
}

Symbol AddFunction(string name)
{
	FunctionSymbol p = new functionSymbol;

	p->kind = SK_Function;
	p->name = name;
	//p->lastv = &p->params;
	p->ref = 0;

	if (Identifiers != &GlobalIDs) {
		AddSymbol(Identifiers, (Symbol)p);
	}

	return AddSymbol(&GlobalIDs, (Symbol)p);
}

Symbol AddSymbol(Table tbl, Symbol sym)
{
	sym->level = tbl->level;
	tbl->buckets.push_back((Symbol)sym);

	return sym;
}

Symbol AddConstant(int i)
{
	Symbol p = new symbol;

	p->kind = SK_Constant;

	char buf[256];
	sprintf(buf, "%d", i);
	p->name = buf;
	p->ref = 0;
	p->val = i;

	Constants.buckets.push_back((Symbol)p);

	return p;
}
Symbol CreateTemp()
{
	VariableSymbol p = new variableSymbol;

	p->kind = SK_Temp;

	char buf[256];
	sprintf(buf, "t%d", TempNum++);
	p->name = buf;

	p->level = 1;

	return (Symbol)p;
}

Symbol CreateLabel(void)
{
	Symbol p = new symbol;

	p->kind = SK_Label;

	char buf[256];
	sprintf(buf, "BB%d", LabelNum++);
	p->name = buf;

	return p;
}

Symbol IntConstant(int i)
{
	return AddConstant(i);
}

void InitSymbolTable(void)
{
	Level = 0;

	Identifiers = &GlobalIDs;

	TempNum = LabelNum = 0;

}