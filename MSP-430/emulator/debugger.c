/*
 *	debugger.c
 *	Purpose:
 *		This source is used to help debug the programmers assembly code (lord knows
 *		it helped me debug the emulator code as well)
 *	Debugger options:
 *		L file filename-- loads 'filename' into mem[]
 *		L S10000--------- loads 'S100000' directly into mem[]
 *		I hex_val-------- initializes the PC to 'hex_val'
 *		B hex_val-------- stops execution when PC == 'hex_val'
 *		W dec_val-------- lets user watch program happen. program runs for 'dec_val' 
 *						  number of instructions before	giving control back to the 
 *						  debugger
 *		S --------------- dump S-record memory as it gets loaded
 *		A --------------- Auto debug mode, prints out all relevent registers, memory 
 *						  space and clock cycles for each instruction executed
 *								
 *		R all------------ dumps all registers
 *		R dec_val-------- dumps the register specified by dec_val
 *		M hex_val1 hex_val2 - dumps all memory between 'hex_val1' and 'hex_val2'
 *		D --------------- dumps all device values (port, ISR addr and dev SR) 
 *		T --------------- terminates the program
 *	Author:	Nick Stanwood
 *	Date:	June 28 - created header file
 *			June 30 - changed made sccan_debug() its own function to better seperate 
 *					  the run time debug options and the initialization options
 *			JULY 2	- added the Dev dump and terminate options to scan_debug()
 **********************************************************************************/
 
 #include "debugger.h"
 #include "memory.h"
 #include "reg_table.h"
 #include "device.h"
 #include "ALU.h"
 #include "decoder.h"
 
 #include <string.h>
 
 char debug_opt[11] = {0};
 int brkpt = -1;
 long inst_cnt = -1;
 
 
/*================================================================================*
 * 	Lets the user initialize the debugger with the options they choose			  *
 *================================================================================*/
 extern void debug_init(void)
 {
 	char opt = 0;							/* debug option scanned from stdin */
 	char discard;
 	int val1;								/* scanning in decimal values  */
 	int hexval1 , hexval2;					/* scanning in hex  values	   */
 	char str[256];							/* reading in a file name	   */
 	int Rn = 0;								/* dump specific register	   */
 	bool done = false;
 	
 	while(!done){
 		scanf(" %c", &opt);
 		switch(opt)
 		{
 			case 'h':
			case 'H':
				printf("comprehensive list of debugger options in DEBUGGER.txt\n\n");
				printf("[L file filename] 	Load file\n");
				printf("[L S1]			Load S-record\n");
				printf("[I hexval]		Init PC\n");
				printf("[B hexval]		Breakpoint\n");
				printf("[W decval]		Watch\n");
				printf("[A] 			Automatic debugger\n"); 
				printf("[S] 			S-record dump\n");
				printf("[H]			help list\n");
				printf("[Q] 			Quit\n");
				
				printf("\ninitialize debugger with which options? ");
				debug_init();
				break;
			case 'l':	
 			case 'L':
				scanf("%s", &str[0]);
				if(!stricmp(str,"file")){
					scanf("%s",&str[0]);
					if((Srec_f = fopen(str , "r")) == NULL){
						printf("opening default test file\n");
						Srec_f = fopen("default.txt" , "r");
						strcpy(str,"default.txt");
					}
					debug_opt[LOAD_F] = true;
					printf("loaded file %s succesfully\n",str);
				}
				else{	/*load S19 directly from command line*/
					load_Srec(str);
					debug_opt[LOAD_S] = true;
					printf("loaded S19\n");	
				}
				break;
			case 'i':	
			case 'I':
				scanf("%x" , &reg_tbl[PC]);
				debug_opt[INIT_PC] = true;
				printf("initialized PC to 0x%04x\n" , reg_tbl[PC]);
				break;
			case 'b':	
			case 'B':
				scanf("%x" , &brkpt);
				debug_opt[BRKPT] = true;
				printf("set breakpoint to 0x%04x\n" , brkpt);
				break;
			case 'w':	
			case 'W':
				scanf("%d" , &inst_cnt);
				debug_opt[WATCH] = true;
				printf("watch every %d instruction(s)\n", inst_cnt);
				break;
			case 'a':
			case 'A':
				debug_opt[AUTO] = true;
				printf("Auto debug enabled\n");	
				break;
			case 's':
			case 'S':
				debug_opt[SREC] = true;
				printf("Srec dump enabled\n");
				break;
			case 'q':
			case 'Q':
				done = true;
				scanf("%c" , &discard);	//get rid of '\n' folling the Q
				break;
			
			default:
				printf("invalid debugger option\n");
				break;
		}
	} /* done == true */
}
	
			
/*================================================================================*
 * 	this function is called during run time. it allows the user to view CPU 	  *
 *	and memory information depending on the options they choose.				  *  
 *================================================================================*/
