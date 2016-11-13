#include "Assembler.h"
#include "VirtualMachine.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <iostream>
#include <map>
#include <ctype.h>


using namespace std;



int main(int argc, char* argv[])
{
	/*if (argv[1] == NULL)
	{
		cout << "Please enter file";
			exit(1);
	}*/

	Assembler asmFile;
	VirtualMachine play;
	

	asmFile.passOne(argv); //parse and create symbol table
	asmFile.passTwo(argv); //validate labels and input data to asmFile.memory
	play.runMachine(asmFile); //run obcodes in asmFile.memory
	system("PAUSE");
}




