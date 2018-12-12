/*
 *	memory.h
 *	Purpose:
 *		This header file emulates what happens in memory. the loader is included in
 *		this file. The values in mem[] or only accessed though the BUS() function.
 *	Definitions:
 *			mem[]  - a statically defined array meant to emulate the memory of 
 * 					 the MSP-430	
 *			Srec_f - the input Srecord file: to be loaded into mem[]	
 *	Author:	Nick Stanwood
 *	Date:	June 15 - created load_Srec() and init_mem() decleration
 *			June 23 - moved typedef bool to reg_table.h
 *			July 10 - added HCF instruction as the reset ISR
 **********************************************************************************/
#ifndef MEM_H
#define MEM_H

#include "reg_table.h"
#include <stdio.h>

#define SIZE_BYTE 2 
#define SIZE_WORD 4
#define MAX_MEM 65535
#define BASE_16 16				//for strtol() calls 
#define HCF_opcode	0xD032 		//same opcode as BIS #16,SR
#define ISR_HCF 	0xFFBC		//store the reset ISR right above INTR_vect table

extern FILE *Srec_f;
 
extern unsigned char mem[];

/*	load_Srec()
 *	this function loads an srec's data into the address specified
 *	it also sums up all the values of the Srec and uses it with the checksum to 
 *	determine if it is a valid record
 *	returns true if it is a valid Srec
 *	returns false if it is an invalid Srec
 */
bool load_Srec(char Srec[]);


/*	init_mem()
 *	this functions initializes the memory array and puts an HCF instruction into 
 *	the reset interrupt vector. This can be easily changed by the programmer 
 *	in their assembly program
 */
void init_mem(void);


#endif
