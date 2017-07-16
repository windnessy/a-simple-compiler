#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "ir.h"
#include "compiler.h"

using namespace std;

#define DEBUG_PRINT(msg) {printf("zcc: %s\n", msg);}

int main(int argc, char **argv)
{
	char *input = NULL, *output = NULL;

	if (argc <= 3) {
		cout << "options:" << endl;
		cout << "compiler src.c -s dst.s " << endl;
		cout << "compiler src.c -ps dst.ps " << endl;
		getchar();
		return 0;
	}
	if (argc == 4) {
		input = argv[1];
		output = argv[3];
		if (!strcmp(argv[2], "-s")) {
			Compiler compiler;
			compiler.Compilation(input, output);
			return 0;
		}
		if (!strcmp(argv[2], "-ps")) {
			IR ir;
			ifstream in(input, ios::in);
			if (!in)
			{
				cout << "can not open" << input << endl;
				system("pause");
				return 0;
			}
			ofstream fout(output);
			cout.rdbuf(fout.rdbuf());
			istreambuf_iterator<char> beg(in), end;
			string input1(beg, end);
			in.close();

			ir.analyze(input1);
			ir.printResult();
			fout.flush();
			fout.close();
			return 0;
		}
		cout << "options:" << endl;
		cout << "compiler src.c -s dst.s " << endl;
		cout << "compiler src.c -ps dst.ps " << endl;
		
	}
	system("pause");

	return 0;
}