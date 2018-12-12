/*
 *	memory.c
 *	Purpose:
 *		This source file emulates what happens in memory. the loader is included in
 *		this file. The values in mem[] or only accessed though the BUS() function
 *		(and the debugger)
 *				
 *	Author:	Nick Stanwood
 *	Date: 	June 15 - created source file and wrote load_Srec()
 *			June 23 - wrote init_mem()
 *			July 10 - added HCF instruction as the reset ISR
 **********************************************************************************/

#include "memory.h"
#include "reg_table.h"
#include "debugger.h"
#include "CPU.h"

#include <stdio.h> 
#include <string.h>
 
 
/* 
 *static array of chars meant to emulate the BYTE addressable memory of the MSP-430
 */
unsigned char mem[MAX_MEM] = {0};

/*decleration of the input Srecord file*/
FILE *Srec_f = NULL;




/*=================================================================================*
 *	Initializes memory with a HCF instruction in the error interrupt vector		   *
 *	if the programmer wants something else as their reset/error ISR they can 	   *
 *	easily change it in their code												   *
 *=================================================================================*/
void init_mem(void){ 

	mem[ISR_HCF]   = LOWBYTE(HCF_opcode);	/*   ISR_HCF --> | BIS #16,SR	| */
	mem[ISR_HCF+1] = HIGHBYTE(HCF_opcode);	/*	(0xFFBC)     |_____#16______| */
	mem[ISR_HCF+2] = 16;					/*			     |		.		| */
	mem[ISR_HCF+3] = 0;						/*			     |		.		| */
											/*			     |______._______| */
	mem[ERR_VECT] = LOWBYTE(ISR_HCF); 		/*   ERR_vect -->|  ISR_HCF 	| */	
	mem[ERR_VECT+1] = HIGHBYTE(ISR_HCF);	/*	 (0xFFFE)	 |______________| */
}







/*=================================================================================*
 * 	takes an Srecord string an gets the length, address 						   *
 *	it then loads all the data into memory and checks the Checksum				   *
 *	all Srecords that are not type S1 or S9 are ignored							   *
 *	Return value: return true if valid Srecord									   *
 *				  return false if invalid										   *
 *=================================================================================*/
bool load_Srec(char Srec[])
{
	char *Srecptr;						/*ptr that runs along the Srecord	 	*/
	char byte[3];						/*2 hex values and the NUL character 	*/
	char word[5];						/*4 hex values and the NUL character 	*/
	
	int len  = 0 ,						/*length of Srec 					 	*/
		sum  = 0 , 						/*checksum of Srec					 	*/
		addr = 0 ,						/*addr stored in the Srec			 	*/
		loc  = 0 ;						/*location of mem[] that is loaded next	*/	
	
	bool val_Srec = true;
	
	byte[2] = '\0';
	word[4] = '\0';
	Srecptr = Srec;
	Srecptr++;		//get past 'S'
	
	if(debug_opt[SREC])
		printf("loading Srec...\n");
	
	/*check if it is an S1 record*/
	if(*Srecptr == '1'){
		Srecptr++;					//get past '1'
		
		/*get length of S1 record*/
		strncpy(byte , Srecptr , SIZE_BYTE);		 						
		len = strtol(byte , NULL , BASE_16);
		Srecptr += SIZE_BYTE;		 
		sum += len;										
		
		/*get address */
		strncpy(word , Srecptr , SIZE_WORD);		
		addr = strtol(word , NULL , BASE_16);		
		Srecptr += SIZE_WORD;
		sum += LOWBYTE(addr);
		sum += HIGHBYTE(addr);	
		
		/*for the length of the Srec, load values*/
		for(loc = addr ; (loc - addr) < (len - 3) ; loc++){
			/*-3 in condition to account for checksum and address bits*/		
			/*get byte of data*/
			strncpy(byte , Srecptr , SIZE_BYTE);
			mem[loc] = strtol(byte , NULL , BASE_16);	
			Srecptr += SIZE_BYTE;
			sum += mem[loc];
		}
		
		/*seperate for() loop for debug option to avoid checking every time*/
		if(debug_opt[SREC]){
			for(loc = addr ; (loc - addr) < (len -3) ; loc++){
				printf("        |____|\n");
				printf("%04x -->| %02x |\n" , loc , mem[loc]);
			}
		}
		
		/*get checksum*/
		strncpy(byte , Srecptr , SIZE_BYTE);
		sum += strtol(byte , NULL , BASE_16);
		if(LOWBYTE(sum) != 0xFF)
			val_Srec = false;
		
		if(debug_opt[SREC])
			printf("sum = %02x\n" , LOWBYTE(sum));
	}
	else if(*Srecptr == '9'){
		Srecptr++;					//get past '9'
		
		/*get length of S9*/
		strncpy(byte , Srecptr , SIZE_BYTE);
		Srecptr += SIZE_BYTE;
		len = strtol(byte , NULL , BASE_16);
		sum += len;
		
		/*all S9 records are 3 bytes in size*/
		if(len != 3)
			val_Srec = false;
		
		/*get starting address for PC*/
		strncpy(word , Srecptr , SIZE_WORD);
		if(!debug_opt[INIT_PC]){
			reg_tbl[PC] = strtol(word , NULL , BASE_16);
			sum += reg_tbl[PC];
		}
		else
			sum += strtol(word , NULL , BASE_16);
			
		Srecptr += SIZE_WORD;
		if(debug_opt[SREC])
			printf("PC = %04x\n" , reg_tbl[PC]);
		
		/*get checksum*/
		strncpy(byte , Srecptr , SIZE_BYTE);
		sum += strtol(byte , NULL , BASE_16);
		if(LOWBYTE(sum) != 0xFF)
			val_Srec = false;
			
		if(debug_opt[SREC])	
			printf("sum = %02x\n" , LOWBYTE(sum));
	}	
	return val_Srec;
}


