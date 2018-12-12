/*================================================================================*
 * TITLE:	processcalls.c														  *
 * PURPOSE:	contains all the calls made by processes that request the kernel to   *
 * 			perform a service for them - this module resides in Pspace			  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Nov 17, 2016														  *
 * REVISION:Nov 21, 2016 - took out all the delays that were put after print funcs*
 * 						   and placed it directly in the print function 		  *
 * ===============================================================================*/

#include "processcalls.h"
#include "kernel.h"
#include "spi.h"
#include "Systick.h"
#include "train_control.h"

/* kernel call - gets id of current process */
int getid(){
	volatile struct kcallargs getidarg;
	getidarg.code = GETID;

	assignR7((unsigned long) &getidarg);
	SVC();

	return getidarg.rtnvalue;
}


/* kernel call - changes current process' priority */
void nice(enum nicecmds command){
	volatile struct kcallargs nicearg;
	nicearg.code = NICE;
	nicearg.arg1 = command;

	assignR7((unsigned long) &nicearg);
	SVC();
}


/* kernel call - terminates current process */
void terminate(void){
	volatile struct kcallargs termarg;
	termarg.code = TERMINATE;

	assignR7((unsigned long) &termarg);
	SVC();
}


/* kernel call - adds characters of string to output queue */
void print(char *string){
	volatile struct kcallargs printarg;
	printarg.code = PRINT;
	printarg.arg1 = (int)string;

	assignR7((unsigned long) &printarg);
	SVC();
	delay(16);	//small delay to let uart catch up
}

/* kernel call - adds diagnostic string to output queue */
void print_diag(int diagnostic , struct spi_pkt resp_pkt){
	volatile struct kcallargs printarg;
	printarg.code = PRINT_DIAG;
	printarg.arg1 = diagnostic;
	printarg.arg2 = (int)&resp_pkt;
	assignR7((unsigned long) &printarg);
	SVC();
	delay(16);
}

/* kernel call - transmits a 5 byte packet to the atmel via the SPI */
unsigned xmit_spi_pkt(struct spi_pkt packet)
{
	volatile struct kcallargs xmitarg;

	packet.start_byte = START_SPI;
	packet.end_byte = END_SPI;

	xmitarg.code = XMIT_SPI;
	xmitarg.arg1 = (unsigned long)&packet;
	assignR7((unsigned long) &xmitarg);
	SVC();

	return xmitarg.rtnvalue;
}
