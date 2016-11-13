#include "VirtualMachine.h"
#include <iostream>
#include <map>


using namespace std;

VirtualMachine::VirtualMachine()
{
}


VirtualMachine::~VirtualMachine()
{
}

void VirtualMachine::runMachine(Assembler asmFile)
{
	map<string, int> symbol_table = asmFile.symbol_table;
	char* vmCP = asmFile.mem;
	int* vmIP = (int*)asmFile.mem;
	int* vmIPvalue;
	int* pc = asmFile.pc;
	bool running = true;
	int desReg, sourceReg, move;
	char moveChar;

	while (running)
	{
		vmCP = asmFile.mem + *pc;
		switch ((int)*vmCP)
		{
		case 1: //JMP - branch to label
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			*pc = *vmIP;
			break;
		case 2: //JMR - branch to address in source reg
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			pc = vmIP;
			break;
		case 3: //BNZ - branch to label if SR is not 0
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			if (*vmIP != 0)
			{
				*pc = *pc + 4;
				vmCP = asmFile.mem + *pc;
				vmIP = (int*)vmCP;
				*pc = *vmIP;
				break;
			}
			else
			{
				*pc = *pc + 8;
				break;
			}
		case 4: //BGT - branch to label if SR is greater than 0
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			if (*vmIP > 0)
			{
				*pc = *pc + 4;
				vmCP = asmFile.mem + *pc;
				vmIP = (int*)vmCP;
				*pc = *vmIP;
				break;
			}
			else
			{
				*pc = *pc + 8;
				break;
			}
		case 5: //BLT - branch to label if SR is less than 0
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			if (*vmIP < 0)
			{
				*pc = *pc + 4;
				vmCP = asmFile.mem + *pc;
				vmIP = (int*)vmCP;
				*pc = *vmIP;
				break;
			}
			else
			{
				*pc = *pc + 8;
				break;
			}
		case 6: //BRZ - branch to label if SR is 0
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			if (*vmIP == 0)
			{
				*pc = *pc + 4;
				vmCP = asmFile.mem + *pc;
				vmIP = (int*)vmCP;
				*pc = *vmIP;
				break;
			}
			else
			{
				*pc = *pc + 8;
				break;
			}
		case 7: //MOV
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			move = *vmIP;
			vmIP = (int*)asmFile.mem + desReg;
			*vmIP = move;
			*pc = *pc + 4;
			break;
		case 8: //LDA
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmIP = (int*)asmFile.mem + desReg;
			vmCP = asmFile.mem + *pc;
			vmIPvalue = (int*)vmCP;
			*vmIP = *vmIPvalue;
			*pc = *pc + 4;
			break;
		case 9: //STR
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			*pc = *pc + 4;
			vmIP = (int*)asmFile.mem + sourceReg;
			move = *vmIP;
			vmCP = asmFile.mem + *pc;
			vmCP = asmFile.mem + *vmCP;
			vmIP = (int*)vmCP;
			*vmIP = move;
			*pc = *pc + 4;
			break;
		case 10: //LDR
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmIP = (int*)asmFile.mem + desReg;
			vmCP = asmFile.mem + *pc;
			vmCP = asmFile.mem + *vmCP;
			vmIPvalue = (int*)vmCP; //cast to int* to handle larger numbers
			*vmIP = *vmIPvalue;
			*pc = *pc + 4;
			break;
		case 11:
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			*pc = *pc + 4;
			vmIP = (int*)asmFile.mem + sourceReg;
			vmCP = asmFile.mem + *pc;
			*vmCP = *vmIP;
			*pc = *pc + 4;
			break;
		case 12:  //LDB
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmIP = (int*)asmFile.mem + desReg;
			vmCP = asmFile.mem + *pc;
			vmCP = asmFile.mem + *vmCP;
			*vmIP = *vmCP;
			*pc = *pc + 4;
			break;
		case 13: //ADD
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			move = *vmIP;
			vmIP = (int*)asmFile.mem + desReg;
			move = move + *vmIP;
			*vmIP = move;
			*pc = *pc + 4;
			break;
		case 14: //ADI
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + desReg;
			move = *vmIP + sourceReg;
			*vmIP = move;
			*pc = *pc + 4;
			break;
		case 15: //SUB
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			move = *vmIP;
			vmIP = (int*)asmFile.mem + desReg;
			move = *vmIP - move;
			*vmIP = move;
			*pc = *pc + 4;
			break;
		case 16: //MUL
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			move = *vmIP;
			vmIP = (int*)asmFile.mem + desReg;
			move = *vmIP * move;
			*vmIP = move;
			*pc = *pc + 4;
			break;
		case 17: //DIV
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			move = *vmIP;
			vmIP = (int*)asmFile.mem + desReg;
			move = *vmIP / move;
			*vmIP = move;
			*pc = *pc + 4;
			break;
		case 20: //CMP - set DR to 0 if DR = SR; set dr to >0 if DR is > sr; set dr to <0 if dr < sr
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			move = *vmIP;
			vmIP = (int*)asmFile.mem + desReg;
			move = *vmIP - move;
			*vmIP = move;
			*pc = *pc + 4;
			break;
		case 21: //TRP
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			if (*vmIP == 0)
			{
				running = false;
			}
			else if (*vmIP == 1)
			{
				vmIP = (int*)asmFile.mem + 3;
				cout << *vmIP;
			}
			else if (*vmIP == 2)
			{
				cout << "pc2 Op Code not implemented sorry! \n"; exit(1);
				exit(1);
			}
			else if (*vmIP == 3)
			{
				vmCP = asmFile.mem + 12;
				if (*vmCP == 92 || *vmCP == 47)
				{
					cout << "\n";
				}
				else
				{
					cout << *vmCP;
				}
			}
			else if (*vmIP == 4)
			{
				cout << "pc 4 Op Code not implemented sorry! \n"; exit(1);
				exit(1);
			}
			else
			{
				cout << "Op Code not implemented sorry! \n";
				exit(1);
			}
			*pc = *pc + 4;
			break;
		case 22: //STR (reg indirect)
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			move = *vmIP;
			vmIP = (int*)asmFile.mem + desReg;
			vmCP = asmFile.mem + *vmIP;
			vmIP = (int*)vmCP;
			*vmIP = move;
			*pc = *pc + 4;
			break;
		case 23: //LDR (reg indirect)
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			vmCP = asmFile.mem + *vmIP;
			vmIP = (int*)vmCP;
			move = *vmIP;
			vmIP = (int*)asmFile.mem + desReg;
			*vmIP = move;
			*pc = *pc + 4;
			break;
		case 24: //STB (reg indirect)
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			vmCP = (char*)vmIP;
			moveChar = *vmCP;
			vmIP = (int*)asmFile.mem + desReg;
			vmCP = asmFile.mem + *vmIP;
			*vmCP = moveChar;
			*pc = *pc + 4;
			break;
		case 25: //LDB (reg indirect)
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			desReg = *vmIP;
			*pc = *pc + 4;
			vmCP = asmFile.mem + *pc;
			vmIP = (int*)vmCP;
			sourceReg = *vmIP;
			vmIP = (int*)asmFile.mem + sourceReg;
			vmCP = asmFile.mem + *vmIP;
			moveChar = *vmCP;
			vmIP = (int*)asmFile.mem + desReg;
			vmCP = (char*)vmIP;
			*vmCP = moveChar;
			*pc = *pc + 4;
			break;
		default:
			cout << "Op Code " << *pc << " not implemented sorry!\n";
			exit(1);
		}
	}

}