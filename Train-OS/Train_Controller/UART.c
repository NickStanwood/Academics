/*================================================================================*
 * TITLE:	UART.c																  *
 * PURPOSE:	initializes the uart and processes all information coming from and    *
 * 			going to the uart.													  *
 * GLOBALS:	xmit_busy - true when the uart is currently transmitting	  	  	  *
 * 			test_recv - true when a proper test command has been entered by the   *
 * 						user. (i.e. they selected which test they want to run) 	  *
 * 						only used during kernel initialization				 	  *
 * 			out_queue -	holds all characters that need to be transmitted to	the   *
 * 						terminal												  *
 * 			out_head  -	head of the output queue								  *
 * 			out_tail  - tail of the output queue								  *
 * 																				  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Oct 12, 2016														  *
 * ===============================================================================*/

#include <stdio.h>
#include <string.h>

#include "UART.h"
#include "kernel.h"
#include "processcalls.h"

/* transmit out queue - array of characters to be output to the uart */
char out_queue[UART_QUEUE_SZ];
volatile unsigned int out_head = 1;
volatile unsigned int out_tail = 0;


volatile bool xmit_busy = false;			/* T|F - data sent to terminal	   */
volatile bool test_recv = false;			/* T|F - received test command	   */


/*================================================================================*
 * initializes UART0 and sets the baud rate to 115200.							  *
 * enables the UART interrupts as well as the transmission interrupts.			  *
 * sets the pins for recieve and transmit.										  *
 * ===============================================================================*/
void UART0_Init(void)
{
	/* Initialize UART0 */
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; 	// Enable Clock Gating for UART0
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; 	// Enable Clock Gating for PORTA

	UART0_CTL_R &= ~UART_CTL_UARTEN;      	// Disable the UART

	// Setup the BAUD rate
	UART0_IBRD_R = 8;	// IBRD = int(16,000,000 / (16 * 115,200)) = int(8.68055)
	UART0_FBRD_R = 44;	// FBRD = int(0.68055 * 64 + 0.5) = 44.0552

	UART0_LCRH_R = (UART_LCRH_WLEN_8);	// WLEN: 8, no parity, one stop bit, without FIFOs)

	UART0_CTL_R = UART_CTL_UARTEN;        // Enable the UART and End of Transmission Interrupts

	GPIO_PORTA_AFSEL_R |= EN_RX_PA0 | EN_TX_PA1;    // Enable Receive and Transmit on PA1-0
	GPIO_PORTA_DEN_R |= EN_DIG_PA0 | EN_DIG_PA1;   	// Enable Digital I/O on PA1-0

}


/*================================================================================*
 * indicates to the CPU which device is to interrupt						  	  *
 * ===============================================================================*/
void IntEnable(unsigned long InterruptIndex){
	if(InterruptIndex < 32){
		/* Enable the interrupt in the EN0 Register */
		NVIC_EN0_R |= 1 << InterruptIndex;
	}else{
		/* Enable the interrupt in the EN1 Register */
		NVIC_EN1_R |= 1 << (InterruptIndex - 32);
	}
}


/*================================================================================*
 * Set specified bits for interrupt												  *
 * ===============================================================================*/
void UART0_IntEnable(unsigned long flags){
	UART0_IM_R |= flags;
}


/*================================================================================*
 * Set BYPASS, clear USRSYSDIV and SYSDIV 										  *
 * ===============================================================================*/
void SetupPIOSC(void){
	/* Sets clock to PIOSC (= 16 MHz) */
	SYSCTRL_RCC_R = (SYSCTRL_RCC_R & CLEAR_USRSYSDIV) | SET_BYPASS ;
}


/*================================================================================*
 * takes care of the full initialization proccess for UART0 					  *
 * ===============================================================================*/
void UART_full_init(void){
	/* Initialize UART */
	SetupPIOSC();								// Set Clock Frequency to 16MHz (PIOSC)
	UART0_Init();								// Initialize UART0
	IntEnable(INT_UART0);						// Enable UART0 interrupts
	UART0_IntEnable(UART_INT_RX | UART_INT_TX);	// Enable Rx and Tx interrupts
}


/*================================================================================*
 * creates a string using VT 100 escape codes to place a string in a specific spot*
 * on the screen. calls the print() service call with the newlyt created string as*
 * the argument																	  *
 * returns the column location at the end of the string 						  *
 * ===============================================================================*/
int print_string(char* outstring , int row , int col){
	char new_string[MAX_STR_SZ] = "\x1b[---;---H";
	int i;

	/* enter escape code sequence */
	new_string[2] =  (row / 100) + ITOA_CONV;
	new_string[3] = ((row / 10) % 10) + ITOA_CONV;
	new_string[4] =  (row % 10) + ITOA_CONV;

	new_string[6] =  (col / 100) + ITOA_CONV;
	new_string[7] = ((col / 10) % 10)+ ITOA_CONV;
	new_string[8] =  (col % 10) + ITOA_CONV;

	i = 10;	//accounting for escape code at start of string
	while(*outstring != '\0'){
		new_string[i] = *outstring;
		i++;
		outstring++;
	}

	new_string[i] = '\0';

	print(new_string);

	i = i + col - 10;	//get the new column location

	return i;
}


/*================================================================================*
 * takes a character out of the circular queue - if the queue is empty then it 	  *
 * returns NUL, if not it returns the dequeued character						  *
 * ===============================================================================*/
char dequeue_uart_out(void){
	char deq_char;
	unsigned int temp_tail;

	temp_tail = (out_tail + 1) % UART_QUEUE_SZ;

	/* empty queue */
	if(temp_tail == out_head){
		deq_char = '\0';
	}
	else{
		deq_char = out_queue[temp_tail];
		out_tail = temp_tail;
	}

	return deq_char;
}


/*================================================================================*
 * enters a character into the uart circular queue. 							  *
 * ===============================================================================*/
void enqueue_uart_out(char new_char){
	out_queue[out_head] = new_char;
	out_head = (++out_head) % UART_QUEUE_SZ;
}
