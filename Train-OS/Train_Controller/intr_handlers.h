/*================================================================================*
 * TITLE:		intr_handlers.h													  *
 * PURPOSE:		declares all interrupt handlers									  *
 * AUTHOR:		Victor Maslov, Nick Stanwood									  *
 * CREATED:		Oct 15, 2016													  *
 * ===============================================================================*/


#ifndef INTR_HANDLERS_H_
#define INTR_HANDLERS_H_

#include "kernel.h"

/* triggers the PendSV handler */
void SysTickHandler(void);

/* handles transmission and receiving characters from the UART */
void UART0_IntHandler(void);

/* a trap called by processes - finds which kernel call the process made */
void SVCall(void);
void SVCHandler(struct stack_frame *argptr);

/* preempts a process - lowest priority interrupt */
void PendSV(void);

/* interrupt from the Atmel for Hall Sensor activations */
void GPIOPortF_IntHandler(void);

#endif /* INTR_HANDLERS_H_ */
