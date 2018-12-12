/*
 *	decoder.h
 *	Purpose:
 *		This header file emulates the instruction decoder. It systematically 
 *		determines which function needs to be executed and sets the ALU instptr
 *		accordingly
 *	Definitions:
 *		MACROS			- various macros for defining parts of the instruction 
 *						  register
 *		inst_reg		- the register in which the opcode from the fetch() is stored
 *		condition_tbl[] - a table of conditions for the jump instructions	
 *		ADDR_TBL[][]	- a table of all registers and addr modes to easily 
 *						  deterimine what the addressing mode of an instruction is		  
 *	Author:	Nick Stanwood
 *	Date:	June 21 - created header file
 **********************************************************************************/
 
 #ifndef DECODER_H
 #define DECODER_H
 
 #include "reg_table.h"
 
 /*macros for finding type and the specific inst of that type*/
 #define INST_TYPE 	(inst_reg >> 13)
 #define INST_T1 	((inst_reg >> 07) & 0x07)
 #define INST_T2 	(((inst_reg >> 12) & 0x0F) - 4) //minus 4 for proper array loc
 #define COND_T3	((inst_reg >> 10) & 0x07)
 
 /*macros for addressing modes and register values in the instruction register*/
 #define OP_T1 		(inst_reg & 0x000F)
 #define OFFSET_T3	(inst_reg & 0x03FF)
 #define SRC_T2		((inst_reg >> 8) & 0x000F)
 #define DST_T2		(inst_reg & 0x000F)		//same as OP_T1 - used for readability
 #define As_mode	((inst_reg >> 4) & 0x0003)
 #define Ad_mode	((inst_reg >> 7) & 0x0001)
 #define BW_mode	((inst_reg >> 6) & 0x0001)
 
 /* if there was an error the instruction register is cleared */
 #define VAL_INST	(inst_reg)
 
 #define MAX_REG 65535 
 
 enum inst_type {TYPE1 , TYPE3 , DEFAULT};
 
 enum addr_mode{DIR , IDX , REL , ABS , IND , INC , IMM};
 enum jmp_types{JNE , JEQ , JNC , JC , JN , JGE , JL , JUMP};
 
 
 /* used to give integer values for each addressing mode */
 enum addr_mode src_mode;
 enum addr_mode dst_mode;
 
 
 /*a table with values that are either true or false depending on the SR*/
 extern int condition_tbl[];
 
 
 /*instruction register*/
 extern reg inst_reg;
 
 
 /*table for determing the addr mode of source and destination ooperands*/
 extern const enum addr_mode ADDR_TBL[][4];
 
 
 /*	decode()
  *	takes the value the inst_reg and systematically finds the proper instruction
  *	it then sets which instruction should be executed in the ALU
  */
 extern void decode(void);
 
 
 #endif
