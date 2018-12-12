/*================================================================================*
 * TITLE:	intr_handlers.c														  *
 * PURPOSE:	any time an exception is called, whether it is a software trap or 	  *
 * 			uart interrupt, the function is in this source file. contains the 	  *
 * 			systick ISR, UART0 ISR, SVC and PendSV							  	  *
 * AUTHOR:	Victor Maslov, Nick Stanwood									  	  *
 * CREATED:	Oct 15, 2016													  	  *
 * REVISION:Nov 09, 2016 - added the GPIOF handler to receive calls from the Atmel*
 * ===============================================================================*/

#include <stdio.h>

#include "Systick.h"
#include "UART.h"
#include "kernel.h"
#include "kernelcalls.h"
#include "intr_handlers.h"
#include "mem_manager.h"
#include "gpiof.h"
#include "train_control.h"
#include "processcalls.h"
#include "msg_queue.h"

/* list of kernel call function pointers for SVChandler() to call */
 void (*kcall_list[])(struct kcallargs *args) = {
		Kgetid,
		Knice,
		Kterminate,
		Kprint,
		Kinit,
		Krun,
		kxmit_spi,
#ifdef _DEBUG_
		kprint_diag,
#endif
 	 	k_send_message,
 	 	k_recv_message,
 	 	k_bind_message
};

/*================================================================================*
 * enqueues the systick application function into the control queue for the kernel*
 * to handle in the main function												  *
 * ===============================================================================*/
void SysTickHandler(void)
{
	ToD++;		//counter for delay function
	if(ToD % TIME_QUANTUM == 0){
		NVIC_INT_CTRL_R |= TRIGGER_PENDSV;	//trigger pendSV
	}

}

/*================================================================================*
 * enqueues the UART0 application function into the control queue for the kernel  *
 * to handle in the main function												  *
 * ===============================================================================*/
void UART0_IntHandler(void){

	char temp_char;

	if (UART0_MIS_R & UART_INT_RX){
		UART0_ICR_R |= UART_INT_RX;		// RECV done - clear interrupt
		//temp_char = UART0_DR_R;
		//UARTrecv(temp_char);			//not currently implemented
	}

	if (UART0_MIS_R & UART_INT_TX){
		UART0_ICR_R |= UART_INT_TX;		// XMIT done - clear interrupt
		temp_char = dequeue_uart_out();
		if(temp_char != '\0'){
			UART0_DR_R = temp_char;
		}else{
			xmit_busy = false;
		}
	}
}

/*================================================================================*
 * Supervisor call entry point													  *
 * Using MSP - trapping process either MSP or PSP (specified in LR)				  *
 * Source is specified in LR: F9 (MSP) or FD (PSP)								  *
 * Save r4-r11 on trapping process stack (MSP or PSP)							  *
 * Restore r4-r11 from trapping process stack to CPU							  *
 * SVCHandler is called with r0 equal to MSP or PSP to access any arguments		  *
 * ===============================================================================*/
void SVCall(void) {
	/* Save LR for return via MSP or PSP */
	__asm(" 	PUSH 	{LR}");

	/* Trapping source: MSP or PSP? */
	__asm(" 	TST 	LR,#4");	/* Bit 4: MSP (0) or PSP (1) */
	__asm(" 	BNE 	RtnViaPSP");	/* EQ zero - MSP; NE zero - PSP */

	/* Trapping source is MSP - save r4-r11 on stack (default, so just push) */
	__asm(" 	PUSH 	{r4-r11}");
	__asm(" 	MRS	r0,msp");
	__asm(" 	BL	SVCHandler");	/* r0 is MSP */
	__asm(" 	POP	{r4-r11}");
	__asm(" 	POP 	{PC}");

	/* Trapping source is PSP - save r4-r11 on psp stack (MSP is active stack) */
	__asm("RtnViaPSP:");
	__asm(" 	mrs 	r0,psp");
	__asm("		stmdb 	r0!,{r4-r11}");	/* Store multiple, decrement before */
	__asm("		msr	psp,r0");
	__asm(" 	BL	SVCHandler");	/* r0 Is PSP */

	/* Restore r4..r11 from trapping process stack  */
	__asm(" 	mrs 	r0,psp");
	__asm("		ldmia 	r0!,{r4-r11}");	/* Load multiple, increment after */
	__asm("		msr	psp,r0");
	__asm(" 	POP 	{PC}");

}


/*================================================================================*
 * Supervisor call handler														  *
 * Handle startup of initial process											  *
 * Handle all other SVCs such as getid, terminate, etc.							  *
 * Assumes first call is from startup code										  *
 * Argptr points to (i.e., has the value of) either:							  *
 * 	   - the top of the MSP stack (startup initial process)						  *
 * 	   - the top of the PSP stack (all subsequent calls)						  *
 * Argptr points to the full stack consisting of both hardware and software		  *
 * register pushes (i.e., R0..xPSR and R4..R10); this is defined in type		  *
 * stack_frame																	  *
 * Argptr is actually R0 -- setup in SVCall(), above.							  *
 * Since this has been called as a trap (Cortex exception), the code is in		  *
 * Handler mode and uses the MSP												  *
 * ===============================================================================*/
void SVCHandler(struct stack_frame *argptr) {
	struct kcallargs *kcaptr;
	/*
	 * kcaptr points to the arguments associated with this kernel call
	 * argptr is the value of the PSP (passed in R0 and pointing to the TOS)
	 * the TOS is the complete stack_frame (R4-R10, R0-xPSR)
	 */
		kcaptr = (struct kcallargs *) argptr -> r7;
		kcall_list[kcaptr->code](kcaptr);
		
}


/*================================================================================*
 * lowest priority interrupt - called from systick to do a preemption on the 	  *
 * currently running process. pendSV is used opposed to doing the preemption 	  *
 * directly in systick to avoid preemption happening in the middle of an interrupt*
 * ===============================================================================*/
void PendSV(void){
	disable();
	save_registers();
	running->sp = get_PSP();
	get_new_proc();
	enable();
}

/*================================================================================*
 * Pin 0 on PortF indicates that a hall sensor has been triggered	 			  *
 * pin 1 on ProtF indicates an error has occured								  *
 * Configured for Rising Edge triggered interrupts on Pins 0 and 1				  *
 * ===============================================================================*/
void GPIOPortF_IntHandler(void)
{
	struct kcallargs interrupt_arg;
	struct msg_packet interrupt_packet;

	interrupt_arg.arg1 = (unsigned long)&interrupt_packet;
	interrupt_packet.size = sizeof(int);

	// Clear the two interrupts
	PORTF_GPIOICR_R -> IC |= (1<<BIT0) | (1<<BIT1);


	if((PORTF_GPIODR_R -> DATA & 0x01) == 0x01)
	{
		interrupt_packet.dst = HAL_SOCKET;
		interrupt_packet.payload[0] = WAKEUP;
		k_send_message(&interrupt_arg);
	}
	else if((PORTF_GPIODR_R -> DATA & 0x02) == 0x02 ){
		interrupt_packet.dst = XMIT_SOCKET;
		interrupt_packet.payload[0] = (char)TX_ERR;
		k_send_message(&interrupt_arg);
	}


}

