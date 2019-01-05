/*	
 *	symbol_table.c
 * 	Author	: 	Nick Stanwood
***********************************************************************************/
#include "symbol_table.h"
#include "S_rec.h"
#include "record.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


struct sym_entry *sym_tbl = NULL;


extern void sym_tbl_init(void)
{
	add_entry("R0"  , 0  , REG);
	add_entry("PC"  , 0  , REG); 	/*alternate for R0 (Program counter)*/
	add_entry("R1"  , 1  , REG);
	add_entry("SP"  , 1  , REG);	/*alternate for R1 (Stack Pointer)*/
	add_entry("R2"  , 2  , REG);
	add_entry("SR"  , 2  , REG);	/*alternate for R2 (Status Register)*/
	add_entry("CG1" , 2  , REG);	/*alternate for R2 (Constant generator 1)*/
	add_entry("R3"  , 3  , REG);
	add_entry("CG2" , 3  , REG);	/*alternate for R3 (constant generator 2)*/
	add_entry("R4"  , 4  , REG);
	add_entry("R5"  , 5  , REG);
	add_entry("R6"  , 6  , REG);
	add_entry("R7"  , 7  , REG);
	add_entry("R8"  , 8  , REG);
	add_entry("R9"  , 9  , REG);
	add_entry("R10" , 10 , REG);
	add_entry("R11" , 11 , REG);
	add_entry("R12" , 12 , REG);
	add_entry("R13" , 13 , REG);
	add_entry("R14" , 14 , REG);
	add_entry("R15" , 15 , REG);
	
	return;
	
}

/*=================================================================================*
 *
 *=================================================================================*/
extern void add_entry(char* sym , int val , enum sym_types typ)
{
	struct sym_entry *new_entry;
	new_entry = malloc(sizeof(struct sym_entry));
	strcpy(new_entry->symbol , sym);
	new_entry->value = val;
	new_entry->type = typ;
	new_entry->next = sym_tbl;
	sym_tbl = new_entry;
	
	return;
}


/*=================================================================================*
 *
 *=================================================================================*/
extern struct sym_entry* get_entry(char* sym)
{
	struct sym_entry* symptr;
	symptr = sym_tbl;
	while(symptr)
	{
		if(strcmp(symptr->symbol , sym) == 0)
			return symptr;
		else
			symptr = symptr->next;
	}
	
	return NULL;
}


/*=================================================================================*
 *
 *=================================================================================*/
extern void print_sym_tbl(void)
{
	struct sym_entry* ptr;
	ptr = sym_tbl;
	fprintf(LIS_f , "\n\n\tSymbol\t|\tValue\t|\tType\t|\n\n");
	while(ptr)
	{
		fprintf(LIS_f , "\t%s\t|" , ptr->symbol);
		fprintf(LIS_f , "\t%d\t|" , ptr->value);
		fprintf(LIS_f , "\t%d\t|\n"  , ptr->type);
		ptr = ptr->next;	
	}
	fprintf(LIS_f , "\n\n");	
}


/*=================================================================================*
 *
 *=================================================================================*/
bool check_sym_tbl(void)
{
	struct sym_entry* symptr;
	symptr = sym_tbl;
	bool check = true;
	
	while(symptr){
		if(symptr->type == UNKNOWN){
			assembly.error = true;
			fprintf(LIS_f , "\t[ERROR]");
			fprintf(LIS_f , " %s used but not declared\n" , symptr->symbol);
			check = false;
		}
		symptr = symptr->next;
	}
	return check;
}
