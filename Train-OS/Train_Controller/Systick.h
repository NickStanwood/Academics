/*================================================================================*
 * TITLE:	Systick.h															  *
 * PURPOSE:	defines all registers pertaining to the clock						  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Oct 12, 2016														  *
 * ===============================================================================*/


#ifndef SYSTICK_H_
#define SYSTICK_H_

/* SysTick NVIC Registers */
 // SysTick Control and Status Register (STCTRL)
#define NVIC_ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))
// Systick Reload Value Register (STRELOAD)
#define NVIC_ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))

			/* SysTick defines */

#define NVIC_ST_CTRL_COUNT      0x00010000  // Count Flag for STCTRL
#define NVIC_ST_CTRL_CLK_SRC    0x00000004  // Clock Source for STCTRL
#define NVIC_ST_CTRL_INTEN      0x00000002  // Interrupt Enable for STCTRL
#define NVIC_ST_CTRL_ENABLE     0x00000001  // Enable for STCTRL

#define TIME_QUANTUM 			100

/* various periods for SysTick increment */
#define CLK_PERIOD_100ns	16042
#define CLK_PERIOD_100ms	1604150
#define MAX_CLK_PERIOD 0x00FFFFFF			//max value for reload register
extern unsigned volatile long ToD;

/* sets the clock source to internal and enables the counter */
void SysTickStart(void);

/* clear the enable bit to stop the counter */
void SysTickStop(void);

/* sets the reload value of SYSTICK in order to customize the period of SYSTICK	*/
void SysTickPeriod(unsigned long Period);


/* sets the interrupt bit in the SYSTICK CNTRL register */
void SysTickIntEnable(void);


/* clears the interrupt bit in the SYSTICK CNTRL register */
void SysTickIntDisable(void);

/* function thats called by control after being put in the que from the SysTickHandler */
void SysTickApplication(char discard);


void systick_init(void);

void delay(volatile int time);

#endif /* SYSTICK_H_ */
