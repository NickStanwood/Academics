/*
 *	device.c
 *	Purpose:
 *		The purpose of this source file is to initialize the devices the way the user
 *		wants them. and to handle device interrupts whenver they happen	
 *	Author:	Nick Stnawood
 *	Date:	June 24 - created source file
 *			July 2 	- wrote init_devs() and poll_dev()
 *			July 3	- moved init_dev_SR() code from init_devs() into its own function
 **********************************************************************************/
 
 #include "device.h"
 #include "memory.h"
 #include "reg_table.h"
 #include "CPU.h"
 #include "debugger.h"
 
 #include <string.h>
 #include <stdlib.h>
 
 /*
  *	array of device files to give each devices its own specific file 
  *	this way each device can be more accurately tested in the emulation
  */
 FILE* DEV_f[MAX_DEV] = {NULL};
 
 /*
  *the device port that each device file is associated with
  */
 int dev_port[MAX_DEV] = {0};
 
 /*
  *	array for device Time of Interrupts , initalized to -1 so there will be no device
  *	interrupts if they are not defined by the user
  */
 unsigned long dev_ToI[MAX_DEV] = {-1};
 
 /* defined by the user to set the number of devices attached to the CPU*/
 int num_dev = 0;
 
 /*
  * All devices set as outputs will write to this file. 
  *	Information written to this file includes:
  *				- Device's Port
  *				- The Time of the Interrupt
  *				- The Program Counter
  */
 FILE* DEV_out;
 
 
 
 
 
 
 
/*=================================================================================*
 *	Recieves User input to set up devices how they want							   *
 *	The user can have up to 16 devices attached all with their own device files.   *
 *	default device files are supplied in \devices\DEVn.txt, these can be changed   *
 *	by the user to have specific Time of Interrupt and Device Status Register 	   *
 *	values.																		   *
 *	All output devices write to dev_out.txt 									   *
 *=================================================================================*/
 void init_devs()
 {
 	char dev = 'n';
 	char dflt = 'n';
 	char *dflt_fname = malloc(256);		//append the rest of the file name on
 	char *tmp = malloc(256);			//what gets appended onto dflt_filename
 	int i =0;						
 	
 	printf("run with devices? (y/n): ");
 	scanf(" %c",&dev);
 	if(dev == 'y'){
 		DEV_out = fopen("dev_out.txt", "w");
 		printf("\nhow many devices? ");
 		scanf("%d",&num_dev);
 		if(num_dev > 16){
 			printf("maximum number of devices is 16\n");
 			num_dev = 16;
		}
 		printf("\nrun with default device files? (y/n): ");
 		scanf(" %c", &dflt);
		if(dflt == 'y'){
			strcpy(tmp , "_.txt");
			strcpy(dflt_fname , "devices\\DEV");
			for(i = 0 ; i < num_dev ; i++){
				*tmp = '0' + i; 					
				strcat(dflt_fname , tmp);			//append i.txt onto devices\DEV
				if(!init_dev_SR(dflt_fname , i)){
					printf("error opening file %s\n",dflt_fname);
					i--;
				}
				strcpy(dflt_fname , "devices\\DEV");			//set back to DEV	
			}
		} 
		else{ /*not running with default test files*/
			for(i = 0 ; i < num_dev ; i++){
		 		printf("enter dev file for device %d " , i);
		 		scanf("%s" , tmp);
		 		if(!init_dev_SR(tmp , i)){
		 			printf("\nunable to open file %s\n", tmp);
		 			i--;		//set i back one so they can try again
				} 	 
			} 
 		} /*done loading all device files*/
 	}
 	free(tmp);
 	free(dflt_fname);
 }
