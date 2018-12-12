/*
 *	reg_table.h
 *	Purpose:
 *		holds all registers involved with the CPU and defines macros for testing
 *		the Staus Register 
 *	Definitions:
 *		macros for setting, clearing and checking specific SR bits
 *		typedef reg - just an unsigned int. wanted a more flavourful name
 *		typedef bool - boolean (true or false)
 *
 *		reg_tbl[]-	table of specific and general register as well as constants
 *		MDR 	 - 	Memory Data Register, could be a value fetched from mem[] 
 *		 			or a value to be written to mem[]
 *		MAR		 -	Memory Address Register, used by BUS() to determine what value
 *					in mem[] needs to be read/written
 *		CTRL	 -	determines whether or not BUS() reads or writes to mem[] and 
 *					whether or not it is a byte or word
 *		clock	 -	system clock cycles 	
 *	Author:	Nick Stanwood
 *	Date:	June 21 - created reg_table[] decleration and created macros
 *			June 23 - changed macros for status register to only work with SR
 *			June 26 - moved all global registers (MDR,MAR,etc.) to register file
 *					  seemed more approprate to have the registers here
 *					  and just the functions in CPU.h
 **********************************************************************************/
 
 #ifndef REGTBL_H
 #define REGTBL_H
 
 #define HIGHBYTE(x) ((x)>>8 & 0xFF)
 #define LOWBYTE(x)  ((x) & 0xFF)
 #define LOWWORD(x)	 ((x) & 0xFFFF)
 #define LLNIB(x) 	 ((x) & 0xF)
 #define LHNIB(x) 	 ((x)>>4 & 0xF)
 #define HLNIB(x)	 ((x)>>8 & 0xF)
 #define HHNIB(x)	 ((x)>>12 & 0xF)
 #define SIGN_B(x)	 ((x)>>7 & 0x01)
 #define SIGN_W(x)	 ((x)>>15 & 0x01)
 
 /*		 ________________________________________________	*/
 /*		|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|__|	*/
 /*     |___HHNIB___|___HLNIB___|___LHNIB___|___LLNIB___|	*/
 /*		|_______HIGHBYTE________|________LOWBYTE________|	*/
 /*		|_____________________LOWWORD___________________|	*/
 /*		|SW|____________________|SB|____________________|	*/
 
/*BYTE and WORD for CTRL register and for B/W operations*/
 #define BYTE	1
 #define WORD	0
 
 /*manually setting status register bits*/
 #define SETCARRY	(reg_tbl[SR] = reg_tbl[SR] | 0x0001) 
 #define SETZERO	(reg_tbl[SR] = reg_tbl[SR] | 0x0002)
 #define SETNEG		(reg_tbl[SR] = reg_tbl[SR] | 0x0004)
 #define GIE 	 	(reg_tbl[SR] = reg_tbl[SR] | 0x0008)
 #define C_OFF	 	(reg_tbl[SR] = reg_tbl[SR] | 0x0010)
 #define SETOVF	 	(reg_tbl[SR] = reg_tbl[SR] | 0x0100)
 
 /*setting Status Register bits with regards to the result of the ALU*/
 #define CARRY_W(x)		(((x) & 0x10000) >> 16)
 #define CARRY_B(x)		(((x) & 0x100) >> 8)
 #define NEG_W(x)		(((x) & 0x8000) >> 13)
 #define NEG_B(x)		(((x) & 0x80) >> 7)
 #define ZERO(x)		(((x) == 0) << 1)
 #define OVF(a, b, c)	(((~a & ~b & c) | (a & b & ~c)) << 8)
 
 /*clearing status register bits*/
 #define CLRCARRY 	(reg_tbl[SR] = reg_tbl[SR] & ~0x0001) 
 #define CLRZERO  	(reg_tbl[SR] = reg_tbl[SR] & ~0x0002)
 #define CLRNEG 	(reg_tbl[SR] = reg_tbl[SR] & ~0x0004)
 #define CIE 	 	(reg_tbl[SR] = reg_tbl[SR] & ~0x0008)
 #define C_ON	 	(reg_tbl[SR] = reg_tbl[SR] & ~0x0010)
 #define CLROVF	 	(reg_tbl[SR] = reg_tbl[SR] & ~0x0100) 

 /*checking status register bits*/
 #define ISCARRY 	(reg_tbl[SR] & 0x0001)
 #define ISZERO		(reg_tbl[SR] & 0x0002)
 #define ISNEG		(reg_tbl[SR] & 0x0004)
 #define ISGIE 	 	(reg_tbl[SR] & 0x0008)
 #define ISC_OFF	(reg_tbl[SR] & 0x0010)
 #define ISOVF	 	(reg_tbl[SR] & 0x0100)
 
 typedef int bool;
 enum { false, true };
 
 /*reg type define to visually differentiate between variables and registers*/
 typedef unsigned int reg;
 
 /*TA - temp address register, TD1 & TD2 - temp data registers*/
 enum reg_file { R0 , R1  , R2  , R3  , R4  , R5  , R6  , R7 , R8 , 
 				R9 , R10 , R11 , R12 , R13 , R14 , R15 , TA , TD1 , TD2 ,
				 NEG_ONE , ZERO , ONE , TWO , FOUR , EIGHT};

/*string names of all registers for easy printing*/				 
 extern char *reg_name[];
 
 /*special registers corresponding to their place in the reg_table[]*/
 enum spec_reg {PC , SP , SR , CG2};
  

 /*a table in which all general purpose and specialty registers are stored*/
 extern reg reg_tbl[];
 
 /*keeping track of clock cycles as the program progresses*/
 extern unsigned long sys_clock;
 
  /*The value currently on the memory data bus*/
 /*MAY NOT BE NEEDED, WE SHALL SEE*/
 extern reg MDB_data;
 
 /*The Memory Data Register*/
 extern reg MDR;
 
 /*the Memory Address Register*/
 extern reg MAR;
 
 /*the control register for the bus.
  * RW for reading or writing 
  *	BW for byte or word
  */
 extern struct control{reg RW; reg BW;}CTRL;
 
 #endif
