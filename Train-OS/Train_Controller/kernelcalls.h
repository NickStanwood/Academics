/*================================================================================*
 * TITLE:	kernelcalls.h														  *
 * PURPOSE:	this source file contains all of the kernel functions.			 	  *
 * 			this module resides in kernel space and the functions are not 	  	  *
 * 			able to be called by processes									  	  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Nov 09, 2016														  *
 * REVISION:	*
 * ===============================================================================*/

#ifndef KERNELCALLS_H_
#define KERNELCALLS_H_

#include "kernel.h"
#include "debugger.h"

/* kernel calls */
void Kinit(struct kcallargs *kcall);		//initializes the kernel
void Kgetid(struct kcallargs *kcall);		//return the PID of a process
void Knice(struct kcallargs *kcall);		//changes the pri level of a process
void Kterminate(struct kcallargs *kcall);	//deallocates a process' stack and PCB
void Kprint(struct kcallargs *kcall);		//prints to the terminal via uart
void Krun(struct kcallargs *kcall);
void kxmit_spi(struct kcallargs *kcall);	//transmits a packet to the atmel

#ifdef _DEBUG_
void kprint_diag(struct kcallargs *kcall);//prints a diagnostic to the debug window
#endif 	/* _DEBUG */

#endif 	/* KERNELCALLS_H_ */
