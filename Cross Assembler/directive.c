/*
 *	directive.c
 *	Author	: Nick Stanwood
**********************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>

#include "S_rec.h"
#include "directive.h"
#include "symbol_table.h"
#include "record.h"

/*array of directives for searching*/
const char*  DIR[8]  = 	{"ALIGN", "ASCII", "BSS" , "BYTE" , 
						 "END"  , "EQU"  , "ORG" , "WORD" };

bool end_flag = false; //set to true when an END directive is found


/*=================================================================================*
 *
 *=================================================================================*/
int search_DirList(char tok[])
{	
	int bottom = 0;
	int top = 7;
	int pos;
	int found = -1;
	
	while((bottom <= top) && (found == -1))
	{
		pos = (bottom + top)/2;
		if(strcmpi(DIR[pos] , tok) == 0)
			found = pos;
		else if(strcmpi(DIR[pos] , tok) > 0)
			top = pos -1;
		else if(strcmpi(DIR[pos] , tok) < 0)
			bottom = pos + 1;
	}
	return found;
}


/*=================================================================================*
 *
 *=================================================================================*/
void process_dir(int directive)
{
	enum dir_names dir = directive;
	char *argptr , *asciiptr , *checkptr;
	unsigned short emit_val = 0;
	int sanity = 0;
	struct sym_entry *EQU_label;
	
	switch(dir)
	{
		case ALIGN:
			if(LC_cur % 2){
				if(second_pass)
					LC_cur = emit_byte( 0 , LC_cur);
				else 
					LC_cur++;
			} 
				
			break;
		case ASCII:
			argptr = strtok(NULL , "");	//find rest of characters on record
			
			if(argptr != NULL && strlen(argptr) < 256){
				argptr = strchr(argptr , '\"'); 	//find first "
				argptr++;							//move to position after first "
				asciiptr = strchr(argptr , '\"');	//find last "
				if(asciiptr != NULL)
					*asciiptr = '\0';				//set last " to '\0'
				else{
					error_flag = true;
					assembly.error = true;
					fprintf(LIS_f , "%04x|\t[ERROR] Incorrect string\n" , LC_cur);
				}
					
				if(!second_pass)
					LC_cur = LC_cur + strlen(argptr);
				else
					while(*argptr != '\0'){
						LC_cur = emit_char(*argptr++ , LC_cur);	
					}							
			}	
			else{
				error_flag = true;
				assembly.error = true;
				fprintf(LIS_f , "%04x|\t[ERROR] " , LC_cur);
				fprintf(LIS_f , "Incorrect string for ASCII\n");
			}
					
			break;
		case BSS:
			argptr = strtok(NULL , " \n\t\r");
			
			if(*argptr == '$' && argptr != NULL){
				if(strtol(++argptr , NULL , 16) <= (0xFFFF - LC_cur)){
			/*if its a hex value and less than the max amount of memory*/
				LC_cur = LC_cur + strtol(argptr , NULL , 16);	
				if(second_pass == true)
					output_Srec();	
				}
				else{
					error_flag = true;		
					assembly.error = true;
					fprintf(LIS_f , "%04x|\t[ERROR]" , LC_cur);
					fprintf(LIS_f , " hex value too large for BSS\n");	
				}
					
			} 
				
			
			else if(atoi(argptr) <= (65535 - LC_cur)  && argptr != NULL){
			 /*if its a deciaml value and less than the max amount of memory*/
				LC_cur = LC_cur + atoi(argptr);	
				if(second_pass == true)
					output_Srec();	
			}
					
			else {
				error_flag = true;		
				assembly.error = true;
				fprintf(LIS_f , "%04x|\t[ERROR]" , LC_cur);
				fprintf(LIS_f , " Incorrect value for BSS\n");
			}
			
			break;
		case BYTE:
			argptr = strtok(NULL , " \n\t\r");
			if(!second_pass && argptr != NULL){
				if(*argptr == '$' && strtol(++argptr , NULL , 16) <= 255)
					LC_cur++;
				else if(atoi(argptr) <= 255)
					LC_cur++;			
				else{
					error_flag = true;
					assembly.error = true;
					fprintf(LIS_f , "%04x|\t[ERROR]" , LC_cur);
					fprintf(LIS_f , " Incorrect value for BYTE\n");
				} 
				
			}
			else if(second_pass && argptr != NULL){
				if(*argptr == '$' && strtol(++argptr , NULL , 16) <= 255)
					LC_cur = emit_byte(atoi(argptr) , LC_cur);
				else if(atoi(argptr) <= 255)
					LC_cur = emit_byte(atoi(argptr) , LC_cur);			
			}
			break;
		case END:
			end_flag = true;
			if(second_pass){
				argptr = strtok(NULL , " \n\t\r");
				
				if(*argptr == '$' && argptr != NULL){
					if(strtol(++argptr , NULL , 16) <= 0xFFFF)
						PC_init = strtol(argptr , NULL , 16); 	
				} 			
				else if(atoi(argptr) <= 65535  && argptr != NULL){
					PC_init = atoi(argptr);		
				}
				else
					PC_init = 0;
			}
			break;
		case EQU:	
			argptr = strtok(NULL , " \n\t\r");
			if(assembly.label[0] != '\0' && !second_pass && argptr != NULL)	
			{
				EQU_label = get_entry(assembly.label);
				if(*argptr == '$')
					EQU_label->value = strtol(++argptr , NULL , 16); 	
				else
					EQU_label->value = atoi(argptr);
				EQU_label->type = LABEL;
			}
			else if(!second_pass){
				error_flag = true;
				assembly.error = true;
				fprintf(LIS_f , "%04x|\t[ERROR]" , LC_cur);
				fprintf(LIS_f , " Incorrect value for EQU\n");
			}	
			break;
		case ORG:
			argptr = strtok(NULL , " \n\t\r");
			if(argptr != NULL){
				if(*argptr == '$')
					LC_cur = strtol(++argptr , NULL , 16);
				else 
					LC_cur = atoi(argptr);
				if(second_pass == true)
					output_Srec();	
			}
			
			break;
		case WORD:
			argptr = strtok(NULL , " \n\t\r");
			if(!second_pass && argptr != NULL){
				if(*argptr == '$' && strtol(++argptr , NULL , 16) <= 65535)
						LC_cur += 2;
				else if(atoi(argptr) <= 65535)
					LC_cur += 2;			
				else {
					error_flag = true;	
					assembly.error = true;	
					fprintf(LIS_f , "%04x|\t[ERROR]" , LC_cur);
					fprintf(LIS_f , "Incorrect value for WORD\n");
				}
			}
			else if(second_pass && argptr != NULL){
				if(*argptr == '$' && 
				  ((emit_val = strtol(++argptr , NULL , 16)) <= 65535))
						LC_cur = emit_word(emit_val , LC_cur);
				else if((emit_val = atoi(argptr)) <= 65535)
					LC_cur = emit_word(emit_val , LC_cur);				
			}
			else{
				error_flag = true;	
				assembly.error = true;	
				fprintf(LIS_f , "%04x|\t[ERROR]" , LC_cur);
				fprintf(LIS_f , "no value found for WORD\n");
			}	
	}
	
	/*if there are still more valid chars on the line 
	  after processing the directive there is an error*/	
	checkptr = strtok(NULL , "\n");
	if(checkptr != NULL && end_flag == false)
		if(strspn(checkptr , " \t") != strlen(checkptr)){
			fprintf(LIS_f , "%04x|\t[ERROR]" , LC_cur);
			fprintf(LIS_f , " Too many arguments for directive\n");
			assembly.error = true;
			error_flag = true;
			}
}

