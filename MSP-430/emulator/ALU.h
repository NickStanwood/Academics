/*
 *	ALU.h
 *	Purpose:
 *		This header file emulates what happens in the ALU. it is passed a specific 
 *		instruction to execute from decoder.c. it then executes the instruction 
 *		and sets the SR depending on the outcome of the instruction.
 *	Global Variable Declerations:
 *		 MACROS			- various macros for determining whether or not certain
 						  status register bits should be set
 *		*t1instptr[]()	- an array of function pointers to all type 1 instructions
 *		*t2instptr[]()	- an array of function pointers to all type 2 instructions
 *		 ALU_reg		- this register is for the outcome of the executed ALU 
 *						  instruction. it is subsequently put onto the MDR to 
 *						  be put into the destination register if need be.
 *				
 *	Author:	Nick Stanwood
 *	Date:	June 23 - created ALU function declerations
 *			June 25 - replaced all conditional jump inst with single function
 **********************************************************************************/
 
 #ifndef ALU_H
 #define ALU_H
 
 #include "reg_table.h"
 
 #define MAX_T1 7
 #define MAX_T2 12
 #define MAX_T3 8 
 
 #define MAX_DEC_B 99			//for DADD instruction
 #define MAX_DEC_W 9999			//for DADD instruction
 
 /* array of function pointers to all type 1 instructions */
 extern void (*t1instptr[])(enum reg_file op);
 
 /* array of function pointers to all type 2 instructions */
 extern void (*t2instptr[])(enum reg_file src, enum reg_file dst); 
 
 /*the resulting value of an ALU instruction*/
 extern reg ALU_reg;
 
 /*
  *	function pointers for type 1 and 2 instructions
  * one of these is the function called in execute(), depending on the type of inst
  */
 extern void (*ALU_t1)(enum reg_file op);
 extern void (*ALU_t2)(enum reg_file src, enum reg_file dst);
 
 
  /*strings for easy printing when using the debugger*/
 extern char *instt1_name[];
 extern char *instt2_name[];
 extern char *instt3_name[];
 
 
 /*	update_SR()
  *	updates all status register bits depending on what happened in the previous 
  *	arithmetic/logic instruction.
  *	Not called when status bits are not affected (i.e SWPB and JMP etc.)
  */
 extern void update_SR(reg src , reg dst , reg res);

 
 /*	type 1 functions
  * these functions are pointed to by (*t1instptr[])()
  * and the specific instruction to be executed is pointed to by (*ALU_t1)()
  */
 void RRC( enum reg_file op);
 void SWPB(enum reg_file op);
 void RRA( enum reg_file op); 
 void SXT( enum reg_file op);
 void PUSH(enum reg_file op); 
 void CALL(enum reg_file op); 
 void RETI(enum reg_file op); 
 
 
 /* JMP()
  *	used for all jump instructions , if the condition is met then PC =  2*offset
  * conditions are set with the use of the condition_tbl[] in decoder.h
  */
 void JMP(enum reg_file offset , int cond);
 
 
 /*	type 2 functions
  * these functions are pointed to by (*t2instptr[])()
  * and the specific instruction to be executed is pointed to by (*ALU_t2)()
  */ 
 void MOV( enum reg_file src , enum reg_file dst);
 void ADD( enum reg_file src , enum reg_file dst);
 void ADDC(enum reg_file src , enum reg_file dst);
 void SUBC(enum reg_file src , enum reg_file dst);
 void SUB( enum reg_file src , enum reg_file dst);
 void CMP( enum reg_file src , enum reg_file dst);
 void DADD(enum reg_file src , enum reg_file dst);
 void BIT( enum reg_file src , enum reg_file dst);
 void BIC( enum reg_file src , enum reg_file dst);
 void BIS( enum reg_file src , enum reg_file dst);
 void XOR( enum reg_file src , enum reg_file dst);
 void AND( enum reg_file src , enum reg_file dst);
 
 #endif
