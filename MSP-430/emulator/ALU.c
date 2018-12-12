/*
 *	ALU.c
 *	Purpose:
 *		This source file emulates what happens in the ALU. it is passed a specific 
 *		instruction to execute from decoder.c. it then executes the instruction 
 *		and sets the SR depending on the outcome of the instruction.	
 *	Author:	Nick Stnawood
 *	Date:	June 23 - created source file
 *					- initialized instruction pointers
 *			June 25 - added update_SR()
 *					- put all jump instructions into one function
 *			July 11 - changed update_SR() from if statements to setting SR 
 *					  depending on ALU_reg directly 
 *			July 12 - changed DADD from adding seperate nibbles to adding as if 
 *					  the values given were decimal numbers instead of hex
 **********************************************************************************/
 
 #include "ALU.h"
 #include "decoder.h"
 #include "debugger.h"
 #include "CPU.h"
 
 #include <stdlib.h>
 #include <stdio.h>
 
 /*table of function pointers to all type 1 instructions*/
 void (*t1instptr[MAX_T1])(enum reg_file op) = 
 {RRC, SWPB, RRA, SXT, PUSH, CALL, RETI};
 
 /*table of function pointers to all type 2 instructions*/
 void (*t2instptr[MAX_T2])(enum reg_file src , enum reg_file dst) = 
 {MOV, ADD, ADDC, SUBC, SUB, CMP, DADD, BIT, BIC, BIS, XOR, AND}; 
								
 /*function pointer set by the decoder. points to the instruction to be executed*/
 void (*ALU_t1)(enum reg_file op) = NULL;
 void (*ALU_t2)(enum reg_file src, enum reg_file dst) = NULL;
 
 /*
  * reg set by each ALU instruction, destination register is set to this value
  * after an instruction has taken place
  */
 reg ALU_reg = 0;
 
 /*strings of the instruction names for ease of printing while using the debugger*/
 char *instt1_name[7] = {"RRC" , "SWPB" , "RRA" , "SXT" , "PUSH" , "CALL" , "RETI"};
 char *instt2_name[12] = {"MOV" ,"ADD" ,"ADDC","SUBC","SUB","CMP",
						  "DADD","BIT" ,"BIC" ,"BIS" ,"XOR","AND"};
 char *instt3_name[8] ={"JNE/JNZ","JEQ/JZ","JNC/JLO","JC/JHS","JN","JGE","JL","JMP"};
 
 
/*================================================================================*
 *	Updates the status Register with new values depending on the ALU instruction  *
 *	that was just performed. 													  *
 *	also sets the array values to true or false for the jump conditions			  * 
 *================================================================================*/
 void update_SR(reg src , reg dst , reg res)
 { 	
 	if(BW_mode == WORD)
 	{
 		reg_tbl[SR]  = CARRY_W(res);
		reg_tbl[SR] |= NEG_W(res);
		reg_tbl[SR] |= ZERO(res);
		reg_tbl[SR] |= OVF(SIGN_W(src),  SIGN_W(dst), SIGN_W(res));
	}
	else /*byte arithmetic*/
	{
 		reg_tbl[SR]  = CARRY_B(res);
		reg_tbl[SR] |= NEG_B(res);
		reg_tbl[SR] |= ZERO(res);
		reg_tbl[SR] |= OVF(SIGN_B(src) , SIGN_B(dst) , SIGN_B(res));	
	}
	/*update JUMP instruction conditions*/
	condition_tbl[JNE] 	= !ISZERO;
 	condition_tbl[JEQ] 	= ISZERO;
 	condition_tbl[JNC] 	= !ISCARRY;
 	condition_tbl[JC] 	= ISCARRY;
	condition_tbl[JN] 	= ISNEG;
	condition_tbl[JGE] 	= (ISNEG == ISOVF);
	condition_tbl[JL] 	= (ISNEG != ISOVF);
	condition_tbl[JUMP]	= 1; 
 } 
 
 
