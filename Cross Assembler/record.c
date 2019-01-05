/*
 *	Code Title  :record.c
 *	Author		: Nick Stanwood
***********************************************************************************/

#include "record.h"
#include "operand.h"
#include "instruction.h"
#include "directive.h"
#include "symbol_table.h"
#include "S_rec.h"

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>



struct record assembly = {"" ,  
                          "" , 
						 {"" , 0 , 0} , 
						  "" , 
						 {"" , BAD_MODE} , 
						 {"" , BAD_MODE} , 
						 false};

/*	current and old LC to keep track of what the current line is 
 *	as well as how much the current record needs to be increased by
 */ 
unsigned int LC_cur = 0; 
unsigned int LC_old = 0;

bool second_pass = false;
bool error_flag = false;

int As_val[7] = {0 , 1 , 1 , 1 , 2 , 3, 3};
int Ad_val[4] = {0 , 1 , 1 ,1};


/*=================================================================================*
 *
 *=================================================================================*/
void parse_record(void)
{
	
	int i = 0 , 
		j = 0 , 
		sanity 	 = 0 , 
		position = 0 ;
	
	char *tok , *lblptr;
	struct sym_entry *sym;
	
	bool 	label 	= false ,
			done	= false ;
	
	/*getting rid of everything after the ; if there is one*/
	for(i = 0 ; i < 256 ; i++){
		if(assembly.line[i] == ';'){
			for(j = i ; j < (256 - i) ; j++)
				assembly.line[j] = '\0';
			break;
		}
	}
	
	/*getting the first token of the line*/
	tok = strtok(assembly.line , " \t\n;");
	  
	while(!done && tok != NULL)
	{	
		/*if it is an instruction*/
		if((position = search_InstList(tok)) != -1)			
		{
			instcpy(position);
			get_operands();
			LC_cur = LC_cur + 2;
			done = true;
		}
		/*if it is a directive*/
		else if((position = search_DirList(tok)) != -1)
		{	
			strcpy(assembly.dir , DIR[position]);
			process_dir(position);
			done = true;
		}
		/*if its not second pass, check if its a valid label*/
		else if(!second_pass)
		{
			lblptr = tok;
			/*check if alphanumeric label and less than max label length*/
			while((isalnum(*lblptr) || *lblptr == '_') && (sanity++ < 32))
				lblptr++;
			/*if there hasnt been a label in the record already*/
			/*and the label is a valid one*/
			if(!label && *lblptr == '\0' && (isalpha(*tok) || *tok == '_')){
				sym = get_entry(tok);
				/*check if tok is in the sym_tbl*/
				if(sym == NULL){
					strcpy(assembly.label , tok);
					label = true;
					add_entry(tok , LC_old , LABEL);	
				}else if(sym->type == UNKNOWN){
					strcpy(assembly.label , tok);
					label = true;
					sym->type = LABEL;
					sym->value = LC_old;
				}
				else{
					error_flag = true;
					assembly.error = true; 
					fprintf(LIS_f,"%04x|\t[ERROR]" , LC_cur);
				 	fprintf(LIS_f , " %s already defined\n" , tok);
				}		
			}
			else{
				/*second label in the record*/
				if(label){
					fprintf(LIS_f,"%04x|\t[ERROR]",LC_cur);
					fprintf(LIS_f , "Too many labels on line\n");
				}
					
				/*incorrect label format*/
				else if(*lblptr != '\0'){
					fprintf(LIS_f,"%04x|\t[ERROR]",LC_cur);
					fprintf(LIS_f , " %s not alphanumeric\n" , tok);
				}
				else{
					fprintf(LIS_f,"%04x|\t[ERROR]",LC_cur);
					fprintf(LIS_f," %s must have alphabetic 1st character\n" , tok);
				}
					
				error_flag = true;
				assembly.error = true;
				done = true;
			} 				
		}
		tok = strtok(NULL , " \n\t");		
	}
	return;
}


/*=================================================================================*
 *
 *=================================================================================*/
