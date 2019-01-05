/*	operand.h
 *	purpose and definitions:
 *			- operand structure
 *			- parses operands
 *			- checks addressing modes
 *	global variables:	
 *			- INDEX_REG 
 *	Author:	Nick Stanwood
 **********************************************************************************/

#ifndef OPERAND_H
#define OPERAND_H

#define RANDOM_VAL 42
#define MAX_LBL_LEN

typedef int bool;
enum { false, true };

extern int INDEX_REG_temp;
extern int INDEX_REG_src;
extern int INDEX_REG_dst;
  
/*used for readability while finding addressing mode and setting inst address*/  
enum ADDR_MODE	{REGISTER ,    INDEXED    , RELATIVE  , ABSOLUTE , 
				 INDIRECT , INDIRECT_INCR , IMMEDIATE , BAD_MODE};

struct op{
	char name[32];
	enum ADDR_MODE mode;
};

/*	get_operands()
 *	finds as many operands as needed
 *	if there are not enough operands it sets the record's error flag
 *	if there are valid characters on the record after the last operand 
 *	it sets the error flag
 */
void get_operands(void);

/*	mode_check()
 *	used to make sure each operand is formatted correctly -
 * 	finds the most likely addressing mode and calls the approprate 
 *	ADDR_MODE function listed below	 
 */
void mode_check(struct op *operand);


/*	abs_operand() , imm_operand , alpha_operand() , ind_operand() ,indexed_operand()
 *	these four functions are called in the switch statement in 
 *	operand_check
 *	on the first pass these functions check the viability of the operand 
 *	and add it to the symbol table if it isnt in there already
 *
 *	on the second pass these functions return the ADDR_MODE of the operand , 
 *	they will never add a symbol to the symbol table on the second pass as 
 *	it will have already been added on the forst pass
 *	returns the ADDR_MODE of the operand sent to it
 */
enum ADDR_MODE abs_operand(char *opptr);
enum ADDR_MODE imm_operand(char *opptr);
enum ADDR_MODE alpha_operand(char *opptr);
enum ADDR_MODE ind_operand(char *opptr);
enum ADDR_MODE indexed_operand(char *opptr);

/*	operand_check()
 *	removes any chars from the operand relating to the addressing mode 
 *	NOTE: only used after finding the addressing mode with mode_check()
 *	returns the opernad name
 */
char* operand_check(char *operand);

#endif