/*================================================================================*
 *	RRC - roll right, CARRY into MSB , LSB into CARRY 							  *
 *		- set SR, set dst														  *
 * SWPB - swap bytes of operand 											      *
 *		- set dst																  *
 *	RRA - roll right, store LSB in CARRY 										  *
 *		- set SR , set dst													  	  *
 *	SXT - extends the signed bit of the LOWBYTE to all bits of the HIGHBYTE		  *
 *		- set SR , set dst														  *
 * PUSH - decrements SP, loads opernad into mem[SP]								  *
 *		- does not change Status Register or dst								  *
 * CALL - PUSH() Program Counter, set PC to operand								  *
 *		- special case for INDEXED , RELATIVE and ABSOLUTE addressing modes		  *
 *		- does not change Status Register or dst								  *
 * RETI - pops SR from stack, then  pops PC from stack							  *
 *		- special case for updating SR											  *
 *		- no need to pass RETI an operand, except to give it the same number of   *
 *		  function paramters as the rest of the type 1 instructions				  *
 *================================================================================*/	
 void RRC( enum reg_file op){
 	ALU_reg = (reg_tbl[op] >> 1);
 	ALU_reg += (BW_mode == WORD) ? (ISCARRY << 16) : (ISCARRY <<  8);	
	
	/*passing 0 and 1 as operand arguments for update_SR enures OVF == 0 */
	update_SR(0 , 1 , ALU_reg);
	
	if(reg_tbl[op] & 1){
		reg_tbl[SR] |= 1;		//set carry if LSB was 1
		condition_tbl[JNC] = false;
		condition_tbl[JC] = true;	
	}
	
 	set_result(op);
 }
 
 
 void SWPB(enum reg_file op){
 	ALU_reg = HIGHBYTE(reg_tbl[op]) + (LOWBYTE(reg_tbl[op]) << 8);
 	set_result(op);
 }
 
 
 void RRA(enum reg_file op){
 	if(BW_mode == WORD)
 		ALU_reg = (reg_tbl[op] >> 1) | (reg_tbl[op]&0x8000);
 	else
 		ALU_reg = (reg_tbl[op] >> 1) | (reg_tbl[op]&0x80);	
 	
 	/*passing 0 and 1 as operand arguments for update_SR enures OVF == 0 */
	update_SR(0 , 1 , ALU_reg);
 	
 	/*special case for updating carry bit*/
 	if(reg_tbl[op] & 1){
		reg_tbl[SR] |= 1;		//set carry if LSB was 1
		condition_tbl[JNC] = false;
		condition_tbl[JC] = true;
	}
 	set_result(op);
 }
 
 
 void SXT(enum reg_file op){
 	ALU_reg = (reg_tbl[op] & 0x80) 
		? reg_tbl[op]|0xFF00 
		: reg_tbl[op]&0x00FF;
		
 	/*passing 0 and 1 as operand arguments for update_SR enures OVF == 0 */ 	
	update_SR(0 , 1 , ALU_reg);
 	set_result(op);
 }
 
 
 void PUSH(enum reg_file op){
 	reg_tbl[SP] -=2;							/* (SP-2)-> SP		*/
 	CTRL.BW = WORD;								/* WORD  -> CTRL  	*/
 	CTRL.RW = WRITE;							/* WRITE -> CTRL  	*/
 	MAB(SP);									/*  SP   -> MAR   	*/
 	MDB(reg_tbl[op] , &MDR);					/*	op   -> MDR		*/
	BUS();										/* MDR	 ->mem[MAR] */							
 } 


 void CALL(enum reg_file op){
 	PUSH(PC);									/*	PC  -> mem[SP]	*/
 												/*					*/
 	// addressing modes indexed , relative 		/*					*/
	// and absoute require special treatment	/*					*/
 	if(dst_mode >= IDX && dst_mode <= ABS)		/*INDX , REL , ABS	*/
 		MDB(reg_tbl[TA] , &reg_tbl[PC]);		/*  EA  -> PC		*/
	 else										/*  DIR,INDR, IMM	*/
	 	MDB(reg_tbl[op] , &reg_tbl[PC]);		/*  op  ->	PC		*/
 }


 void RETI(enum reg_file op){
 	CTRL.BW = WORD;								/* WORD   -> CTRL  	*/
 	CTRL.RW = READ;								/* READ   -> CTRL  	*/
 	MAB(SP);									/*  SP    -> MAR   	*/
 	BUS();										/*mem[MAR]-> MDR	*/
 	MDB(MDR , &reg_tbl[SR]);					/*	MDR   -> SR 	*/
 	reg_tbl[SP] +=2;							/* (SP+2) -> SP		*/
 												/*					*/
 	MAB(SP);									/*  SP    -> MAR   	*/
 	BUS();										/*mem[MAR]-> MDR	*/
 	MDB(MDR , &reg_tbl[PC]);					/*	MDR   -> PC 	*/
 	reg_tbl[SP] +=2;							/* (SP+2) -> SP		*/
 } 
 
 
/*================================================================================*
 * JMP - Only one jumps instruction as they all do the same thing. by passing it  *
 *		 a particular conditon from the conidition_tbl[] it will only jump when	  *
 *		 the correct condtion is met											  *  
 *================================================================================*/  
 void JMP(enum reg_file offset , int cond){
 	/*if cond is met, adds offset to PC*/
 	if(cond)
 	{
 		/*signed bit is set*/
	 	if(reg_tbl[offset] & 0x0200){
	 		reg_tbl[offset] = reg_tbl[PC] - (~reg_tbl[offset] & 0x01FF)*2 - 2;
		}		
		else
			reg_tbl[offset] = (reg_tbl[PC] + reg_tbl[offset]*2);
	 	MDB(reg_tbl[offset] , &reg_tbl[PC]);					
	}
 }
 
 
