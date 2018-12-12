/*
 *	decoder.c
 *	Purpose:
 *		This source file emulates the instruction decoder. It systematically 
 *		determines which function needs to be executed and sets the ALU instptr
 *		accordingly		
 *			
 *	Author:	Nick Stanwood
 *	Date:	June 21 - created c file
 **********************************************************************************/
 
 #include "decoder.h"
 #include "ALU.h"
 
 #include <stdlib.h>
 #include <stdio.h>
 
 reg inst_reg = 0;
 
 enum addr_mode src_mode;
 enum addr_mode dst_mode;
 										/*         As/Ad         */			
 										/*  0     1     2     3  */			
 const enum addr_mode ADDR_TBL[16][4] = {{ DIR , REL , IND , IMM } , /*PC*/
 						  				 { DIR , IDX , IND , INC } , /*SP*/
						  				 { DIR , ABS , IND , INC } , /*SR*/
						  				 { DIR , IDX , IND , INC } , /*R3*/
						  			     { DIR , IDX , IND , INC } , /*R4*/
						  				 { DIR , IDX , IND , INC } , /*R5*/
										 { DIR , IDX , IND , INC } , /*R6*/ /*reg*/
										 { DIR , IDX , IND , INC } , /*R7*/
										 { DIR , IDX , IND , INC } , /*R8*/
										 { DIR , IDX , IND , INC } , /*R9*/
										 { DIR , IDX , IND , INC } , /*R10*/
										 { DIR , IDX , IND , INC } , /*R11*/
										 { DIR , IDX , IND , INC } , /*R12*/
										 { DIR , IDX , IND , INC } , /*R13*/
										 { DIR , IDX , IND , INC } , /*R14*/
										 { DIR , IDX , IND , INC } , /*R15*/
										};
										
 /* used to determine which jump instruction conditions are met	*/
 /* initial values are based off of a zeroed Status Register	*/
 int condition_tbl[8] = { 1 , 0 , 1 , 0 , 0 , 1 , 0 , 1 }; 
 					   /*JNE,JEQ,JNC,JC ,JN ,JGE,JL ,JMP*/
 
 
 
 
 
 
 
 
 
 
/*================================================================================*
 *	Decodes the value in tthe instruction register to give the proper instruction *
 *	for the ALU to execute as well as the proper addressing modes of each operand *
 *	calls the error interrupt vector if the instruction register does not contain *
 *	a valid instruction															  *
 *================================================================================*/
 void decode(void)
 {
	switch(INST_TYPE)
	{
		case TYPE1:			/*single operand instruction*/
			/*make sure bit 12 is set & bits 7:9 correspond to a valid inst */
			if(inst_reg & 0x1000 && INST_T1 != 7){
				ALU_t1 = t1instptr[INST_T1];
				ALU_t2 = NULL;
				dst_mode = ADDR_TBL[OP_T1][As_mode];
			}
			else{ // not valid type 1 instruction
				ALU_t2 = NULL;
				ALU_t1 = NULL;
				error();
			}
			break;
			
		case TYPE3:			/*jump instruction*/
			ALU_t1 = NULL;
			ALU_t2 = NULL;
			break;
			
		default: 			/*double operand instruction*/
			if(INST_T2 >= 0){
				ALU_t2 = t2instptr[INST_T2];
				ALU_t1 = NULL;
				src_mode = ADDR_TBL[SRC_T2][As_mode];
				dst_mode = ADDR_TBL[DST_T2][Ad_mode];		
			}
			else{
				ALU_t1 = NULL;
				ALU_t2 = NULL;
				error();									
			}	
			break;	
	}	 			
 }
 
