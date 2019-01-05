/*
 *	instruction.c
 * 	Author		: Nick Stanwood
***********************************************************************************/
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>

#include "instruction.h"
#include "record.h"

/*byte instructions already have their B/W bit set*/
const struct instruction INST_LIST[61]	=  
		{{"ADD"   , 0x5000 , 2} , {"ADD.B" , 0x5040 , 2} , {"ADD.W" , 0x5000 , 2} ,  
		 {"ADDC"  , 0x6000 , 2} , {"ADDC.B", 0x6040 , 2} , {"ADDC.W", 0x6000 , 2} , 
    	 {"AND"   , 0xF000 , 2} , {"AND.B" , 0xF040 , 2} , {"AND.W" , 0xF000 , 2} ,
		 {"BIC"   , 0xC000 , 2} , {"BIC.B" , 0xC040 , 2} , {"BIC.B" , 0xC000 , 2} , 
		 {"BIS"   , 0xD000 , 2} , {"BIS.B" , 0xD040 , 2} , {"BIS.W" , 0xD000 , 2} , 
    	 {"BIT"   , 0xB000 , 2} , {"BIT.B" , 0xB040 , 2} , {"BIT.W" , 0xB000 , 2} , 
		 {"CALL"  , 0x1280 , 1} , {"CMP"   , 0x9000 , 2} , {"CMP.B" , 0x9040 , 2} ,
		 {"CMP.W" , 0x9000 , 2} , {"DADD"  , 0xA000 , 2} , {"DADD.B", 0xA040 , 2} , 
	     {"DADD.W", 0xA000 , 2} , {"JC"    , 0x2C00 , 3} , {"JEQ"   , 0x2400 , 3} , 
	     {"JGE"   , 0x3400 , 3} , {"JHS"   , 0x2C00 , 3} , {"JL"    , 0x3800 , 3} , 
		 {"JLO"   , 0x2800 , 3} , {"JMP"   , 0x3C00 , 3} , {"JN"    , 0x3000 , 3} , 
		 {"JNC"   , 0x2800 , 3} , {"JNE"   , 0x2000 , 3} , {"JNZ"   , 0x2000 , 3} , 
		 {"JZ"    , 0x2400 , 3} , {"MOV"   , 0x4000 , 2} , {"MOV.B" , 0x4040 , 2} , 
		 {"MOV.W" , 0x4000 , 2} , {"PUSH"  , 0x1200 , 1} , {"PUSH.B", 0x1240 , 1} , 
		 {"PUSH.W", 0x1200 , 1} , {"RETI"  , 0x1300 , 0} , {"RRA"   , 0x1100 , 1} , 
		 {"RRA.B" , 0x1140 , 1} , {"RRA.W" , 0x1100 , 1} , {"RRC"   , 0x1000 , 1} , 
		 {"RRC.B" , 0x1040 , 1} , {"RRC.W" , 0x1000 , 1} , {"SUB"   , 0x8000 , 2} , 
		 {"SUB.B" , 0x8040 , 2} , {"SUB.W" , 0x8000 , 2} , {"SUBC"  , 0x7000 , 2} , 
		 {"SUBC.B", 0x7040 , 2} , {"SUBC.W", 0x7000 , 2} , {"SWPB"  , 0x1080 , 1} , 
		 {"SXT"   , 0x1180 , 1} , {"XOR"   , 0xE000 , 2} , {"XOR.B" , 0xE040 , 2} ,
		 {"XOR.W" , 0xE000 , 2}							
  	    };


/*=================================================================================*
 *
 *=================================================================================*/
void instcpy(int a)
{
	strcpy(assembly.inst.name , INST_LIST[a].name);
	assembly.inst.type = INST_LIST[a].type;
	assembly.inst.addr.ADDR_whole = INST_LIST[a].addr.ADDR_whole;
	
	return;
}


/*=================================================================================*
 *
 *=================================================================================*/
int search_InstList(char tok[])
{	
	int bottom = 0;
	int top = 60;
	int pos;
	int found = -1;
	
	while((bottom <= top) && (found == -1))
	{
		pos = (bottom + top)/2;
		if(strcmpi(INST_LIST[pos].name , tok) == 0)
			found = pos;
		else if(strcmpi(INST_LIST[pos].name , tok) > 0)
			top = pos -1;
		else if(strcmpi(INST_LIST[pos].name , tok) < 0)
			bottom = pos + 1;
	}
	return found;
}


