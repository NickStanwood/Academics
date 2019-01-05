/*	record.h
 *	purpose and definitions:
 *			- record structure
 *			- parses records 
 *			- sets the address for emittion
 *			- emits record adresses
 *	global variables:	
 *			LC_old , LC_cur , assembly , second_pass
 *	Author:	Nick Stanwood
 **********************************************************************************/
 
#ifndef RECORD_H
#define RECORD_H

#include <ctype.h>

#include "instruction.h"
#include "operand.h"

#define SR 2
#define PC 0

extern int As_val[];
extern int Ad_val[];

struct record{
	char line[256];
	char dir[6]; //largest directive is 5 char long
	struct instruction inst;  
	char label[33]; //maximum size of a label is 32
	struct op src;
	struct op dst;
	bool error;
};

extern unsigned int LC_cur;
extern unsigned int LC_old;
bool second_pass;
bool error_flag;

/*this struct gets its data set for the current record being parsed*/
extern struct record assembly;

/* 	print_record()
 *	prints a parsed record to the .LIS file with the addressing modes 
 *	of each operand and the LC 
 *	for debugging purposes
 */
extern void print_record(void);

/*	parse_record()
 *	dissects a record into its constituent parts
 *	as it goes through it checks for possible errors such as:
 *		- more than one label on a line
 *		- too many operands 
 */
void parse_record(void);

/*	clear_record()
 *	sets all int values in the global struct record assembly to 0
 *	and all strings to empty strings
 */
void clear_record(void);

/*	set_adddress()
 *	used on the second pass only, creates the address to be 
 *	emitted with the Location Counter for each instruction
 */
void set_address(void);




#endif
