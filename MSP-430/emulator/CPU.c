/*
 *	CPU.c
 *	Purpose:
 *		this source file emulates the inner workings of the CPU. it is meant to
 *		work exaclty as the CPU of the MSP-430 works. It has a bus() function
 *		for accessing memory as well as a MAB() for setting the memory destination
 *		and a MDB() for setting a register to a specific value.
 *
 *		This source file is also where code is fetched() and ececuted(). after 
 *		an instruction is executed on the ALU the result is set in the set_result()
 *		function. 	
 *	Author:	Nick Stanwood
 *	Date:	June 21 - created cpu functions and reg declerations
 *			June 23 - moved all ALU functions to seperate header
 *			June 24 - wrote BUS() and fetch() code
 *			June 25 - wrote fetch_op() and execute() code
 *			July 10 - changed how clock was increased into tables  
 **********************************************************************************/
 #include "CPU.h"
 #include "memory.h"
 #include "reg_table.h"
 #include "ALU.h"
 #include "decoder.h"
 #include "debugger.h"
 #include "device.h"
 
 #include <stdlib.h>
 #include <stdio.h>
 

 /*table used for increasing the clock with type 1 instructions*/
					   /*| Rn | x(Rn) | ADDR | &ADDR | @Rn | @Rn+ | #N | */
int clk_cyclesT1[7][7] = {{ 1 ,   4   ,  4   ,   4   ,   3  ,   3  , 0  }, /* RRC  */
						 {  1 ,   4   ,  4   ,   4   ,   3  ,   3  , 0  }, /* SWPB */
 						 {  1 ,   4   ,  4   ,   4   ,   3  ,   3  , 0  }, /* RRA  */
 						 {  1 ,   4   ,  4   ,   4   ,   3  ,   3  , 0  }, /* SXT  */
 						 {  3 ,   5   ,  5   ,   5   ,   4  ,   5  , 4  }, /* PUSH */
 						 {  4 ,   5   ,  5   ,   5   ,   4  ,   5  , 5  }, /* CALL */
 						 {  5 ,   5   ,  5   ,   5   ,   5  ,   5  ,  5 }  /* RETI */
 						};
 						
/*table used for increasing the clock with type 3 isntructions*/
					   /*			Source addressign mode				 */
				       /*| Rn | x(Rn) | ADDR | &ADDR | @Rn | @Rn+ | #N | */
