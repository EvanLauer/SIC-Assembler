#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SYMBOLS 50

int main(int argc, char *argv[])
{

	//test
	//printf("%d arguments from command-line\n", argc);
	int i;
	//int j;
	for(i=0; i<argc; i++)
	{
		//j=i+1;
		//printf("Argument %d is %s\n", j, argv[i]);
	}



	if(argc !=2)
	{
	//printf("Error: Missing SIC Assembly File Name\n");
	//printf("USAGE: project1 <filename> where filename is SIC assembly file\n");
	exit(0);
	}

	//If we get to here, user has typed a commmand line argument that is the name
	//of a file we need to open and attempt to do pass 1 on
	
	FILE *fIN;
	fIN = fopen(argv[1], "r");

	//Check to make sure file exists
	if(fIN == NULL){
	printf("Error opening file\n");
	exit(1);
	}

	//Variables and Arrays
	char line[256];
	int linecounter = 0;
	int tokencounter = 0;
	int arraycounter = 0;
	int dupcounter = 0;
	int found = 0;
	int address = 0;
	int addressold = 0;
	int dircounter = 0;
	int STARTflag = 0;
	int commentflag = 0;
	int wordcheck = 0;
	int lettercheck = 0;
	char *hexvals[18] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "X", "\'"};
	char *instructions1[] = {"FIX", "FLOAT", "HIO", "NORM", "SIO", "TIO"};
	char *instructions2[] = {"ADDR", "CLEAR", "COMPR", "DIVR", "MULR", "RMO", "SHIFTL", "SHIFTR", "SUBR", "SVC", "TIXR"};
	char *instructions3[] = {"ADDF", "ADDR", "AND", "COMP", "COMPF", "DIV", "DIVF", "J", "JEQ", "JGT", "JLT", "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL", "LDS", "LDT", "LDX", "LPS", "MUL", "MULF", "OR", "RD", "RSUB", "RESW", "SSK", "STA", "STB", "STCH", "STF", "STI", "STL", "STS", "STSW", "STT", "STX", "SUB", "SUBF", "SUBR", "TD", "TIX", "WD", "WORD", };
	char *symbols[MAX_SYMBOLS] = {NULL};
	char *assdir[] = {"START", "END", "BYTE", "WORD", "RESB", "RESW", "EQU", "ORG", "BASE", "NOBASE", "LTORG"};
	int addresses[MAX_SYMBOLS] = {0};

	while(fgets(line, 256, fIN) !=NULL)
	{
		if(address > 32768)
		{
			printf("ERROR: Program Does not fit in SIC Memory\r\n");
			return 0;
		}
		
		commentflag = 0;
		linecounter++;

		if(line[0]== 35)
		{
			//printf("Line %d is a comment. SKIPPING\n", linecounter );
			commentflag = 1;
		}

		if(line[0] == 10)
		{
			printf("ERROR: Line cannot be blank.\n");
			return(0);
		}

		tokencounter=0;
		
		/*Tokenize the line, and figure out, based upon what is on the line, what we should do*/
		for(char *token = strtok(line, "\t"); token!= NULL; token = strtok(NULL, "\t"))
		{
			//skip comments
			if(token[0]== '#')
			{
				break;
			}
			
			//deal with the extra token issue
			if(isspace(*token))
			{
				break;
			}
			else
			{
				tokencounter++;
			}
			
			//get starting address
			if(strcmp(token, "START")==0)
			{
				STARTflag = 1;
				token = strtok(NULL, "\t\n"); //moves to next token
				if(token != NULL)
				{
					tokencounter++;
					address = (int)strtol(token, NULL, 16);
					addressold = (int)strtol(token, NULL, 16);
					
					//test case 5
					if(address == 32768)
					{
						printf("ERROR: SIC Program Starts at Hex %02X. No room left in SIC memory.\r\n", address);
						return 0;
					}
				}
			}

			//handle incrementing with buffer instruction
			if(strcmp(token, "BUFFER")==0)
			{
				token = strtok(NULL, "\t\n"); //moves to next token
				token = strtok(NULL, "\t\n"); //moves to next token
				tokencounter++;
				tokencounter++;
				if(token != NULL)
				{
					address += (int)strtol(token, NULL, 10);
				}
			}
			
			//fix a seg fault
			if(token != NULL)
			{
				//handle incrementing with byte instruction
				if(strcmp(token, "BYTE") == 0)
				{
					token = strtok(NULL, "\t\n"); //moves to next token
					tokencounter++;
					
					
					int size = 0;
					size = strlen(token);
					int sizeloop = size;
					size -=3; //accounts for excess chars
					
					if(token != NULL)
					{	
					
					
						if(token[0]=='C')
						{
							//printf("Current Address: %02X\n", address);
							address+=size;
							//printf("Current Address: %02X\n", address);
						}
						
						else if (token[0] == 'X')
						{
							if(token != NULL) //yes I know Ive checked for NULL 3 times, but it works and I dont want to touch it lol
							{
								for (int i = 0; i < sizeloop; i++)
								{
									char currentchar = token[i];
									for (int j = 0; j < 18; j++)
									{

										char testchar = *hexvals[j];
										//printf("Test %c compared to Current %c\n", testchar, currentchar);
										if (currentchar == testchar)
										{
											found = 1;
											//printf("Found, moving to next test char\n");
											break;
										}
										else
										{
											found = 0;
											//printf("Not found, setting found to 0\n");
										}
										
									}

									if (found != 1)
									{
										printf("ERROR: SIC Program Has Invalid Hex Constant\r\n");
										return 0;
									}
								}
								
								size=(size/2);
								address+=size;
							}	

						}
						
						else
						{
							//can put error message here in future if needed
						}
					}
					
				}
			}

			if(token != NULL)
			{
				if(strcmp(token, "WORD") == 0)
				{
					token = strtok(NULL, "\t\n"); //moves to next token
					if(token != NULL)
					{
						tokencounter++;
						wordcheck = (int)strtol(token, NULL, 16);
						
						//printf("Value of wordcheck: %u\r\n", (unsigned int)wordcheck); more debug print
						if((unsigned int)wordcheck > 16777215)
						{
							printf("ERROR: WORD COntant Exceeds 24 bit limitation\r\n");
							return 0;
						}
						else
						{
							address += 3;
							continue;
						}
					}
				}
			}

			//handle all other instructions
			int loopcounter1;
			for(loopcounter1 = 0; loopcounter1 < 6; loopcounter1++)
			{
				if(token != NULL)
				{
					if(strcmp(token, instructions1[loopcounter1]) == 0)
					{
						address += 1;
					}
				}
			}
			
			int loopcounter2;
			for(loopcounter2 = 0; loopcounter2 < 11; loopcounter2++)
			{
				if(token != NULL)
				{
					if(strcmp(token, instructions2[loopcounter2]) == 0)
					{
						address += 2;
					}
				}
			}
			
			int loopcounter3;
			for(loopcounter3 = 0; loopcounter3 < 46; loopcounter3++)
			{
				if(token != NULL)
				{
					if(strcmp(token, instructions3[loopcounter3]) == 0)
					{
						address += 3;
					}
				}
			}
			
			if (tokencounter == 3)
			{
				token = strtok(line, " ");

				int size = 0;
				size = strlen(token);
				int sizeloop = size;
				for(lettercheck = 0; lettercheck < sizeloop; lettercheck++)
				{
					if(isalpha(token[lettercheck]))
					{
						//printf("test isalpha\n"); //just another debug line
					}
					else
					{
						printf("ERROR: Program Has Invalid Symbol Name\r\n");
						return 0;
					}
				}
				
				for(dupcounter = 0; dupcounter < arraycounter; dupcounter++)
				{	
					if(strcmp(token, symbols[dupcounter]) == 0)
					{
						printf("ERROR: SIC Program Has %s defined twice (DUPLICATE SYMBOL)\r\n", token);
						return 0;
					}
				}
					
				for(dircounter = 0; dircounter < 11; dircounter++)
				{
					if(strcmp(token, assdir[dircounter]) == 0)
					{
						printf("ERROR: SIC Program Has Symbol Name that is an assembler Directive %s\r\n", token);
						return 0;
					}
				}
					
				symbols[arraycounter] = strdup(token); // Store a copy of the token
				if (symbols[arraycounter] == NULL)
				{
					// handle allocation failure
					printf("Memory allocation failed.\n");
					return(0);
				}
				
				addresses[arraycounter] = addressold;
					
				//printf("%s \t%02X\r\n", symbols[arraycounter], addresses[arraycounter]);
				addressold=address;
				arraycounter++;
					
			}	
		}
		
		//printf("Token counter: %d\n", tokencounter);
		if(STARTflag == 1)
		{
			if(commentflag == 0)
			{
				if(tokencounter == 0)
				{
					printf("ERROR: Has blank lines - violates specification.\r\n");
					return 0;
				}	
			}

		}

		//some hacky crap I had to do to get the output correct because man did I botched how I did this lol
		addressold=address;
	}
	fclose(fIN);

	int printval = 0;
	for(printval = 0; printval < arraycounter; printval++)
	{
		printf("%s \t%02X\r\n", symbols[printval], addresses[printval]);
	}

}
