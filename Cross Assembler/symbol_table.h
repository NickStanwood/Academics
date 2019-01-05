/*	symbol_table.h
 *	purpose and definitions:
 *			- add symbols 
 *			- get symbols
 *			- check if valid table at end of first pass
 *	global variables:
 * 			- sym_tbl
 *	Author: Nick Stanwood
 			with reference to Dr. Larry Hughes 
			 ECED 3403 symtbl.h
***********************************************************************************/

#ifndef SYM_TBL_H
#define SYM_TBL_H

#include "operand.h"

/*	an additional type of IMM is used for an easy way to get values of # operands
 *	no need to worry about two of the same label as $CAFE would not be able to be 
 *	put into the symbol table at any other point in the program because of the $ char
 */
enum sym_types{ REG , LABEL , IMM , UNKNOWN};

struct sym_entry
{
	char symbol[32];
	int value;
	enum sym_types type;
	struct sym_entry *next;
};
/*defining the link list for the symbol table*/
extern struct sym_entry *sym_tbl;

/*	sym_tbl_int()
 *	initializes the symbol table by putting in every register 
 *	and each registers alternate function if there is one
*/
extern void sym_tbl_init(void);


/*	add_entry()
 *	this function adds an entry onto the end of the symbol table
 *	NOTE: does not check if the symbol is already in the table 
 */
extern void add_entry(char* sym , int val , enum sym_types typ);

/*	get_entry()
 *	checks the symbol table for the symbol name passed to it
 *	returns NULL if the symbol is not in the table
 *	retunrs the sym_entry if there is one
 */
extern struct sym_entry* get_entry(char* sym);

/*	print_sym_tbl()
 *	prints out the symbol table to the .LIS file
 *	for debugging purposes only
 */
extern void print_sym_tbl(void);

/*	check_sym_tbl()
 *	called after the first pass to check for any UNKOWN symbol types
 *	returns true if sym_tbl is valid
 */
bool check_sym_tbl(void);

#endif
