/*
 *	operand.c
 *	Author	: Nick Stanwood
***********************************************************************************/
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<ctype.h>
#include <stdlib.h>

#include "operand.h"	
#include "record.h"
#include "symbol_table.h"
#include "S_rec.h"


/*	need a temp index reg to set in the function index_operand()
 *	then get_operands() will set the respective register 
 */
int INDEX_REG_temp 	= 0;
int INDEX_REG_src 	= 0;
int INDEX_REG_dst 	= 0;


/*=================================================================================*
 *
 *=================================================================================*/
void get_operands(void)
{	
	char *sanity , *tok;
	struct op *opptr;
	opptr = malloc(sizeof(struct op));
	
	/*type 1 inst or type  2 inst or jump*/
	if(assembly.inst.type>=1)
	{
		if(assembly.inst.type != 2)
			tok = strtok(NULL , " ;\n\t\r");
		else{
			 tok = strtok(NULL , ",");
			 while(*tok == ' ' || *tok == '\t')
			 	tok++;
		}
			
		strcpy(opptr->name , tok);
		if(opptr->name != NULL){
			mode_check(opptr);
			/*only type 1 & 2*/
			if(opptr->mode < 7 && assembly.inst.type != 3){
				/*get rid of addressing mode chars to search sym_tbl later*/ 
				tok = operand_check(opptr->name);	
				strcpy(assembly.src.name , tok);
				assembly.src.mode = opptr->mode;
				
				if(opptr->mode == INDEXED)
					INDEX_REG_src = INDEX_REG_temp;	
			}
			/*jump inst can only be relative*/
			else if((opptr->mode == RELATIVE) && (assembly.inst.type == 3)){
				/*no need for operand_check()*/ 
				strcpy(assembly.src.name , opptr->name);		
			}
			else{ //bad addressing mode
				error_flag = true;
				assembly.error = true;
				fprintf(LIS_f , "%04x|\t[ERROR] >>%s<<" , LC_cur, opptr->name);
				fprintf(LIS_f , " has bad addressing mode\n");
			} 
					
		}else{ //null operand pointer
			error_flag = true;
			assembly.error = true;
			fprintf(LIS_f , "%04x|\t[ERROR] No source operand\n" , LC_cur);
		}
			
			
	}
		
	/*only type 2 inst*/	
	if(assembly.inst.type ==2)
	{
		tok = strtok(NULL , " ;\n\t\r");
		strcpy(opptr->name , tok);
		if(opptr->name != NULL){
			mode_check(opptr);
			/*check if it is an invalid destination mode*/
			if(opptr->mode < 4){
				tok = operand_check(opptr->name);
				strcpy(assembly.dst.name , tok);
				assembly.dst.mode = opptr->mode;
				if(opptr->mode == INDEXED)
					INDEX_REG_dst = INDEX_REG_temp;
			}
			else{	//bad addressing mode
				error_flag = true;
				assembly.error = true;
				fprintf(LIS_f , "%04x|\t[ERROR]" , LC_cur);
				fprintf(LIS_f , " Destination has bad addressing mode\n");
			} 
						
		}else{	//no destination operand
			error_flag = true;
			assembly.error = true;
			fprintf(LIS_f , "%04x|\t[ERROR] No destination operand\n" , LC_cur);
		} 
					
	}
	
	/*if there are still more valid chars on the line 
	  after getting both operands then there is an error*/	
	sanity = strtok(NULL , "\n");
	if(sanity != NULL)
		if(strspn(sanity , " \t") != strlen(sanity)){
			fprintf(LIS_f , "%04x|\t[ERROR] Too many operands\n" , LC_cur);
			assembly.error = true;
			error_flag = true;
			}
			
	
	return;
}


/*=================================================================================*
 *
 *=================================================================================*/
void mode_check(struct op* operand)
{

	char *opptr;
	opptr = operand->name;
	
	switch(*opptr)
	{	
		case '@':
			operand->mode = ind_operand(opptr);
			break;
	
		case '&':	
			operand->mode = abs_operand(opptr);
			if(operand->mode == ABSOLUTE)
				LC_cur += 2;
			else;
			break;	
		case '#':
			operand->mode = imm_operand(opptr);
			if(operand->mode == IMMEDIATE)
				LC_cur += 2;
			else;
			break;
		
		default:
			operand->mode = alpha_operand(opptr);
			if(operand->mode == INDEXED  || 
			   operand->mode == RELATIVE && 
			   assembly.inst.type != 3)
				LC_cur += 2;
			else;
			break;			
	}
}

