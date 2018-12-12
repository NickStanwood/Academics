/*
 *	device.h
 *	Purpose:
 *		The purpose of this source file is to initialize the devices the way the user
 *		wants them. and to handle device interrupts whenver they happen	
 *	Definitions:		
 *	Author:	Nick Stnawood
 *	Date:	June 24 - created header file
 *			July 3  - created a function for init_dev_SR() seperate from init_dev()
 **********************************************************************************/
 #ifndef DEVICE_H
 #define DEVICE_H
 
 #include <stdio.h>
 #include "reg_table.h"
 
 #define MAX_DEV 16
 #define VECT_TBL 0xFFC0
 
 #define ISIE(x)	(mem[(x)] & 0x0001)
 #define ISINPUT(x) (mem[(x)] & 0x0002)
 #define ISDBA(x)	(mem[(x)] & 0x0004)
 #define CLRDBA(x)	(mem[(x)] = mem[(x)] & ~0x0004)
 
 extern FILE* DEV_f[];
 extern FILE* DEV_out;
 
 /*	the current State of a device read in from it's respective device file.*/
 extern int dev_port[];
 
 /*	the next Time of Interrupt for each device, checked everytime the sys_clock
  *	is incremented */
 extern unsigned long dev_ToI[];
 
 /*the number of devices hooked up the the CPU*/
 extern int num_dev;

/*	init_devs()
 *	asks for user input on how many devices are attached to the CPU and what
 *	their file names are. an option for default file names is also available
 *	(DEV1.txt , DEV2.txt , etc.)
 */
 extern void init_devs();
 
/*	init_dev_SR()
 *	reads in the initial dev information including the port it is connected to
 *	as well as the initial SR value and the time of the first interrupt
 */
 extern bool init_dev_SR(char *dev_fname , int i);
 
/*	poll_dev()
 *	checks all devices attached devices to see if their DBA bit is set
 */
 extern void poll_dev(void);
 #endif
