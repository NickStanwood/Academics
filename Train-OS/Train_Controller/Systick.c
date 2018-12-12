/*================================================================================*
 * TITLE:	Systick.c														  	  *
 * PURPOSE:	Sets up the clock and provides functions for disabling and enabling it*
 * GLOBALS:	ToD	- increased at every SysTick Interrupt, when it matches the time  *
 * 				  quantum for the process it will preempt the current process	  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Oct 12, 2016														  *
 * ===============================================================================*/

#include "Systick.h"
#include "kernel.h"

unsigned volatile long ToD = 0;	//counter for delay function


/*================================================================================*
 * does a full initialization of the system clock. keeps the systick interrupt	  *
 * disabled until the kernel is in the running state 							  *
 * ===============================================================================*/
void systick_init(void)
{
	SysTickPeriod(CLK_PERIOD_100ns);
	SysTickIntDisable();
	SysTickStart();
}

/*================================================================================*
 * sets the clock source to internal and enables the counter					  *
 * ===============================================================================*/
void SysTickStart(void)
{
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE;
}


/*================================================================================*
 * clear the enable bit to stop the counter									  	  *
 * ===============================================================================*/
void SysTickStop(void)
{
	// Clear the enable bit to stop the counter
	NVIC_ST_CTRL_R &= ~(NVIC_ST_CTRL_ENABLE);
}


/*================================================================================*
 * sets the reload value of SYSTICK in order to customize the period of SYSTICK	  *
 * the value passed must be between 0 and 16 777 216						 	  *
 * ===============================================================================*/
void SysTickPeriod(unsigned long Period)
{
	NVIC_ST_RELOAD_R = Period - 1;
}


/*================================================================================*
 * sets the interrupt bit in the SYSTICK CNTRL register							  *
 * ===============================================================================*/
void SysTickIntEnable(void)
{
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_INTEN;
}

/*================================================================================*
 * clears the interrupt bit in the SYSTICK CNTRL register						  *
 * ===============================================================================*/
void SysTickIntDisable(void)
{
	// Clear the interrupt bit in STCTRL
	NVIC_ST_CTRL_R &= ~(NVIC_ST_CTRL_INTEN);
}


/*================================================================================*
 * delays for a certain amount of time based on the passed argument.			  *
 * an argument of 1 will result in ~100ns delay									  *
 * ===============================================================================*/
void delay(volatile int time)
{
	time += ToD;
	while(ToD <= time){
	}
	return;
}


