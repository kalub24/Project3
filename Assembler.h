#pragma once
#include <string>
#include <map>
using namespace std;
class Assembler
{
public:
	char mem[10000];
	map<string, int> symbol_table;
	char *cp;
	int *ip;
	int pcStart;
	int *pc;
	int NumOfReg;
	int index; //0 - 32 are reserved for registers 0 - 8 (4 bytes each)

	Assembler();
	~Assembler();
	void passOne(char**);
	void passTwo(char**);
	int indexValue(char*);
	int opCodeValue(char*, string);
	bool isRegister(char *);
	bool isLabel(char*);
	bool isLabel(char *, string);
	bool isInstruction(char *);
	bool isInstruction(char *, string);
	bool isDirective(char * c);
	bool isDirective(char *, string);
};

