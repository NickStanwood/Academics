/*================================================================================*
 * TITLE:	processcalls.h														  *
 * PURPOSE:	contains all the calls made by processes that request the kernel to   *
 * 			perform a service for them - this module resides in pspace			  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Nov 17, 2016														  *
 * REVISION:	*
 * ===============================================================================*/

#ifndef PROCESSCALLS_H_
#define PROCESSCALLS_H_

#include "kernel.h"
#include "debugger.h"
#include "spi.h"

/* service calls (called by processes) */
int getid(void);			//return the PID ofthe processes
void terminate(void);		//removes processes and deallocates stack and PCB
void nice(enum nicecmds);	//moves the priority of the process
void print(char* string);	//prints a message to the terminal
unsigned xmit_spi_pkt(struct spi_pkt packet);	//transmits a command to the atmel

extern int recv(unsigned, unsigned *, void *, unsigned);
extern int send(unsigned, unsigned, void *, unsigned);
extern int bind(unsigned);


#ifdef _DEBUG_
void print_diag(int diagnostic , struct spi_pkt resp_pkt);	//prints an error diagnostic
#endif 	/* _DEBUG_ */

#endif /* PROCESSCALLS_H_ */