/*================================================================================*
 *	MOV - sets destiaion equal to source										  *
 *		- sets dst, does not set SR												  *
 *	ADD - adds two values 														  *
 *		- sets SR , sets dst								 					  *
 *	ADDC- adds two values with carry 											  *
 *		- sets SR , sets dst													  *
 *	SUBC- subtracts source and CARRY from destination							  * 
 *		- sets SR , sets dst													  *
 *	SUB - subtracts source from destination										  *
 *		- sets SR , sets dst													  *
 *	CMP - subtracts source from destinations									  *
 *		- sets SR , does not set dst											  *
 *	DADD- deciamlly adds source and destination									  *
 *		- sets SR , sets dst													  *
 *	BIT	- bitwise OR two values													  * 
 *		- set SR																  *
 *	BIC - clear bits in destination												  *
 *		- set dst																  *
 *	BIS - Bitwise OR two values 												  *
 *		- set dst															 	  *
 *	XOR - Bitwise Exclusive OR two values 										  *
 *		- set SR , set dst														  *
 *	AND - Bitwise AND two values 												  *
 *		- set SR , set dst														  *
 *================================================================================*/  
 void MOV(enum reg_file src , enum reg_file dst){	
 	ALU_reg = reg_tbl[src];
 	set_result(dst);
 }
 
 
 void ADD(enum reg_file src , enum reg_file dst){
 	ALU_reg = reg_tbl[src] + reg_tbl[dst];
 	update_SR(reg_tbl[src] , reg_tbl[dst] , ALU_reg);
 	set_result(dst);
 	
 }
 
 
 void ADDC(enum reg_file src , enum reg_file dst){
 	ALU_reg = reg_tbl[src] + reg_tbl[dst] + ISCARRY;
 	update_SR(reg_tbl[src] , reg_tbl[dst] , ALU_reg);
 	set_result(dst);
 }
 
 
 void SUBC(enum reg_file src , enum reg_file dst){
 	ALU_reg = reg_tbl[dst] - reg_tbl[src] - 1 + ISCARRY;
 	/*pass the ones compliment of source to ensure OVF is set properly*/
	update_SR(~reg_tbl[src] , reg_tbl[dst] , ALU_reg);
 	set_result(dst);
 }
 
 
 void SUB(enum reg_file src , enum reg_file dst){
 	ALU_reg = reg_tbl[dst] - reg_tbl[src];
 	
 	/*pass the ones compliment of source to ensure OVF is set properly*/
 	update_SR(~reg_tbl[src] , reg_tbl[dst] , ALU_reg);
 	set_result(dst);
 }
 
 
 void CMP(enum reg_file src , enum reg_file dst){
 	ALU_reg = reg_tbl[dst] - reg_tbl[src];
 	/*pass the ones compliment of source to ensure OVF is set properly*/
 	update_SR(~reg_tbl[src] , reg_tbl[dst] , ALU_reg);
 }
 
 
 void DADD(enum reg_file src , enum reg_file dst){
 	int tmpsrc , tmpdst;		//keep original values of src & dst for update_SR()
	char srcstr[5];
	char dststr[5];
	char *srcend;
	char *dstend;
	sprintf(srcstr , "%x" , LOWWORD(reg_tbl[src]));
	sprintf(dststr , "%x" , LOWWORD(reg_tbl[dst]));
	
	tmpsrc = strtol(srcstr , &srcend , 10);
	tmpdst = strtol(dststr , &dstend , 10);
	
	/*set error interrupt and return if values are incorrect */
	if(*srcend != '\0' || *dstend != '\0'){
		error();
		return;
	}
	if(tmpsrc > MAX_DEC_W || tmpdst > MAX_DEC_W){
		error();
		return;
	}
	if(BW_mode == BYTE){
		if(tmpdst > MAX_DEC_B || tmpsrc > MAX_DEC_B){
			error();
			return;	
		}
	}
	ALU_reg = tmpsrc + tmpdst;	
	sprintf(dststr , "%d" , LOWWORD(ALU_reg));
	ALU_reg = strtol(dststr , NULL , 16);
		
 	update_SR(reg_tbl[src] , reg_tbl[dst] , ALU_reg);
 	set_result(dst);
 }
 
 
 
 void BIT(enum reg_file src , enum reg_file dst){
	ALU_reg = reg_tbl[src] & reg_tbl[dst];
 	update_SR(reg_tbl[src] , reg_tbl[dst] , ALU_reg);
 }
 
 
 void BIC(enum reg_file src , enum reg_file dst){
	ALU_reg = reg_tbl[dst] & ~reg_tbl[src];
 	set_result(dst);
 }
 
  
 void BIS(enum reg_file src , enum reg_file dst){
	ALU_reg = reg_tbl[src] | reg_tbl[dst];
 	set_result(dst);
 }
 
  
 void XOR(enum reg_file src , enum reg_file dst){
	ALU_reg = reg_tbl[src] ^ reg_tbl[dst];
 	update_SR(reg_tbl[src] , reg_tbl[dst] , ALU_reg);
 	set_result(dst);
 }
 
 
 void AND(enum reg_file src , enum reg_file dst){
 	ALU_reg = reg_tbl[src] & reg_tbl[dst];
 	update_SR(reg_tbl[src] , reg_tbl[dst] , ALU_reg);
 	set_result(dst);
 }
 
 
 
 
 
 
 
 
 
 
 
