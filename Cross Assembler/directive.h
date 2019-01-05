/*	directive.h
 *	purpose and definitions
 *			- search directive list
 *			- processes each directive
 *	global variable: 
 			- end_flag , DIR
 *	Author:	Nick Stanwood
 **********************************************************************************/
#ifndef DIR_H
#define DIR_H

#include<string.h>
#include "operand.h"	//for bool type

extern const char*  DIR[];

/*used for readability in switch statements*/
enum dir_names{ALIGN , ASCII , BSS , BYTE , END , EQU , ORG , WORD};

/*set to true when an END directive is found*/
extern bool end_flag;


/*	search_dirList()
 *	searches through the valid directives using a binary search. 
 *	if the token matches any of the directives in the list it sends 
 *	back the position in the array
 *	if it does not match any of the directives it sends back -1
 */
int search_DirList(char tok[]);


/*	process_dir()
 *	called on both the first and second pass
 *	argument is the directive in the DIR[] array that needs to be processed
 *	no return value
 */
void process_dir(int directive);

#endif