/*=================================================================================*
 *	Checks if any device's DBA bit is set										   *
 * 	handles the device interrupt accordingly								       *
 *=================================================================================*/
 void poll_dev(void)
 {
 	int i = 0;
 	int pri_port = 0x0020;	//port with top priority, start value at lowest priority
 	int dev = 0;
	char *val = malloc(7);
 	for( i = 0 ; i < num_dev ; i++){
 		if(ISIE(dev_port[i])){
 			if(ISDBA(dev_port[i]) && (dev_port[i] < pri_port)){
 				pri_port = dev_port[i];	
 				dev = i;
			 }
 		}
	}
	if(pri_port != 0x20){
		if(debug_opt[AUTO] || debug_opt[WATCH])
			printf("interrupt on port 0x%02x\n" , pri_port);
		if(ISINPUT(pri_port)){
			PUSH(PC);									/*	  PC   -> mem[SP] */
			PUSH(SR);									/*	  SR   -> mem[SP] */
			MDB(VECT_TBL+pri_port, &reg_tbl[TD1]);		/*INTR vect-> TD1	  */
			MAB(TD1);									/*		TD1-> MAR	  */
			CTRL.BW = WORD;								/*   WORD  -> CTRL	  */
			CTRL.RW = READ;								/*	 READ  -> CTRL	  */
			BUS();										/* ISR addr-> MDR	  */
			MDB(MDR , &reg_tbl[PC]);					/*   MDR   -> PC	  */
			CIE;										/*	~GIE   -> SR	  */
			C_ON;										/* ~C_OFF  -> SR	  */
			if(debug_opt[WATCH] || debug_opt[AUTO]){
				printf("device SR:	0x%02x%02x\n",mem[pri_port+1],mem[pri_port]);
				printf("ISR addr:	0x%04x", reg_tbl[PC]);
				printf("\n_________________________________\n");
			}
			CLRDBA(pri_port);
			fscanf(DEV_f[dev], "%s" , val);
			dev_ToI[dev] = strtol(val , NULL , 0);
		}
		else{	/*dev is output*/
			fprintf(DEV_out , "interrupt on port:\t0x%02x\n" , pri_port);
		 	fprintf(DEV_out  ,"time of interrupt:\t%d\n", sys_clock);
		 	fprintf(DEV_out , "PC:\t\t\t0x%04x\n" , reg_tbl[PC]);
		 	fprintf(DEV_out , "\n_________________________________\n");
		 	if(debug_opt[WATCH])
		 		scan_debug();
		 	if(debug_opt[AUTO]){
		 		printf("interrupt on port:\t0x%02x\n" , pri_port);
		 		printf("time of interrupt:\t%d\n", sys_clock);
		 		printf("PC:\t\t\t0x%04x\n" , reg_tbl[PC]);	
			}
			CLRDBA(pri_port);
			fscanf(DEV_f[dev], "%s" , val);
			dev_ToI[dev] = strtol(val , NULL , 0);
		} 
 	}
 }
 
/*=================================================================================*
 * This is function is called during the initilzation of a device 				   *
 *	It opens the device specific file and reads in the device port and initial	   *
 *	device Status register														   *
 *=================================================================================*/
 bool init_dev_SR(char *dev_fname , int i)
 {
 	char *val = malloc(256);
 	
 	/*open dev_fname and confirm it is a valid path*/
	if((DEV_f[i] = fopen(dev_fname , "r")) != NULL){
		printf("opened file %s successfully\n",dev_fname);
		
		/*get port number*/
		fscanf(DEV_f[i] , "%s" , val);
		dev_port[i] = strtol(val , NULL , 0);
		
		/*get initial SR value*/
		fscanf(DEV_f[i] , "%s" , val);
		mem[dev_port[i]] = strtol(val , NULL , 0);
		
		/*get time of first interrupt*/
		fscanf(DEV_f[i] , "%s" , val);
		dev_ToI[i] = strtol(val , NULL , 0);
		free(val);
		return true;
	} 	
	else{
		free(val);
		return false;
	}
		
 }
 
 
 
 
 
 
 
 
 
