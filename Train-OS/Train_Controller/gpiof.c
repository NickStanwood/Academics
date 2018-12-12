/*================================================================================*
 * TITLE:	gpiof.c																  *
 * PURPOSE:	initializes interrupts on rising edges on pins F0 and F1 			  *
 * AUTHOR:	Dr. Larry Hughes												  	  *
 * CREATED:	Nov 09, 2016														  *
 * REVISION:	*
 * ===============================================================================*/

#include "gpiof.h"
#include "sysctl.h"
//#include "queue.h"
//#include "hex.h"

struct PORTF_GPIODR * const PORTF_GPIODR_R = (struct PORTF_GPIODR *) 0x400253FC;
struct PORTF_GPIODIR * const PORTF_GPIODIR_R = (struct PORTF_GPIODIR *) 0x40025400;

// Interrupt Sense Registers
struct PORTF_GPIOIS * const PORTF_GPIOIS_R = (struct PORTF_GPIOIS *) 0x40025404;	
struct PORTF_GPIOIEV * const PORTF_GPIOIEV_R = (struct PORTF_GPIOIEV *) 0x4002540C;
struct PORTF_GPIOIM * const PORTF_GPIOIM_R = (struct PORTF_GPIOIM *) 0x40025410;
struct PORTF_GPIOICR * const PORTF_GPIOICR_R = (struct PORTF_GPIOICR *) 0x4002541C;

struct PORTF_GPIOAFSEL * const PORTF_GPIOAFSEL_R = (struct PORTF_GPIOAFSEL *) 0x40025420;
struct PORTF_GPIODEN * const PORTF_GPIODEN_R = (struct PORTF_GPIODEN *) 0x4002551C;
struct PORTF_GPIOPCTL * const PORTF_GPIOPCTL_R = (struct PORTF_GPIOPCTL *) 0x4002552C;


void PORTF_Init(void)
{
	volatile unsigned wait;
	// Activate Port F
	SYSCTL_RCGC2_R -> GPIOF = ACTIVATE;
	
	wait = 0;	// wait for PORTF to activate
	
	// Make PF0 and PF1 Inputs
	PORTF_GPIODIR_R -> DIR	&= ~(1<<BIT0);
	PORTF_GPIODIR_R -> DIR	&= ~(1<<BIT1);
	
	// Enable Digital I/O
	PORTF_GPIODEN_R -> DEN |= (1<<BIT0) | (1<<BIT1);
	
	// Edge Sensitive Interrupt
	PORTF_GPIOIS_R -> IS &= ~(1<<BIT0);
	PORTF_GPIOIS_R -> IS &= ~(1<<BIT1);
	
	
	// Rising Edge Sensitive
	PORTF_GPIOIEV_R -> IEV |= (1<<BIT0) | (1<<BIT1);
	
	
	// Both Edge Sensitive
	//PORTF_GPIOIBE_R -> IBE |= (1<<BIT0) | (1<<BIT1);		PORT UNDEFINED?
	
	// Clear Pin Interrupts
	PORTF_GPIOICR_R -> IC |= (1<<BIT0) | (1<<BIT1);
	
	// Enable Pin Interrupts
	PORTF_GPIOIM_R -> IME |= (1<<BIT0) | (1<<BIT1);

	//Enable PortF interrupts on the NVIC
	NVIC_EN0_R |= (1 << NVIC_PORTF_PIN);
}	


