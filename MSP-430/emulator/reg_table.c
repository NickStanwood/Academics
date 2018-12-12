/*
 *	reg_table.c
 *	Purpose:
 *		this source file holds all registers involved with the CPU(not IR or ALU_reg) 
 *		it does not contain any functions. 		
 *	Author:	Nick Stanwood
 *	Date:	June 21 - reg_table initialization
 *			June 26 - created reg_name[] for printing registers easily
 *					- moved MDR, MAR, CTRL, and sys_clock from CPU to reg_tbl
 *			
 **********************************************************************************/
 #include "reg_table.h"
 

 /*initialize all registers to 0 except the constant generators*/
 reg reg_tbl[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,1,2,4,8};
 
 
 /* used for printing out register names easily*/
 char *reg_name[25] = {"PC","SP","SR","R3","R4","R5","R6","R7","R8","R9",
 						"R10","R11","R12","R13","R14","R15","TA","TD1","TD2",
						 "NEG_ONE","ZERO","ONE","TWO","FOUR","EIGHT"};

  /*keeping track of clock cycles as the program progresses*/
 unsigned long sys_clock = 0;

 
 /*
  *	Memory Data Register
  *	Has a wide variety of uses. mainly used for setting and reading data in memory
  * using the BUS
  */
 reg MDR = 0;
 
 /*
  * Memory Address Register
  *	used to access a certain location in memory while using the BUS
  */
 reg MAR = 0;
 
 /*	
  * the control register for the bus.
  * RW for reading or writing 
  *	BW for byte or word
  */
 struct control CTRL = {0};
