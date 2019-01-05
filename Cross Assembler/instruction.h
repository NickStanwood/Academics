/*	instruction.h
 *	purpose and definitions:
 *			- union address
 *			- struct instruction 
 *			- searches inst_list
 * 	global variables: 
 		INST_LIST
 *	Author:	Nick Stanwood
 **********************************************************************************/
 
#ifndef INST_H
#define INST_H

#include <strings.h>

struct type1_addr{unsigned int reg:4 , As:2 , BW:1 , opc:9;};
struct type2_addr{unsigned int dst:4 , As:2 , BW:1 , Ad:1 , src:4 , opc:4;};
struct type3_addr{signed int dst:10 ; unsigned int opc:6;};


/*used for ease of setting addressing depending on which type of instruction it is*/
union address{
	unsigned short		ADDR_whole;
	struct type1_addr 	ADDR1;
	struct type2_addr 	ADDR2;
	struct type3_addr	ADDR3;	
};

/*contains the name , address to emit and the type (single , double , jump)*/
struct instruction{
	char name[7];		//max size of an instruction is 7 including '\0'
	union address addr;
	unsigned int type;
};

/*	NOTE: 
 *	the instruction mov.b ,add.b , addc.b etc.. have 0x0040 added to their 
 *	address in the instruction table to account for the B/W being set
 */
extern const struct instruction INST_LIST[];

/*	instcpy()
 *	copies the inst from the INST_LIST at position 'a' into 
 *	the global record variable assembly.inst
 */									  	   
void instcpy(int a);


/*	search_InstList()
 * takes a string as an argument.
 *	searches through the valid instructions using a binary search. 
 *	if the token matches any of the instructions in the list it sends 
 *	back the position in the array
 *	if it does not match any of the instructions it sends back -1
 */
int search_InstList(char tok[]);

									  	   
#endif