int clk_cyclesT2[5][7] ={{  1 ,  3   ,  3   ,   3   ,  2  ,   2  ,   2   }, /* Rm  */
 						 {  4 ,  6   ,  6   ,   6   ,  5  ,   5  ,   5   }, /*x(Rm)*/
 						 {  4 ,  6   ,  6   ,   6   ,  5  ,   5  ,   5   }, /*ADDR */
 						 {  4 ,  6   ,  6   ,   6   ,  5  ,   5  ,   5   }, /*&ADDR*/
 					   	 {  2 ,  3   ,  3   ,   3   ,  2  ,   2  ,   3   }, /* PC  */
						 }; 


 /*================================================================================*
  *	 accesses mem[]																   *
  *		  -	read or writes depending on the CTRL register (CTRL.RW)				   *
  *		  -	The control register also determiens if it is a byte or word (CTRL.BW) *
  *		  - No return value. The data is put into the MDR						   *
  *================================================================================*/
 void BUS(void)
 {
	if(CTRL.RW == READ)
	{
		MDR = mem[MAR];
  		if(CTRL.BW == WORD)
  			MDR += (mem[MAR+1] << 8);
	}
	else /*write*/
	{
		mem[MAR] = LOWBYTE(MDR);
		if(CTRL.BW == WORD)
	  		mem[MAR+1] = HIGHBYTE(MDR);	
	}
 }
 
 
 /*================================================================================*
  *	Takes a register from the register table and puts it into the Memroy Address   *
  *  Register																	   *
  *================================================================================*/	 
 void MAB(enum reg_file src){
	MAR = reg_tbl[src];	  							/*   src -> MAR		*/
 }
 
 
 /*================================================================================*
  *	A two way data bus:															   *
  *		sets destination register to src register's value					       *
  *================================================================================*/
 void MDB(reg src , reg *dst){
  	*dst = src;										/*   src -> dst		*/
 }
 
 
 /*================================================================================*
  * fetches the next instruction from mem[] and increments PC					   *
  * also used when fetching an operand requires mem[PC] to be fetched			   * 	   																	       
  *================================================================================*/
 void fetch(void)
 { 
 	MAB(PC);										/*    PC    -> MAR	*/
	CTRL.BW = WORD;									/*   WORD   -> CTRL	*/
	CTRL.RW = READ;									/*   READ   -> CTRL	*/
	BUS();											/* mem[MAR] -> MDR 	*/
	reg_tbl[PC] +=2;								/* (2 + PC) -> PC	*/
 }
 
 
 /*================================================================================*
  * executes the current instruction found by the decoder.						   *
  * checking whether or not the destination addressing mode is register direct 	   *
  * must be done so the ALU can set the result in the proper register			   *
  *================================================================================*/
 void execute(void)
 {
 	if(ALU_t1 != NULL)		/*single operand instruction to be executed*/
 	{
		 if(dst_mode != DIR)
 		{
 			fetch_op(TD2 , dst_mode , OP_T1);		/*OP_T1 = reg in IR*/
			ALU_t1(TD2);		 
 		}
		else{	/*dst == DIR*/
 			ALU_t1(OP_T1);			/*take operand reg directly from reg table*/
 		}
		
	}
	else if(ALU_t2 != NULL) 						/*double operand instruction*/
	{		
		fetch_op(TD1 , src_mode , SRC_T2); 			/*SRC_T2 = source reg in IR */ 
		if(dst_mode != DIR){
			fetch_op(TD2 , dst_mode , DST_T2);		/*DST_T2 = dest reg in IR   */
			ALU_t2(TD1 , TD2);
		}
		else{  /*dst == DIR*/
			ALU_t2(TD1 , DST_T2);			/*take dst reg directly from reg table*/
		}		
		
	}
	else if(VAL_INST)	/*jump instruction to be executed*/
	{
		MDB(OFFSET_T3 , &reg_tbl[TD1]);				/* IR offest -> TD  */
		JMP(TD1 , condition_tbl[COND_T3]);
	}
	/* 
	 * if is was not Type 1 or Type 2 or a jump it was invalid and has already been
	 * handled accordingly
	 */
	
	inc_clk();
 }
 
 
 /*================================================================================*
  * fetches the operand from memory and puts it into TMP reg (either TD1 or TD2)   *
  * NOTE:	Rn is the register number found in the instruction regiter			   *
  *			for Relative and Immediate this is the Program Counter 				   *
  *			for Absolute this is the Status Register							   *
  *================================================================================*/
 void fetch_op(enum reg_file TMP , enum addr_mode op_mode , enum reg_file Rn)
 {
 	switch(op_mode)
 	{												/*__________________*/
 		case DIR:									/* REGISTER DIRECT  */
 			MDB(reg_tbl[Rn] , &reg_tbl[TMP]);		/*    Rn -> TMP		*/
 			break;									/*__________________*/
		case IDX:									/*					*/
		case REL:									/*RELATIVE & INDEXED*/
			fetch();								/* mem[PC] -> MDR	*/
													/*		PC += 2		*/
			MDB(MDR , &reg_tbl[TA]);				/*   MDR   -> TA	*/
			reg_tbl[TA] += reg_tbl[Rn];				/*  TA+Rn  -> TA	*/
			MAB(TA);								/*   TA    -> MAR	*/
			CTRL.BW = BW_mode;						/*   B/W   -> CTRL  */
			CTRL.RW = READ;							/*  READ   -> CTRL	*/
			BUS();									/* mem[TA] -> MDR	*/
			MDB(MDR , &reg_tbl[TMP]);				/*   MDR   -> TMP	*/
			break;									/*__________________*/
		case ABS:									/*	  ABSOLUTE		*/
			fetch();								/* mem[PC] -> MDR	*/
													/*		PC += 2		*/
			MDB(MDR , &reg_tbl[TA]);				/*   MDR   -> TA	*/
			MAB(TA);								/*   TA    -> MAR	*/
			CTRL.BW = BW_mode;						/*   B/W   -> CTRL	*/
			CTRL.RW = READ;							/*  READ   -> CTRL	*/
			BUS();									/* mem[TA] -> MDR	*/
			MDB(MDR , &reg_tbl[TMP]);				/*   MDR   -> TMP	*/
			break;									/*__________________*/
		case IND:									/* 	  INDIRECT		*/
			MAB(Rn);								/*   Rn    -> MAR	*/
			CTRL.BW = BW_mode;						/*   B/W   -> CTRL	*/
			CTRL.RW = READ;							/*  READ   -> CTRL	*/
			BUS();									/* mem[Rn] -> MDR	*/
			MDB(MDR , &reg_tbl[TMP]);				/*   MDR   -> TMP	*/
			break;									/*__________________*/
		case INC:									/*INDIRECT INCREMENT*/
		case IMM:									/*  AND IMMEDIATE	*/
			MAB(Rn);								/*   Rn    -> MAR	*/
			CTRL.BW = BW_mode;						/*   B/W   -> CTRL	*/
			CTRL.RW = READ;							/*  READ   -> CTRL	*/
			BUS();									/* mem[Rn] -> MDR	*/
			MDB(MDR , &reg_tbl[TMP]);				/*   MDR   -> TMP	*/
			reg_tbl[Rn]++;							/* 1 + Rn  -> Rn 	*/
			if(op_mode == IMM || BW_mode == WORD)	/*					*/
				reg_tbl[Rn]++;						/* 1 + Rn  -> Rn	*/
			break;									/*__________________*/
	}		
 }
 
 
 /*================================================================================*
  * Called from any ALU instruction that requires the dst to be updated			   *
  *	returns early if the destination addressing mode is Register Direct			   *
  * if it is not register direct than the data to be put into mem[] will be in TD2 *
  * and the Effective Address will be in TA										   *
  *================================================================================*/ 
 void set_result(enum reg_file dst)
 { /*sets the result of the ALU instruction into the dst*/
 	MDB(LOWWORD(ALU_reg) , &reg_tbl[dst]);			/*	ALU -> dst reg	*/
 	/*return if register direct*/					/*__________________*/
	if(dst != TD2)									/*	  ABSOLUTE		*/	
 		return;										/*	  INDEXED		*/
													/*   & RELATIVE		*/
	MDB(reg_tbl[TD2] , &MDR);						/*  TD2  -> MDR		*/
	MAB(TA);										/*  TA   -> MAR		*/
	CTRL.BW = BW_mode;								/*  B/W  -> CTRL	*/
	CTRL.RW = WRITE;								/* WRITE -> CTRL	*/
	BUS();											/*  MDR  -> mem[TA]	*/
													/*__________________*/
	return;
 }
 
 
  /*================================================================================*
  * Called from the decoder if the instruction that was found did not match any    *
  * opcodes. by defualt this function will set the C-off bit in the Status Register*
  * but this can be easily changed by the programmer by setting the ERR_vect to    *
  * a different address: 														   *
  *			ORG $FFFE															   *
  *			WORD $2000															   *
  *																				   *
  *			ORG $2000															   *
  *			NEW_ISR 															   *
  *			  ...																   *
  *================================================================================*/ 
 void error(void)
 { /*called when an error is found, sets PC to err_vect found in mem[0xFFFE]*/
 	PUSH(PC);										/*    PC   -> --SP	*/
	PUSH(SR);										/*    SR   -> --SP	*/
	CIE;											/*  ~GIE   -> SR	*/
	MDB(ERR_VECT , &reg_tbl[TA]);					/* err_vect-> TA	*/
	MAB(TA);										/*    TA   -> MAR	*/
	CTRL.BW = WORD;									/*   WORD  -> CTRL	*/
	CTRL.RW = READ;									/*   READ  -> CTRL	*/
	BUS();											/*mem[vect]-> MDR	*/
	MDB(MDR , &reg_tbl[PC]);						/*	 MDR   -> PC	*/
	MDB(reg_tbl[ZERO] , &inst_reg);					/*	   0   -> IR	*/
 }
 
 
 /*================================================================================*
  * increases the clock based on the clock cylce tables and checks if any devices  *
  * have reached their next Time of Interrupt, if they have it sets that device's  *
  * device port Status Register to the next value in that device's input file
  *================================================================================*/
 void inc_clk(void)
 { 
 	int i = 0;
 	char *tmp = malloc(7);
 	int tmpval;
 	
 	switch(INST_TYPE)
 	{
 		case TYPE1:
		 	sys_clock += clk_cyclesT1[INST_T1][dst_mode];
			break;
		case TYPE3: 
			sys_clock += 2;
			break;
		default:
			/*
			 * different clock cycle values if the destination is the PC
			 * so go to row 4 of the table instead of the regualar DIR location
			 */
			if(DST_T2 == PC && dst_mode == DIR)
				sys_clock += clk_cyclesT2[4][src_mode]; 
			else
				sys_clock += clk_cyclesT2[dst_mode][src_mode];
				break;	
	}
 	for(i = 0 ; i < num_dev ; i++)
 	{
 		if(dev_ToI[i] <= sys_clock){		
 			fscanf(DEV_f[i] , "%s" , tmp);			  /*     get next SR value    */
 			tmpval = strtol(tmp,NULL,0);
 			mem[dev_port[i]] = LOWBYTE(tmpval);		  /* put new SR into dev_port */
 			mem[dev_port[i] + 1] = HIGHBYTE(tmpval);	
		}
	}
 }
 
 

 
 
 
 
 
 
 
 
 
 
 
 
