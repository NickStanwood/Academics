/*
 *	mainline.c
 *	purpose
 *		This source file starts the assembler by asking for an assembly (.asm)
 *		file. it then begins the first pass through the file and checks if there are
 *		any errors on any of the records. if no errors are found it will begin the
 *		second pass and start outputting hex values to the Srecord.   
 *	author :	Nick Stanwood
 **********************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>

#include "S_rec.h"
#include "operand.h"
#include "record.h"
#include "instruction.h"
#include "directive.h"
#include "symbol_table.h"


/*=================================================================================*
 * executes first pass of the assembly file and checks for errors. it then 		   *
 * executes the second pass and outputs the instruction opcodes and data to the    *
 * Srecord 																		   *
 *=================================================================================*/
int main(int argc , char *argv[])
{
	char assembler_file[256];
	FILE *asm_f;
	LIS_f = fopen("assembly.LIS.txt" , "w");
	
	unsigned short emit_val;
	struct sym_entry *sym;
	bool valid_tbl = false;
	
	if(argc != 2){
		/*	give them the option of entering on the command line 
		 *	or just running the program*/
		printf("enter an assembler file name to compile:\n");
		scanf("%s" , &assembler_file[0]);
		asm_f = fopen(assembler_file , "r");
	}
	else if((asm_f = fopen(argv[1] , "r")) == NULL){
		printf("File %s could not be found\n" , argv[1]);
		getchar();
		return 0;
	}
	sym_tbl_init();
	Srec_init();
	
	/*get one record from file until EOF or END directive*/
	while (fgets(assembly.line , 256, asm_f) != NULL && end_flag == false) 
	{	
		LC_old = LC_cur;
		parse_record();
		print_record();							
		clear_record();
		if(LC_cur > 65535){
			error_flag = true;
			fprintf(LIS_f , "%04x|\t[ERROR] LC is too high of a value\n");
			break;
		}		
	}
	
	if(end_flag == false)
		fprintf(LIS_f , "\t[ERROR] No END directive found\n");
	
	print_sym_tbl();
	valid_tbl = check_sym_tbl();
	if(valid_tbl == false || error_flag == true)
	{
		fprintf(LIS_f , "\t[ERROR] One or more errors found " , LC_cur); 
		fprintf(LIS_f , "second pass not executed\n");
		return 0;
	}

	rewind(asm_f);
	LC_cur = 0;
	second_pass = true;
	end_flag = false;
	
	
	while (fgets(assembly.line , 256, asm_f) != NULL && end_flag == false)
	{
		LC_old = LC_cur;
		parse_record();
		
		if(assembly.inst.name[0] != '\0'){
			set_address();
			LC_old = emit_word(assembly.inst.addr.ADDR_whole , LC_old);
		}
			
		if(assembly.inst.type == 1 || assembly.inst.type == 2){
			if(assembly.src.mode == RELATIVE){
				sym = get_entry(assembly.src.name);
				emit_val = sym->value - (LC_old + 2);
				LC_old = emit_word(emit_val , LC_old);
			}
			else if(assembly.src.mode == ABSOLUTE  || 
					assembly.src.mode == IMMEDIATE ||
					assembly.src.mode == INDEXED){
				sym = get_entry(assembly.src.name);
				emit_val = sym->value;
				LC_old = emit_word(emit_val , LC_old);
			} 
				
		}
		if(assembly.inst.type == 2){
			if(assembly.dst.mode == INDEXED || assembly.dst.mode == ABSOLUTE){
				sym = get_entry(assembly.dst.name);
				emit_val = sym->value;
				LC_old = emit_word(emit_val , LC_old);
			} 
			else if(assembly.dst.mode == RELATIVE){
				sym = get_entry(assembly.dst.name);
				emit_val = sym->value - (LC_old + 2);
				LC_old = emit_word(emit_val , LC_old);
			}		
		}
		clear_record();	
	}
	fclose(asm_f);
	
	output_Srec();
	Srec_s9();
	
	return 0;
}