/*=================================================================================*
 *
 *=================================================================================*/
enum ADDR_MODE ind_operand(char *opptr)		//	@Rn or @Rn+
{
	
	int sanity = 0;
	struct sym_entry* symbol;
	enum ADDR_MODE mode;
	char *opstart;
	opstart = opptr;			//save initial position of operand
	
	opptr++;
	while((isalnum(*opptr) || *opptr == '_') && sanity++ < 32)
		opptr++;
	
	if(*opptr == '+'){
		if(*++opptr != '\0')		//check if next character after + is NULL
			mode = BAD_MODE;		
		else{
			*--opptr = '\0';			//set '+' to '\0'
			mode = INDIRECT_INCR;	
		}
	}
	else if(*opptr == '\0')	mode = INDIRECT;
	else mode = BAD_MODE; 
	opstart++;				//getting past @ char
	symbol = get_entry(opstart);
	if(symbol == NULL) 
		mode = BAD_MODE;	//needs to be type REG 	
	else if(symbol->type != REG)
		mode = BAD_MODE;

	return mode;
	
}


/*=================================================================================*
 *
 *=================================================================================*/
enum ADDR_MODE abs_operand(char *opptr)		//	&LABEL
{
	int sanity = 0;
	struct sym_entry* symbol;
	enum ADDR_MODE mode;
	char *opstart;
	opstart = opptr;
	
	/*check if first character after & is alphabetic*/
	if(!isalpha(*++opptr)){
		mode = BAD_MODE;
		return mode;	
	}
	
	while((isalnum(*opptr) || *opptr == '_') && sanity++ < 32)
		opptr++;
		
	if(*opptr == '\0'){
		mode = ABSOLUTE;
		opstart++;						//getting past & char
		symbol = get_entry(opstart);
		
		if(symbol == NULL) 
			add_entry(opstart , RANDOM_VAL , UNKNOWN); 	
		
		else if(symbol->type == REG)								
			mode = BAD_MODE;
	}
	
	else
		mode = BAD_MODE;
	
	return mode;	
}


/*=================================================================================*
 *
 *=================================================================================*/
enum ADDR_MODE imm_operand(char *opptr)		//	#LABEL , #x
{
	int sanity = 0 , value = 0;
	struct sym_entry* symbol;
	enum ADDR_MODE mode;
	char *opstart;
	opstart = opptr;
	
	opptr++;		//get past #
	if(*opptr == '$') opptr++;	//get past '$' if its hex
	while((isalnum(*opptr) || *opptr == '_' || *opptr == '-') && sanity++ < 32)
		opptr++;
	if(*opptr == '\0')
		mode = IMMEDIATE;
	else
		mode = BAD_MODE;
		
	opstart++;				//getting past # char
	if(*opstart == '$'){
		symbol = get_entry(opstart);
		value = strtol(++opstart , NULL , 16);
	}	
	else{
		value = atoi(opstart);
		symbol = get_entry(opstart);
	} 
	
	
	/*	if its not in the symbol table and it is a valid number
	 *	cant just check if value != 0 because the programmer may have 
	 *	wanted the value 0 i.e 0(Rn) so check to see if 
	 *	every value in the string is 0*/
	if(symbol == NULL && (value != 0 || 
	  (strspn(opstart , "0") == strlen(opstart))))
	{
		/*add an entry with a value equal to itself 
		for ease of access down the road*/
		if(*--opstart == '$');		//point opstart back to '$' if there is one
		else opstart++;
		add_entry(opstart , value , IMM); 		
	}
	else if(symbol == NULL && value == 0 && *opstart != '0')
	{
		/*add an entry with an unknown type because 
		this is most likely a forward reference*/
		add_entry(opstart , value , UNKNOWN);
	}  	
	 
	return mode;
	
}


/*=================================================================================*
 *
 *=================================================================================*/
