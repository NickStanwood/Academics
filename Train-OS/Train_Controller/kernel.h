/*================================================================================*
 * TITLE:	kernel.h															  *
 * PURPOSE:	This header file #defines everything related to to the kernel and 	  *
 * 			declares structs												  	  *
 * STRUCTS:	stack_frame - a typical stack frame with H/W pushed registers and S/W *
 * 						  pushed registers										  *
 * 			pcb - contains the unique PID for the process it contains and the 	  *
 * 				 stack pointer for that process. it also contains the next and    *
 * 				 previous pcb in the queue										  *
 * 			kcallargs - used to pass arguments to the kernel. 					  *
 * 																				  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Oct 12, 2016														  *
 * ===============================================================================*/

#ifndef KERNEL_H_
#define KERNEL_H_

#include "spi.h"
#include "debugger.h"

typedef unsigned int bool;
enum {false,true};

enum kernelcallcodes {
	GETID,    		//returns the PID of the process
	NICE , 			//changes the priority of the processes
	TERMINATE, 		// deallocates the running processes
	PRINT, 			//adds a string to the uart output queue
	INIT , 			//initializes the kernel
	RUN  ,			//begins running the first processes
	XMIT_SPI , 		//transmits a packet to the atmel
#ifdef _DEBUG_
	PRINT_DIAG, 	//prints a diagnostic to the debug window
#endif
	SEND,			// Sending interprocess packets
	RECV,			// Receiving interprocess packets
	BIND			// Bind socket to process
};

enum nicecmds {
	PRI0 , 			//move to priority 0 	(highest priority)
	PRI1 , 			//move to priority 1
	PRI2 , 			//move to priority 2
	PRI3 , 			//move to priority 3	(lowest priority)
	INC  , 			//increase to the next highest priority ( 2 -> 1 )
	DEC  , 			//decrease to the next lowest priority  (2 -> 3)
	GUCPU			//give up the cpu (preempt itself)
};

#define SVC()		__asm(" SVC	#0")
#define disable()	__asm(" cpsid i")
#define enable() 	__asm(" cpsie i")

#define MSP_RETURN 0xFFFFFFF9 	 //LR value: exception return using MSP as SP
#define PSP_RETURN 0xFFFFFFFD 	 //LR value: exception return using PSP as SP

#define STACKSIZE	1024
#define MAX_QUEUE_SZ 32

#define NVIC_SYS_PRI3_R (*((volatile unsigned long *) 0xE000ED20))
#define PENDSV_LOWEST_PRIORITY 0x00E00000

#define NVIC_INT_CTRL_R (*((volatile unsigned long *) 0xE000ED04))
#define TRIGGER_PENDSV 0x10000000


#define MAX_NUM_PRI 5
#define NUM_KCALLS 4

/* Cortex default stack frame */
struct stack_frame
{
	/* Registers saved by s/w (explicit) */
	/* There is no actual need to reserve space for R4-R11, other than
	 * for initialization purposes.  Note that r0 is the h/w top-of-stack.
	 */
	unsigned long r4;
	unsigned long r5;
	unsigned long r6;
	unsigned long r7;
	unsigned long r8;
	unsigned long r9;
	unsigned long r10;
	unsigned long r11;
	/* Stacked by hardware (implicit)*/
	unsigned long r0;
	unsigned long r1;
	unsigned long r2;
	unsigned long r3;
	unsigned long r12;
	unsigned long lr;	/* Address of process-terminate function */
	unsigned long pc;  	/* Address of process entry point */
	unsigned long xpsr;	/* Initialize to 0x01000000 */
};

/* Process control block */
struct pcb
{
	/* Stack pointer - r13 (PSP) */
	unsigned long sp;
	unsigned long stack_entry;
	/* Links to adjacent PCBs */
	struct pcb *next;
	struct pcb *prev;
	int PID;
	int priority;
	int binded_socket;
};

struct kcallargs{
	unsigned int code;
	unsigned int rtnvalue;
	unsigned int arg1;
	unsigned int arg2;
};


/* the current running process */
extern struct pcb *running;

/* waiting to run priority queues */
extern struct pcb *wtr[];

/* keeping track of the PID that are already given out */
extern int PID_count;

/* current priority of the running process */
extern volatile int cur_priority;

/* allocates a stack and a PID to a process */
int create_proc(void (*funcptr)() , int priority);

/* sets runnign to next PCB */
void get_new_proc(void);

/* initializes the kernel */
void Kernel_Initialization(void);

/* lowest priority process - runs when nothing else is ready */
void idle_process(void);

/* recieves on the error socket - wakes up when an error is detected */
void error_process(void);

/* adds PCB to WTR[] queue */
void enqueue_pcb(struct pcb *proc, int pri);

/* removes PCB from WTR[] queue */
void dequeue_pcb(struct pcb *proc, int pri);

/* used during preemption for saving and restoring state */
void save_registers();
void restore_registers();

/* special register functions */
void set_LR(volatile unsigned long);
unsigned long get_PSP();
void set_PSP(volatile unsigned long);
unsigned long get_MSP(void);
void set_MSP(volatile unsigned long);
unsigned long get_SP();
void assignR7(volatile unsigned long);


#endif /* KERNEL_H_ */