extern void scan_debug(void)
{
	char opt = 0;						
 	int hexval1 , hexval2;			//for case memory dump
 	char reg_str[4];				//either 'all' or a number
 	int Rn = 0;
 	int DEVn = 0;
 	bool done = false;
	 	
	while(!done){
		opt = getchar();
		switch(opt)
		{	
			case 'h':
			case 'H':
				printf("comprehensive list of debugger options in DEBUGGER.txt\n\n");
				printf("\n[R all]			Reg dump - all registers\n");
				printf("[R decval]			Reg dump - Rn\n");
				printf("[M hexval1 hexval2] Mem dump\n");
				printf("[B hexval]			Breakpoint\n");
				printf("[D]					Device dump\n");
				printf("[T]					Terminate\n");
				printf("ENTER to continue to next instruction\n");
					
				printf("\nrun debugger with which options? ");
				scan_debug();
				break;
			case 'r':		
			case 'R':	/*register dump*/
				scanf("%s" , &reg_str);
				if(!stricmp(reg_str , "all")){
					for(Rn = 0; Rn < 16 ; Rn++)
						printf("\tR%d:	0x%04x\n",Rn , LOWWORD(reg_tbl[Rn]));
					debug_opt[R_ALL] = true;
				}
				else{
					Rn = atoi(reg_str);
					printf("\tR%d:	0x%04x\n",Rn ,LOWWORD(reg_tbl[Rn]));
					debug_opt[R_SPEC] = true;
				}
				getchar();
				break;
			case 'm':
			case 'M':	/*memory dump*/
				scanf("%x", &hexval1);
				scanf("%x", &hexval2);
				for( ; hexval1 <= hexval2 ; hexval1++){
					printf("        |____|\n");
					printf("%04x -->| %02x |\n" , hexval1 , mem[hexval1]);
				}
				debug_opt[MEM_DUMP] = true;
				getchar();
				break;
			case 'b':
			case 'B':	/*new breakpoint*/
				scanf("%x" , &brkpt);
				printf("set breakpoint to 0x%04x\n" , brkpt);
				debug_opt[BRKPT] = true;
				break;
			case 'd':
			case 'D':	/*device dump*/
				for(DEVn = 0 ; DEVn < num_dev ; DEVn++){
					printf("DEVICE %d\n" , DEVn);
					printf("  |	PORT:	%d\n" , dev_port[DEVn]);
					printf("  |	SR:	0x%04x\n" , mem[dev_port[DEVn]]);
					if(ISINPUT(dev_port[DEVn]))
						printf("  |	ISR:\t0x%04x\n" , mem[dev_port[DEVn] + 0xFFC0]);
					printf("  |	ToI:	%ld\n" , dev_ToI[DEVn]);
					printf("  |______________________________\n\n");
				}
				break;
			case 't':
			case 'T':	/*terminate*/
				debug_opt[TERM] = true;
				break;
			case '\n':	/*done*/
				done = true;
				break;
			default:
				printf("invalid debugger option\n");
				break;
		}
	} /* opt == '\n' */	
 } 
 
 
/*================================================================================*
 * 	called when the user has elected to use the Watch or Auto debug features.	  *
 *	prints out the instruction depending on the type and all relevent registers   *
 * 	and memory space for that instruction										  *
 *================================================================================*/
 void print_inst(void)
 {
 	if(!VAL_INST){	/* did not find a valid instruction when decoding*/
		printf("instruction fault\n");
 	}
 	else if(ALU_t1 != NULL)	/*type 1 instruction was executed*/
 	{
	 	printf("%s\t",instt1_name[INST_T1]);
		if(dst_mode == DIR){
			printf("%s\n" , reg_name[OP_T1]);
			if(debug_opt[AUTO])
				printf("\t%s = 0x%04x\n", reg_name[OP_T1],reg_tbl[OP_T1] );
		}			
		else{	/*addressing mode is not register direct*/
			printf("mem[0x%04x]\n" , reg_tbl[TA]);
			if(debug_opt[AUTO])
				printf("\tmem[0x%04x] = 0x%04x\n",reg_tbl[TA], reg_tbl[TD2]);
		}	 		
	}
	else if(ALU_t2 != NULL) /*type 2 instruction was executed*/
	{
		printf("%s\t" , instt2_name[INST_T2]);
		printf("0x%04x , " , reg_tbl[TD1]);
		if(dst_mode == DIR){
			printf("%s\n" , reg_name[DST_T2]);
			if(debug_opt[AUTO])
				printf("\t%s = 0x%04x\n" , reg_name[DST_T2] , reg_tbl[DST_T2]);	
		}
		else{
			printf("mem[0x%04x]\n" , reg_tbl[TA]);
			if(debug_opt[AUTO])
				printf("\tmem[0x%04x] = 0x%04x" , reg_tbl[TA] , reg_tbl[TD2]);
		}
	}
	else
	{
		if(condition_tbl[COND_T3])
			printf("JMP 0x%04x\n" , reg_tbl[PC]);
		else
			printf("jump condition not met\n");
	}
	
	if(debug_opt[AUTO]){
		printf("\n\tSR = 0x%04x\n" , reg_tbl[SR]);
		printf("\tPC = 0x%04x\n" , reg_tbl[PC]);
		printf("\tclock = %ld\n" , sys_clock);
		printf("---------------------------------------------\n");	
	}	
	
 }

 
 
 
 
 
