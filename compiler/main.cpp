#define _CRE_SECURE_NO_WARNINGS
#include <fstream>
#include "ir.h"
#include "assembly.h"

int main(int argc, int **argv)
{
	IR ir;
	
	char filename[300];
	cout << "输入需要生成中间代码的文件:";
	cin >> filename;
	ifstream in(filename, ios::in);
	if (!in)
	{
		cout << "打开" << filename << "失败！" << endl;
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