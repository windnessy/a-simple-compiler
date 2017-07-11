#define _CRE_SECURE_NO_WARNINGS
#include <fstream>
#include "ir.h"
#include "assembly.h"

int main(int argc, int **argv)
{
	IR ir;
	
	char filename[300];
	cout << "������Ҫ�����м������ļ�:";
	cin >> filename;
	ifstream in(filename, ios::in);
	if (!in)
	{
		cout << "��" << filename << "ʧ�ܣ�" << endl;
		system("pause");
		return 0;
	}
	ofstream fout("result.txt");
	cout.rdbuf(fout.rdbuf());
	istreambuf_iterator<char> beg(in), end;
	string input(beg, end);
	in.close();

	ir.analyze(input);
	ir.printResult();

	Assembly assembly;

	assembly.output();

	fout.flush();
	fout.close();
	system("pause");
	return 0;
}