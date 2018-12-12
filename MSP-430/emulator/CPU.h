/*
 *	CPU.h
 *	Purpose:
 *		this header file emulates the inner workings of the CPU. it is meant to
 *		work exaclty as the CPU of the MSP-430 works. It has a bus() function
 *		for accessing memory as well as a MAB() for setting the memory destination
 *		and a MDB() for setting a register to a specific value.
 *
 *	Global Variable Declerations:
 *			All global registers moved to reg_table.h
 *	Author:	Nick Stanwood
 *	Date:	June 21 - created cpu functions and reg declerations
 *			June 23 - moved all ALU functions to seperate header
 *			June 25 - wrote fetch(), fetch_op() and execute() 
 **********************************************************************************/
 #ifndef CPU_H
 #define CPU_H
 
 #include "reg_table.h"
 #include "ALU.h"
 #include "decoder.h"
 
 #define READ 	1
 #define WRITE 	0
 
 #define ERR_VECT 0xFFFE 
 
/*table used for increasing the clock with type 1 instructions*/
/*					   [addr mode][inst name] */
 extern int clk_cyclesT1[][7];

/*table used for increasing the clock with type 2 isntructions*/
/*						[dst addr mode][src addr mode]*/ 
 extern int clk_cyclesT2[][7];

 
 /*	MDB()
  *	takes a source register from the register table or the MDR and puts it 
  *	into the destination register
  */
 extern void MDB(reg src , reg *dst);
 
 
 /*	MAB()
  * takes a source register from the register table as its only argument
  *	and puts it into the MAR
  */
 extern void MAB(enum reg_file src);
 
 
 
 
 
 
 
 /*	BUS()
  *	if CTRL == READ
  *		reads the value of mem[] in the location of the value of the MAR
  *	if CTRL == WRITE
  *		sets the value in mem[MAR] to the value of MDB
  *	There is no return value.
  */
 extern void BUS(void);
 
 
 /* fetch()
  *	fetches the next instruction from memory and increments the PC
  */
 extern void fetch(void);
 
 
 /* fetch_op()
  *	fetches the operand from memory and puts it value into TMP register
  *	op_mode is the enumerated value of the addressing mode of the operand
  *	Rn is the register found in the instruction register
  *	No return value, however reg_tbl[TMP] is updated with a new value
  */
 extern void fetch_op(enum reg_file TMP , enum addr_mode op_mode , enum reg_file Rn);
 
/* set_result()
 *	sets the destination of the previous ALU instruction whether it is a 
 *	register or a place in memory.
 *	Not called when instruction does not overwrite destination (i.e. CMP)
 */
 extern void set_result(enum reg_file dst);
 
 /* execute()
  *	executes the current instruction, this involves calling ALU() and 
  *	BUS() if operands are stored in mem[] (i.e. everything except register direct)
  */
 extern void execute(void);
 
 /*	inc_clk()
  *	increments the system clock and checks all devivces to see if the clock 
  *	time matches it's interrupt time
  */
 extern void inc_clk(void);
 
 /* error()
  * if an illegal instruction is found this function is called to handle it
  */
 void error(void);
 
 
 #endif