void print_record(void)
{	
	if(assembly.error == true)
		return;

	fprintf(LIS_f , "%04x|\t" , LC_old);
	
	if(assembly.label[0] != '\0')
		fprintf(LIS_f ,"%s", assembly.label);
	if(assembly.dir[0] != '\0')
		fprintf(LIS_f , "\t%s", assembly.dir);
	if(assembly.inst.name[0] != '\0')
	{
		fprintf(LIS_f , "\t%s" , assembly.inst.name);
		if(assembly.inst.type == 1 || assembly.inst.type == 2){
			fprintf(LIS_f , "\t%s" , assembly.src.name);
			if(assembly.src.mode == 0)
				fprintf(LIS_f , "[dir]\t");
			else if(assembly.src.mode == 1)
				fprintf(LIS_f , "(%d)" , INDEX_REG_src);
			else if(assembly.src.mode == 2)
				fprintf(LIS_f , "[rel]");
			else if(assembly.src.mode == 3)
				fprintf(LIS_f , "[abs]");
			else if(assembly.src.mode == 4)
				fprintf(LIS_f , "[ind]");
			else if(assembly.src.mode == 5)
				fprintf(LIS_f , "[ind+]");
			else if(assembly.src.mode == 6)
				fprintf(LIS_f , "[imm]");
			else if(assembly.src.mode == 7)
				fprintf(LIS_f , "[bad]");
		}
			
		if(assembly.inst.type == 2){
			fprintf(LIS_f ,"\t%s" , assembly.dst.name);
			if(assembly.dst.mode == 0)
				fprintf(LIS_f , "[dir]");
			else if(assembly.dst.mode == 1)
				fprintf(LIS_f , "(%d)" , INDEX_REG_dst);
			else if(assembly.dst.mode == 2)
				fprintf(LIS_f , "[rel]");
			else if(assembly.dst.mode == 3)
				fprintf(LIS_f , "[abs]");
			else if(assembly.dst.mode == 7)
				fprintf(LIS_f , "[bad]");
		}
	}
	
	fprintf(LIS_f , "\n");
	
	return;
}


/*=================================================================================*
 *
 *=================================================================================*/
void clear_record(void)
{
	strcpy(assembly.line , "");
	strcpy(assembly.label , "");
	strcpy(assembly.inst.name , "");
	assembly.inst.addr.ADDR_whole = 0;
	assembly.inst.type = 0;
	strcpy(assembly.dir , "");
	strcpy(assembly.src.name , "");
	strcpy(assembly.dst.name , "");
	assembly.src.mode = BAD_MODE;
	assembly.dst.mode = BAD_MODE;
	assembly.error = false;
}


/*=================================================================================*
 *
 *=================================================================================*/
void set_address(void)
{
	struct sym_entry *src , *dst;
	int jmp_dist;
	
	
	switch(assembly.inst.type)
	{
		case 1:
			src = get_entry(assembly.src.name);
			assembly.inst.addr.ADDR1.As = As_val[assembly.src.mode];
			
			if(assembly.src.mode == INDEXED){
				assembly.inst.addr.ADDR1.reg = INDEX_REG_src;
			}
			else if(assembly.src.mode == IMMEDIATE || 
					assembly.src.mode == RELATIVE){
						assembly.inst.addr.ADDR1.reg = PC;
			}	
			else if(assembly.src.mode == ABSOLUTE){
				assembly.inst.addr.ADDR1.reg = SR;
			}
			else { //direct , indirect , indirect+
				assembly.inst.addr.ADDR1.reg = src->value;
			}
			break;
			
		case 2:
			src = get_entry(assembly.src.name);
			dst = get_entry(assembly.dst.name);
			
			assembly.inst.addr.ADDR2.As = As_val[assembly.src.mode];
			assembly.inst.addr.ADDR2.Ad = Ad_val[assembly.dst.mode];
			
			if(assembly.src.mode == INDEXED){
				assembly.inst.addr.ADDR2.src = INDEX_REG_src;
			}
			else if(assembly.src.mode == IMMEDIATE || 
					assembly.src.mode == RELATIVE){
						assembly.inst.addr.ADDR2.src = PC;
			}	
			else if(assembly.src.mode == ABSOLUTE){
				assembly.inst.addr.ADDR2.src = SR;
			}
			else{	//direct , indirect , indirect+
				assembly.inst.addr.ADDR2.src = src->value;
			}
			
			if(assembly.dst.mode == REGISTER){
				assembly.inst.addr.ADDR2.dst = dst->value;
			}
			else if(assembly.dst.mode == INDEXED){
				assembly.inst.addr.ADDR2.dst = INDEX_REG_dst;
			}
			else if(assembly.dst.mode == RELATIVE){
				assembly.inst.addr.ADDR2.dst = PC;
			}
			else if (assembly.dst.mode == ABSOLUTE){
				assembly.inst.addr.ADDR2.dst = SR;
			}
		
			break;
		case 3:
			dst = get_entry(assembly.src.name);
			jmp_dist = dst->value - LC_old - 2;
			if((jmp_dist < -1024) || (jmp_dist > 1022) || (jmp_dist % 2)){
				error_flag = true;
				fprintf(LIS_f,"%04x|\t[ERROR] Jump distance of" , LC_cur);
				fprintf(LIS_f , " %d is too far\n", jmp_dist);
			}		
			else
				assembly.inst.addr.ADDR3.dst = jmp_dist/2;	
			break;	 
	}
	return;		
}




