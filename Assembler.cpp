#include "Assembler.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <iostream>
#include <map>
#include <ctype.h>

using namespace std;

Assembler::Assembler()
{
	cp = mem;
	ip = (int*)mem;
	pc = (int*)mem + 7;
	NumOfReg = 8; //7 reg + 1 PC reg
	index = NumOfReg * 4; //reserved for registers (4 bytes each)
	for (int i = 0; i < 32; i += 4) //initialize regiesters to 0
	{
		ip = (int*)mem + i;
		*ip = 0;
	}
}


Assembler::~Assembler()
{
}

void Assembler::passOne(char** argv)
{
	int lineNum = 1;
	char *myp, *move_token, *mystr;
	string temp;

	ifstream myfile("C:/Users/Caleb/Documents/Cs-4380/Project1/proj1.asm");
	//ifstream myfile(argv[1]); //read in file name
	if (myfile.is_open())
	{
		while (getline(myfile, temp))
		{
			transform(temp.begin(), temp.end(), temp.begin(), toupper);
			// cout << lineNum << " ";
			mystr = new char[temp.length() + 1];
			mystr[temp.size()] = 0;
			memcpy(mystr, temp.data(), temp.size());
			myp = strtok_s(mystr, " ,\t'", &move_token);


			if (myp == NULL || *myp == ';')
			{
				myp = NULL;
			}
			else if (isInstruction(myp, temp) == true)
			{
				if (pcStart == 0)
				{
					pcStart = index;
				}
				index = index + indexValue(myp);
			}
			else if (isDirective(myp, temp) == true)
			{
				index = index + indexValue(myp);
			}
			else if (isLabel(myp, temp) == true)
			{
				myp = strtok_s(NULL, " ,\t", &move_token);
				index = index + indexValue(myp);
			}
			else
			{
				cout << "error occured at line " << lineNum << " invalid use of " << myp << " - " << temp << "\n";
				exit(1);
				myp = strtok_s(NULL, " ,\t", &move_token);
			}
			lineNum = lineNum + 1;
		}
		myfile.close();
	}
	else
	{
		cout << "Failed to open file\n";
		exit(1);
	}
}
void Assembler::passTwo(char** argv)
{
	ip = (int*)mem + NumOfReg;
	cp = (char*)ip;
	char *myp, *move_token, *mystr;
	string temp;

	ifstream myfile("C:/Users/Caleb/Documents/Cs-4380/Project1/proj1.asm");
	//ifstream myfile(argv[1]); //read in file name
	if (myfile.is_open())
	{
		while (getline(myfile, temp))
		{
			transform(temp.begin(), temp.end(), temp.begin(), toupper);
			mystr = new char[temp.length() + 1];
			mystr[temp.size()] = 0;
			memcpy(mystr, temp.data(), temp.size());
			myp = strtok_s(mystr, " ,\t'", &move_token);
			while (myp != NULL)
			{
				if (*myp == ';')
				{
					myp = NULL;
					break;
				}
				else if (isInstruction(myp))
				{
					if (*pc == 0)
					{
						*pc = pcStart;
					}
					*ip = opCodeValue(myp, temp);
					ip = ip + 1;
					cp = (char*)ip;
					myp = strtok_s(NULL, " ,\t'", &move_token);
				}
				else if (isDirective(myp))
				{
					myp = strtok_s(NULL, ",\t'", &move_token);
				}
				else if (isRegister(myp))
				{
					*ip = opCodeValue(myp, temp);
					ip = ip + 1;
					cp = (char*)ip;
					myp = strtok_s(NULL, " ,\t'", &move_token);
				}
				else if (isLabel(myp))
				{
					string str = string(myp);
					if (symbol_table.find(str) == symbol_table.end() && *pc != 0) //find undeclared labels in instructions
					{
						if (atoi(myp) || *myp == '0') //immediate values
						{
							*ip = atoi(myp);
							ip = ip + 1;
							cp = (char*)ip;
						}
						else
						{
							cout << "*Undeclared Label found* " << myp << "\n";
						}
					}
					if ((*move_token == '\0' && *pc != 0) && !(symbol_table.find(str) == symbol_table.end())) //for labels after instructions
					{
							int labelAddress = symbol_table.at(str);
							*ip = labelAddress;
							ip = ip + 1;
							cp = (char*)ip;
					
					}
					else if (*move_token == '\0' && *pc == 0) //for labels before instructions
					{
						if (atoi(myp) || *myp == 48)
						{
							*ip = atoi(myp);
							ip = ip + 1;
							cp = (char*)ip;
						}
						else if ((isalpha(*myp) && myp[1] == '\0') || (*myp == 92 || *myp == 47) && myp[1] == 'N' || *myp == 32) //space and newline chars included
						{
							*cp = *myp;
							cp = cp + 1;
							ip = (int*)cp;
						}
						else
						{
							cout << "ERROR invalid use of " << myp << " - " << temp << "\n";
							myp = strtok_s(NULL, " ,\t'", &move_token);
						}
					}
				myp = strtok_s(NULL, " ,\t'", &move_token);
				}
				else
				{
					cout << "ERROR invalid use of " << myp << " - " << temp << "\n";
					exit(1);
					myp = strtok_s(NULL, " ,\t'", &move_token);
				}
			}
		}
		myfile.close();
	}
	else
	{
		cout << "Failed to open file\n";
		exit(1);
	}

}
bool Assembler::isInstruction(char * c)
{
	if (strcmp(c, "JMP") == 0)
	{
		return true;
	}
	else if (strcmp(c, "JMR") == 0)
	{
		return true;
	}
	else if (strcmp(c, "BNZ") == 0)
	{
		return true;
	}
	else if (strcmp(c, "BGT") == 0)
	{
		return true;
	}
	else if (strcmp(c, "BLT") == 0)
	{
		return true;
	}
	else if (strcmp(c, "BRZ") == 0)
	{
		return true;
	}
	else if (strcmp(c, "MOV") == 0)
	{
		return true;
	}
	else if (strcmp(c, "LDA") == 0)
	{
		return true;
	}
	else if (strcmp(c, "STR") == 0)
	{
		return true;
	}
	else if (strcmp(c, "LDR") == 0)
	{
		return true;
	}
	else if (strcmp(c, "STB") == 0)
	{
		return true;
	}
	else if (strcmp(c, "LDB") == 0)
	{
		return true;
	}
	else if (strcmp(c, "ADD") == 0)
	{
		return true;
	}
	else if (strcmp(c, "ADI") == 0)
	{
		return true;
	}
	else if (strcmp(c, "SUB") == 0)
	{
		return true;
	}
	else if (strcmp(c, "MUL") == 0)
	{
		return true;
	}
	else if (strcmp(c, "DIV") == 0)
	{
		return true;
	}
	else if (strcmp(c, "AND") == 0)
	{
		return true;
	}
	else if (strcmp(c, "OR") == 0)
	{
		return true;
	}
	else if (strcmp(c, "CMP") == 0)
	{
		return true;
	}
	else if (strcmp(c, "TRP") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool Assembler::isInstruction(char * c, string s) 
{
	char *myp, *move_token, *mystr;
	mystr = new char[s.length() + 1];
	mystr[s.size()] = 0;
	memcpy(mystr, s.data(), s.size());
	const string str = string(c);
	myp = strtok_s(mystr, " ,\t", &move_token);

	if (strcmp(c, "JMP") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format JMP Label ";
			exit(1);
			return false;
		}
		if (isLabel(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL) { return true; }
			else
			{
				return true;
			}
		}
		else
			cout << "*Wrong format* must be in format JMP Label ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "JMR") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format JMR Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				return true;
			}
			else
			{
				cout << "*Wrong format* must be in format JMR Register ";
				
				exit(1);
				return false;
			}
		}
		else
			cout << "*Wrong format* must be in format JMR Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "BNZ") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format BNZ Register,Label ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format BNZ Register,Label ";
				
				exit(1);
				return false;
			}
			if (isLabel(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format BNZ Register,Label ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format BNZ Register,Label ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "BGT") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format BGT Register,Label ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format BGT Register,Label ";
				
				exit(1);
				return false;
			}
			if (isLabel(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format BGT Register,Label ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format BGT Register,Label ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "BLT") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format BLT Register,Label ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format BLT Register,Label ";
				
				exit(1);
				return false;
			}
			if (isLabel(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format BLT Register,Label ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format BLT Register,Label ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "BRZ") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format BRZ Register,Label ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format BRZ Register,Label ";
				
				exit(1);
				return false;
			}
			if (isLabel(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format BRZ Register,Label ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format BRZ Register,Label ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "MOV") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format MOV Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format MOV Register,Register ";
				
				exit(1);
				return false;
			}
			if (isRegister(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format MOV Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format MOV Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "LDA") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format LDA Register,Label ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format LDA Register,Label ";
				
				exit(1);
				return false;
			}
			if (isLabel(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format LDA Register,Label ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format LDA Register,Label ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "STR") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
				
				exit(1);
				return false;
			}
			if (isLabel(myp) || isRegister(myp))
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "LDR") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
				
				exit(1);
				return false;
			}
			if (isLabel(myp) || isRegister(myp))
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "STB") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
				
				exit(1);
				return false;
			}
			if (isLabel(myp) || isRegister(myp))
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "LDB") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
				
				exit(1);
				return false;
			}
			if (isLabel(myp) || isRegister(myp))
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format STR Register,Label or Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "ADD") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format ADD Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format ADD Register,Register ";
				
				exit(1);
				return false;
			}
			if (isRegister(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format ADD Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format ADD Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "ADI") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format ADI Register,IMM ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format ADI Register,IMM ";
				
				exit(1);
				return false;
			}
			if (atoi(myp) || *myp == '0')
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format ADI Register,IMM ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format ADI Register,IMM ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "SUB") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format SUB Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format SUB Register,Register ";
				
				exit(1);
				return false;
			}
			if (isRegister(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format SUB Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format SUB Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "MUL") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format MUL Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format MUL Register,Register ";
				
				exit(1);
				return false;
			}
			if (isRegister(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format MUL Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format MUL Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "DIV") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format DIV Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format DIV Register,Register ";
				
				exit(1);
				return false;
			}
			if (isRegister(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format DIV Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format DIV Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "AND") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format AND Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format AND Register,Register ";
				
				exit(1);
				return false;
			}
			if (isRegister(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format AND Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format AND Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "OR") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format OR Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format OR Register,Register ";
				
				exit(1);
				return false;
			}
			if (isRegister(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format OR Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format OR Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "CMP") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format CMP Register,Register ";
			
			exit(1);
			return false;
		}
		if (isRegister(myp) == true)
		{
			myp = strtok_s(NULL, " ,\t", &move_token);
			if (myp == NULL)
			{
				cout << "*Wrong format* must be in format CMP Register,Register ";
				
				exit(1);
				return false;
			}
			if (isRegister(myp) == true)
			{
				return true;
			}
			else
				cout << "*Wrong format* must be in format CMP Register,Register ";
			
			exit(1);
			return false;
		}
		else
			cout << "*Wrong format* must be in format CMP Register,Register ";
		
		exit(1);
		return false;
	}
	else if (strcmp(c, "TRP") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL)
		{
			cout << "*Wrong format* must be in format TRP IMM ";
			
			exit(1);
			return false;
		}
		if (atoi(myp) || *myp == '0')
		{
			return true;
		}
		else
			cout << "*Wrong format* must be in format TRP IMM ";
		
		exit(1);
		return false;
	}
	else
	{
		return false;
	}
}

