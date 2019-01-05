/*
 *	S_rec.c
 *	Author	: Nick Stanwood
**********************************************************************************/

#include <stdio.h>
#include "S_rec.h"

int PC_init = 0; 
static union Srecord Srec_data[max_data] = {0};
static unsigned int Srec_addr = 0;
static signed int check_sum = 0;
static unsigned int Srec_len = 0;
static unsigned int index = -1;  


extern void Srec_init(void)
{
	Srec_f = fopen("default.txt" , "w");
	return; 
}
 
unsigned int emit_word(unsigned short address , unsigned int LC)
{
	union Srecord data_LL , data_HH;
	int i;
	
	data_LL.decimal.byte = address & 0xFF;
	data_HH.decimal.byte = (address >> 8) & 0xFF;

	/*if the Srec has no data*/
	if(index == -1){
		Srec_addr = LC;
		check_sum = Srec_addr & 0xFF;
		check_sum += (Srec_addr >> 8) & 0xFF;
		index= 0;
	}
	
	Srec_data[index].decimal.byte = data_LL.decimal.byte;
	check_sum += Srec_data[index].shrt;
	LC++;
	index++;
	
	if(index >= 32)
	{
		output_Srec();
		Srec_addr = LC;
		check_sum = Srec_addr & 0xFF;
		check_sum += (Srec_addr >> 8) & 0xFF;
		index= 0;
	}
	
	Srec_data[index].decimal.byte = data_HH.decimal.byte;
	check_sum += Srec_data[index].shrt;
	index++;
	LC++;
	if(index >= 32)
		output_Srec();

	return LC;
}
 
 
 
unsigned int emit_char(unsigned char byte , unsigned int LC)
{
 	if(index == -1){
			Srec_addr = LC;
			check_sum = Srec_addr & 0xFF;
			check_sum += (Srec_addr >> 8) & 0xFF;
			index = 0;	
		}
	Srec_data[index].decimal.byte = byte;
	check_sum += Srec_data[index].shrt;
	index++;
	LC++;
	
	if(index >= 32)
		output_Srec();
	return LC;
}

unsigned int emit_byte(unsigned short val , unsigned int LC)
{
	union Srecord byte;
	byte.decimal.byte = val & 0x00FF;
	
	if(index == -1){
			Srec_addr = LC;
			check_sum = Srec_addr & 0xFF;
			check_sum += (Srec_addr >> 8) & 0xFF;
			index = 0;	
		}
	Srec_data[index].decimal.byte = byte.decimal.byte;
	check_sum += Srec_data[index].shrt;
	index++;
	LC++;
	
	if(index >= 32)
		output_Srec();
	return LC;	
} 
 
void output_Srec(void)
{
 	int i = 0, check;
 	/*+ 3 for address and checksum*/	
 	Srec_len = index + 3;
	check_sum += Srec_len; 
	 
 	if(index == -1)
		return;
	 		
 	fprintf(Srec_f , "S1%02x%04x" , Srec_len , Srec_addr);
 	fprintf(LIS_f , "S1%02x%04x" , Srec_len , Srec_addr);
 	check = Srec_len + Srec_addr;
 	
 	for(i = 0 ; i < index ; i++){
 		fprintf(Srec_f , "%02x" , Srec_data[i].decimal.byte);
 		fprintf(LIS_f , "%02x" , Srec_data[i].decimal.byte);
 		check +=Srec_data[i].decimal.byte;
	 }
 		
 	check += (~check_sum & 0xFF);
 	fprintf(Srec_f , "%02x\n" , (~check_sum & 0xFF));
 	fprintf(LIS_f , "%02x\n" , (~check_sum & 0xFF));
 	
	index = -1;
 	check_sum = 0;
 	
 	return;
 		
}
 
void Srec_s9(void)
{
	fprintf(Srec_f , "S903%04xFC\n",PC_init);
	fprintf(LIS_f , "S903%04xFC\n",PC_init);
	fclose(Srec_f);
	return;	
} 
 
 
 
 
 
 
 
 
 
 
