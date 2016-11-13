#pragma once
#include "Assembler.h"
#include <string>
#include <map>
using namespace std;
class VirtualMachine
{
public:
	VirtualMachine();
	~VirtualMachine();
	void runMachine(Assembler);
};

