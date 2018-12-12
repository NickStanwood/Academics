/*
 *	debugger.h
 *	Purpose:
 *		This header is used to help debug the programmer's assembly code (lord knows
 		it helped me debug the emulator code as well)
 *	Definitions:
 *			All debugger options and information can be found in DEBUGGER.txt
 *	Author:	Nick Stanwood
 *	Date:	June 28 - created header file
 **********************************************************************************/
 
 #ifndef DEBUG_H
 #define DEBUG_H
 
 #include <stdio.h>
 
 enum {LOAD_F , LOAD_S , INIT_PC , R_ALL , R_SPEC , 
        MEM_DUMP , BRKPT , WATCH , AUTO , SREC , TERM};
 
 /*the options that the programmer has chosen are set to true*/
 extern char debug_opt[];
 
 /*used for a breakpoint in the assembly code*/
 extern int brkpt;
 
 /*number of instructions to be executed before control goes to the debugger*/
 extern long inst_cnt;
 
 /*initialization function for the debugger*/
 extern void debug_init(void);
  
 /*run time function for the debuggger*/
 extern void scan_debug(void); 
 
 
 #endif