bool Assembler::isDirective(char * c)
{
	if (strcmp(c, ".INT") == 0)
	{
		return true;
	}
	else if (strcmp(c, ".ALN") == 0)
	{
		return true;
	}
	else if (strcmp(c, ".BYT") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool Assembler::isDirective(char * c, string s)
{
	char *myp, *move_token, *mystr;
	mystr = new char[s.length() + 1];
	mystr[s.size()] = 0;
	memcpy(mystr, s.data(), s.size());
	myp = strtok_s(mystr, " ,\t", &move_token);

	if (myp == NULL) { return true; }
	if (strcmp(myp, ".INT") == 0)
	{
		myp = strtok_s(NULL, " ,\t", &move_token);
		if (myp == NULL) { return false; }
		if (atoi(myp) || *myp == '0')
		{
			return true;
		}
		else
			return false;
	}
	else if (strcmp(myp, ".ALN") == 0)
	{
		if (myp == NULL) { return true; }
		else return false;
	}
	else if (strcmp(myp, ".BYT") == 0)
	{
		myp = strtok_s(NULL, ",t'", &move_token);
		if (myp == NULL) { return false; }
		if (isalpha(*myp) && myp[1] == '\0' || *myp == 32 || (*myp == 92 && myp[1] == 'N') || (*myp == 47 && myp[1] == 'N'))
		{
			return true;
		}
		else
			return false;
	}
	else
	{
		return false;
	}
}

bool Assembler::isRegister(char * c)
{
	if (strcmp(c, "R0") == 0)
	{
		return true;
	}
	else if (strcmp(c, "R1") == 0)
	{
		return true;
	}
	else if (strcmp(c, "R2") == 0)
	{
		return true;
	}
	else if (strcmp(c, "R3") == 0)
	{
		return true;
	}
	else if (strcmp(c, "R4") == 0)
	{
		return true;
	}
	else if (strcmp(c, "R5") == 0)
	{
		return true;
	}
	else if (strcmp(c, "R6") == 0)
	{
		return true;
	}
	else if (strcmp(c, "R7") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Assembler::isLabel(char * c)
{
	if (isDirective(c) == true || isInstruction(c) == true || isRegister(c) == true)
		return false;
	else
		return true;
}
bool Assembler::isLabel(char * c, string s)
{
	if (isDirective(c) || isRegister(c) || isInstruction(c))
	{
		return false;
	}
	char *myp, *move_token, *mystr;
	mystr = new char[s.length() + 1];
	mystr[s.size()] = 0;
	memcpy(mystr, s.data(), s.size());
	const string str = string(c);
	myp = strtok_s(mystr, " ,\t", &move_token);
	string passRemain = string(move_token);
	myp = strtok_s(NULL, " ,\t", &move_token);


	if (isDirective(myp, passRemain) || isInstruction(myp, passRemain) || myp == NULL)
	{
		if (pcStart == 0 && isInstruction(myp, passRemain))
		{
			pcStart = index;
		}
		if (symbol_table.find(str) == symbol_table.end())
		{
			symbol_table[str] = index;
			return true;
		}
		else
		{
			cout << "*Duplicate label* ";
			return false;
		}
	}
	else
	{
		return false;
	}

}

int Assembler::indexValue(char* c)
{
	if (c == NULL)
	{
		return 0;
	}
	else if (strcmp(c, ".BYT") == 0)
	{
		return 1;
	}
	else if (strcmp(c, ".INT") == 0)
	{
		return 4;
	}
	else if (strcmp(c, "JMP") == 0 || strcmp(c, "JMR") == 0 || strcmp(c, "TRP") == 0)
	{
		return 8;
	}
	else
		return 12;

}

int Assembler:: opCodeValue(char* c, string s)
{
	char *myp, *move_token, *mystr;
	mystr = new char[s.length() + 1];
	mystr[s.size()] = 0;
	memcpy(mystr, s.data(), s.size());
	myp = strtok_s(mystr, " ,\t", &move_token);

	if (strcmp(c, "JMP") == 0)
	{
		return 1;
	}
	else if (strcmp(c, "JMR") == 0)
	{
		return 2;
	}
	else if (strcmp(c, "BNZ") == 0)
	{
		return 3;
	}
	else if (strcmp(c, "BGT") == 0)
	{
		return 4;
	}
	else if (strcmp(c, "BLT") == 0)
	{
		return 5;
	}
	else if (strcmp(c, "BRZ") == 0)
	{
		return 6;
	}
	else if (strcmp(c, "MOV") == 0)
	{
		return 7;
	}
	else if (strcmp(c, "LDA") == 0)
	{
		return 8;
	}
	else if (strcmp(c, "STR") == 0)
	{
		myp = strtok_s(NULL, " ,\t'", &move_token);
		if (isRegister(move_token))
		{
			return 22;
		}
		else
		return 9;
	}
	else if (strcmp(c, "LDR") == 0)
	{
		myp = strtok_s(NULL, " ,\t'", &move_token);
		if (isRegister(move_token))
		{
			return 23;
		}
		else
		return 10;
	}
	else if (strcmp(c, "STB") == 0)
	{
		myp = strtok_s(NULL, " ,\t'", &move_token);
		if (isRegister(move_token))
		{
			return 24;
		}
		else
		return 11;
	}
	else if (strcmp(c, "LDB") == 0)
	{
		myp = strtok_s(NULL, " ,\t'", &move_token);
		if (isRegister(move_token))
		{
			return 25;
		}
		else
		return 12;
	}
	else if (strcmp(c, "ADD") == 0)
	{
		return 13;
	}
	else if (strcmp(c, "ADI") == 0)
	{
		return 14;
	}
	else if (strcmp(c, "SUB") == 0)
	{
		return 15;
	}
	else if (strcmp(c, "MUL") == 0)
	{
		return 16;
	}
	else if (strcmp(c, "DIV") == 0)
	{
		return 17;
	}
	else if (strcmp(c, "AND") == 0)
	{
		return 18;
	}
	else if (strcmp(c, "OR") == 0)
	{
		return 19;
	}
	else if (strcmp(c, "CMP") == 0)
	{
		return 20;
	}
	else if (strcmp(c, "TRP") == 0)
	{
		return 21;
	}
	else if (strcmp(c, "R0") == 0)
	{
		return 0;
	}
	else if (strcmp(c, "R1") == 0)
	{
		return 1;
	}
	else if (strcmp(c, "R2") == 0)
	{
		return 2;
	}
	else if (strcmp(c, "R3") == 0)
	{
		return 3;
	}
	else if (strcmp(c, "R4") == 0)
	{
		return 4;
	}
	else if (strcmp(c, "R5") == 0)
	{
		return 5;
	}
	else if (strcmp(c, "R6") == 0)
	{
		return 6;
	}
	else if (strcmp(c, "R7") == 0)
	{
		return 7;
	}
	else {
		return 99;
	}
}