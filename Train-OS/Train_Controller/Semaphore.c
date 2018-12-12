
/*
 * Binary semaphore example using
 * LDR and STR exclusive instructions
 * ECED 4402
 * 5 March 2012 - Code
 * 13 November 2016 - Additional comments
 */


#include "kernelcalls.h"
#include "kernel.h"
#include "UART.h"
#include "mem_manager.h"
#include "Systick.h"
#include "train_control.h"
#include "train_display.h"
#include "gpiof.h"
#include "spi.h"
#include "debugger.h"

#include <stdio.h>
#define OPEN	0
#define CLOSED	1
#define SEMA_MAX 20

unsigned int semaphore = OPEN;

struct semaphore_build {
	struct pcb *head;
	struct pcb *tail;
	int semaphore_lock;
};

struct semaphore_build semaphore_table[SEMA_MAX];

void kwait(struct kcallargs *kcall)
{
	if ( kcall->arg1 <= SEMA_MAX) {
		kcall->rtnvalue = false;
	}

	if ( semaphore_table[kcall->arg1].semaphore_lock == OPEN) {
		semaphore_table[kcall->arg1].semaphore_lock = CLOSED;
		kcall->rtnvalue = true;
	}
	else {
		/* Closed, add to Waiting to run for that semaphore */
		dequeue_pcb(running,running->priority);
		if ( semaphore_table[kcall->arg1].head == NULL ) {
			/* First to request specified semaphore */
			semaphore_table[kcall->arg1].head = running;
		}
		running->next = NULL;
		semaphore_table[kcall->arg1].tail->next = running;
		running->prev = semaphore_table[kcall->arg1].tail;
		semaphore_table[kcall->arg1].tail = running;
		NVIC_INT_CTRL_R |= TRIGGER_PENDSV;	//trigger pendSV

	}

}

void  kpost(struct kcallargs *kcall)
{
	struct pcb *post_pcb; // pcb used to save pcb added back from sema to wtr queue
	/* See if proper semaphore value */
	if (kcall->arg1 <= SEMA_MAX) {
		kcall->rtnvalue = false;
	}
	/* See if lock already open */
	if ( semaphore_table[kcall->arg1].semaphore_lock == OPEN) {
		kcall->rtnvalue = true; // already open
	}
	/* Check if semphore pcb queue head is null */
	else if ( semaphore_table[kcall->arg1].head == NULL) {
		semaphore_table[kcall->arg1].semaphore_lock = OPEN;
	}
	/* We know Head is not NULL */
	else {
		post_pcb = semaphore_table[kcall->arg1].head;
		semaphore_table[kcall->arg1].head = semaphore_table[kcall->arg1].head->next;
		semaphore_table[kcall->arg1].head->prev = NULL;
		if ( semaphore_table[kcall->arg1].head == NULL ) {
			semaphore_table[kcall->arg1].tail = NULL;
		}
		enqueue_pcb(post_pcb,post_pcb->priority);
	}

}