enum ADDR_MODE alpha_operand(char *opptr)	//	Rn , LABEL(Rn) , LABEL
{
	int sanity = 0, val;
	struct sym_entry *symbol;
	enum ADDR_MODE mode;
	char *opstart , *op_ind;

	opstart = opptr;
	
	if(strchr(opptr , '(')){		//indexed mode
		mode = indexed_operand(opptr);
		return mode;
	}
	
	/*check if first char is alphabetic*/
	if(!isalpha(*opptr)){
		symbol = get_entry(opptr);
		if(*opptr == '$'){
			opptr++;	
			val = strtol(opptr , NULL , 16);
			if(val == 0 && *opptr != '0')
				mode = BAD_MODE;
			else{
				mode = RELATIVE;
				if(symbol == NULL)
					add_entry(opstart , val , IMM);
				else;
			}		
		}
		else{
			val = atoi(opptr);
			if(val == 0 && *opptr != '0')
				mode = BAD_MODE;
			else{
				mode = RELATIVE;
				if(symbol == NULL)
					add_entry(opstart , val , IMM);
			}
		}
		return mode;	
	}
	while((isalnum(*opptr) || *opptr == '_') && sanity++ < 32)
		opptr++;
	/*entire operand is alphnumeric (register direct or relative)*/
	if(*opptr == '\0')				
	{
		symbol = get_entry(opstart);
		if(symbol != NULL)
		{
			if(symbol->type == REG)
				mode = REGISTER;
			else mode = RELATIVE;
		}
		else{
			add_entry(opstart , RANDOM_VAL , UNKNOWN);
			mode = RELATIVE;
		}
	}
	else 
		mode = BAD_MODE;
			
	return mode;
}


/*=================================================================================*
 *
 *=================================================================================*/
enum ADDR_MODE indexed_operand(char *opptr)	//	LABEL(Rn) , $x(Rn) , x(Rn)
{
	int sanity = 0 , val = 0;
	char *op_ind , *opstart;
	
	enum ADDR_MODE mode = INDEXED;
	struct sym_entry *symbol;
	opstart = opptr;
	op_ind = opptr;
	
	while(*op_ind != '(')
		op_ind++;
		
	*op_ind = '\0';							//set '(' to '\0'
	symbol = get_entry(opstart);
	
	/*if indexed with hex value as label*/
	if(*opstart == '$' && symbol == NULL){											
		val = strtol(++opptr , NULL , 16);
		if(val == 0 && *opstart != '0')		//if it is not a proper hex value
			mode = BAD_MODE;
		else
			add_entry(opstart , val , IMM);
			
	}
	/*if indexed with decimal value as label*/
	else if(isdigit(*opstart) && symbol == NULL){			
		val = atoi(opstart);
		if(val == 0 && *opstart != '0')		//if not proper decimal value
			mode = BAD_MODE;
		else 								//proper deciaml value
			add_entry(opstart , val , IMM);	
			
	}
	/*if indexed with a label as the value*/		
	else if(symbol == NULL){								
		while((isalnum(*opptr) || *opptr == '_') && sanity++ < 32)
			opptr++;
		if(*opptr == '\0')
			add_entry(opstart , RANDOM_VAL , UNKNOWN);	
		else 
			mode = BAD_MODE;
	}

	opptr = op_ind;		//op_ind already points towards '\0' of first symbol
	opptr++;			//point opptr to the start of the index register
	sanity = 0;
	
	while(isalnum(*opptr) && sanity++ < 32)
		opptr++;
	if(*opptr == ')' && *++opptr == '\0'){
		*--opptr = '\0';				//set ')' to '\0'
		symbol = get_entry(++op_ind);
		*opptr = ')';					//set back to ')'
		if(symbol != NULL && symbol->type == REGISTER)
			INDEX_REG_temp = symbol->value;			//set INDEX_REG to offset	
		else 
			mode = BAD_MODE;
	}
	else 
		mode = BAD_MODE; 					
	
	return mode;
}


/*=================================================================================*
 *
 *=================================================================================*/
char* operand_check(char *op)
{	
	char *operand = op;
	
	if(*operand == '@' || *operand == '#' || *operand == '&')
		op++;
	
	while(*operand != '(' && *operand != '\0' && *operand != '+')
		operand++;
	*operand = '\0';
	
	return op;
}
