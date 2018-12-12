/*================================================================================*
 * TITLE:		kernel.c														  *
 * PURPOSE:		contains waiting to run processes in an array of linked lists 	  *
 * 				which are organized by priority. Also has functions to preempt 	  *
 * 				processes and create new processes								  *
 *																				  *
 * GLOBALS:	running - points to the currently running PCB						  *
 * 			wtr[]	- array of linked lists for the waiting to run PCBs			  *
 * 			PID_count - used to keep track of the already-in-use PIDs			  *
 * 			cur_priority - the priority of the currently running process.		  *
 * 						   initialized to the lowest priority					  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Oct 12, 2016														  *
 * REVISION:Nov 09, 2016 - moved all kernel calls into their own module		  	  *
 * 			Nov 17, 2016 - moved all process calls into their own module		  *
 * ===============================================================================*/

#include <stdio.h>

#include "kernel.h"
#include "mem_manager.h"
#include "UART.h"
#include "Systick.h"
#include "kernelcalls.h"
#include "processcalls.h"
#include "gpiof.h"
#include "train_control.h"
#include "msg_queue.h"

struct pcb *running;						// the current running process
struct pcb *wtr[MAX_NUM_PRI] = {0};			// waiting to run priority queues

int PID_count = 0;							// counter for used PIDs
volatile int cur_priority = MAX_NUM_PRI;	// current priority of the running process

/*================================================================================*
 * calls the initialization code via an SVC call and then begins running 		  *
 * processes by calling the run code											  *
 * ===============================================================================*/
void Kernel_Initialization(void)
{
	volatile struct kcallargs startuparg;

	startuparg.code = INIT;
	assignR7((unsigned long) &startuparg);
	SVC();

	/* UART recv is available to use here is any user information   *
	 * is needed during the initialization process 				    */

	startuparg.code = RUN;
	assignR7((unsigned long) &startuparg);
	SVC();
}


/*================================================================================*
 * allocates a stack and a PID to a process, then adds the new PID to a WTR queue *
 * returns a 1 on successful creation.											  *
 * returns a 0 on failure														  *
 * ===============================================================================*/
int create_proc(void (*funcptr)() , int priority)
{
	int stack_top;
	struct stack_frame *new_stack = NULL;
	struct pcb *new_proc;
	bool success = false;

	if(priority < MAX_NUM_PRI){
		new_proc = (struct pcb*)allocate(sizeof(struct pcb));

		stack_top = allocate(STACKSIZE);
		new_proc->stack_entry = stack_top;

		if(stack_top != -1){
			/* get proper stack location */
			new_proc->sp = stack_top + STACKSIZE - sizeof(struct stack_frame);
			new_stack = (struct stack_frame*)new_proc->sp;
			new_proc->PID = PID_count;
			new_proc->priority = priority;
			new_proc->binded_socket = -1; /* Non-valid value for socket */

			PID_count++;

			/* set initial register values */
			new_stack->pc   = (unsigned long)funcptr;
			new_stack->xpsr = 0x01000000;
			new_stack->r0   = 0x00000000;
			new_stack->r1   = 0x11111111;
			new_stack->r2   = 0x22222222;
			new_stack->r3   = 0x33333333;
			new_stack->r4   = 0x44444444;
			new_stack->r5   = 0x55555555;
			new_stack->r6   = 0x66666666;
			new_stack->r7   = 0x77777777;
			new_stack->r8   = 0x88888888;
			new_stack->r9   = 0x99999999;
			new_stack->r10  = 0xaaaaaaaa;
			new_stack->r11  = 0xbbbbbbbb;
			new_stack->r12  = 0xcccccccc;

			enqueue_pcb(new_proc , priority);

			success = true;
		}
	}

	return success;
}

/*================================================================================*
 * finds the next PID with the highest priority and restores the state	of it 	  *
 * from the new stack pointer													  *
 * ===============================================================================*/
void get_new_proc(void)
{
	int i;

	for(i = PRI0 ; i < MAX_NUM_PRI ; i++){
		if(wtr[i] != NULL){
			running = wtr[i];
			wtr[i] = wtr[i]->next;
			cur_priority = i;
			set_PSP(running->sp);
			restore_registers();
			break;
		}
	}
}


/*================================================================================*
 * this process is initialized in the lowest priority queue all by itself 		  *
 * and prints out a spinner in the top right corner when its running			  *
 * ===============================================================================*/
void idle_process(void)
{
	int col = TERM_WIDTH - 8;
	int row = 1;

	char time[] = "--:--:--";
	int temp;

	while(1){
		if(ToD % 1000 == 0){
			temp = ToD/1000;
			time[0] = (temp /36000) % 02 + ITOA_CONV;
			time[1] = (temp / 3600) % 04 + ITOA_CONV;
			time[3] = (temp / 600 ) % 06 + ITOA_CONV;
			time[4] = (temp / 60  ) % 10 + ITOA_CONV;
			time[6] = (temp / 10  ) % 06 + ITOA_CONV;
			time[7] =        (temp) % 10 + ITOA_CONV;

			print_string(time , row , col);
		}
	}
}



/*================================================================================*
 * adds the designated PCB to the priority queue specified						  *
 * ===============================================================================*/
void enqueue_pcb(struct pcb *proc, int pri)
{
	/* empty queue */
	if(wtr[pri] == NULL){
		wtr[pri] = proc;
		wtr[pri]->next = proc;
		wtr[pri]->prev = proc;
	}
	else{
		/* link PCB into queue */
		proc->prev = wtr[pri]->prev;
		(proc->prev)->next = proc;
		wtr[pri]->prev = proc;
		proc->next = wtr[pri];
	}
}


/*================================================================================*
 * dequeues a process from a priority queue - used for terminating a process	  *
 * ===============================================================================*/
void dequeue_pcb(struct pcb *proc, int pri)
{

	/* only process in queue*/
	if(proc->next == proc){
		wtr[pri] = NULL;
		wtr[pri]->next = NULL;
		wtr[pri]->prev = NULL;
	}
	else{
		(proc->next)->prev = proc->prev;
		(proc->prev)->next = proc->next;
	}

}

/* saves R4 through R11 on the current process' stack */
void save_registers()
{
	__asm("	mrs r0,psp");
	__asm("	stmdb r0!,{r4-r11}");	// Store multiple, decrement before
	__asm("	msr	psp,r0");
}

/* restores R4 through R11 on the current process' stack */
void restore_registers()
{
	__asm("	mrs	r0,psp");
	__asm("	ldmia r0!,{r4-r11}");	// Load multiple, increment after
	__asm("	msr	psp,r0");
}

/* returns the current process' stack */
unsigned long get_PSP(void)
{
	__asm("	mrs r0, psp");
	__asm("	bx lr");
	return 0;
}

/* Returns contents of MSP (main stack) */
unsigned long get_MSP(void)
{
	__asm("	mrs r0, msp");
	__asm("	bx lr");
	return(0);
}

/* set PSP to ProcessStack */
void set_PSP(volatile unsigned long ProcessStack)
{
	__asm("	msr	psp, r0");
}

/* Set MSP to MainStack */
void set_MSP(volatile unsigned long MainStack)
{
	__asm("	msr	msp, r0");
}

/* Set MSP to MainStack */
unsigned long get_SP()
{
	__asm(" 	mov r0,SP");
	__asm("		bx lr");
	return 0;
}


/* assigns data to R7 - used to pass arguments for kernel calls */
void assignR7(volatile unsigned long data)
{
	 __asm("	mov r7,r0");

}
