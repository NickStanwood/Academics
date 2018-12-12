/*
 *	mainline.c
 *	Purpose:	
 *		The purpose of this program is to emulate the inner workings
 *		of the MSP-430  CPU. It loads an S-record into memory, sets the initial 
 *		program counter value from the S9 record and then begins to fetch, decode 
 *		and execute the loaded program.	
 *		
 *		An optional debugger can be run with the program to see what is happening
 *		after each instruction is executed. various other options are available in 
 *		the debugger as well.
 *
 *		Devices are included in the emulation and are chosen by the user during
 *		initialization. A miximum of 16 devices are available	
 *			 
 *	Author:	Nick Stanwood
 *	Date:	June 15 - created loader emulator
 *			June 24 - put in fetch decode execute loop
 *			June 29 - put in debugger initialization sequence
 *			July 10 - seperated the fetch, decode and execute loop into two
 *					  different loops depending on if the debugger is running
 *					  to avoid unnecessary checks when running without the debugger 
 **********************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "memory.h"
#include "CPU.h"
#include "reg_table.h"
#include "debugger.h"
#include "device.h"


int main(int argc , char *argv[])
{
	char Srec[256];
	char *infile;
	char debug = 'n';
	long i=-1;
	bool Scheck = true;
	
	init_mem();
	init_devs();
	
	if(argc >= 2){
		if((Srec_f = fopen(argv[1] , "r")) != NULL){
			printf("loaded file %s successfully\n" , argv[1]);
			debug_opt[LOAD_F] = true;	
		}else{
		printf("File %s could not be found\n" , argv[1]);
		getchar();
		return 0;
		}	
	}
	
	printf("run with debugger? (y/n): ");
	scanf(" %c",&debug);
	if(debug == 'y'){
		printf("initialize debugger with which options? ");
		printf("('H' for list) ");
		debug_init();
	}
	
	
	
	
	/*open the default file if no file was selelcted during debug initialization*/
	if(!debug_opt[LOAD_F])
		Srec_f = fopen("default.txt" , "r");
		
/*=================================================================================*
 *	Loader																		   *
 *	takes all the Srecords from the Srecord file and loads them into memory		   *	
/*=================================================================================*/
	while(fgets(Srec , 256, Srec_f) != NULL && Scheck == true) 
	{	
		Scheck = load_Srec(Srec);
		memset(Srec , '\0' , sizeof(Srec));	
	}

/*=================================================================================*
 *	CPU emulation																   *
 *	fetch decode and execute loop in debug mode									   *
 *=================================================================================*/
	if(debug == 'y'){
		i = inst_cnt;
		while(sys_clock != -1 && !debug_opt[TERM]){
			while(!ISC_OFF && (i-- != 0) && reg_tbl[PC] < brkpt)
			{
				fetch();
				MDB(MDR , &inst_reg);					/* MDR -> inst_reg 	*/
				decode();
				execute();
				if(debug_opt[AUTO] || debug_opt[WATCH])
					print_inst();
				if(ISGIE)
					poll_dev();
			}
			i = inst_cnt;
			if(ISC_OFF && ISGIE){
				inc_clk();
				if(debug_opt[AUTO])
					printf("clock = %ld\n" , sys_clock);
				poll_dev();
			}
			scan_debug();	
		}	
	}
	
/*=================================================================================*
 *	CPU emulation																   *
 *	fetch decode and execute loop while not in debug mode						   *
 *=================================================================================*/
	else{ 
		while(sys_clock != -1){
			while(!ISC_OFF)
			{
				fetch();
				MDB(MDR , &inst_reg);					/* MDR -> inst_reg 	*/
				decode();
				execute();
				if(ISGIE)
					poll_dev();
			} /*clock is off - keep checking for device intterrupts*/
			if(ISGIE){
				inc_clk();
				poll_dev();
			}	
		}
	}
	return 0;
}
