/*================================================================================*
 * TITLE:	gpiof.h																  *
 * PURPOSE:	defines all the structs related to the portF interrupts 			  *
 * AUTHOR:	Dr. Larry Hughes													  *
 * CREATED:	Now 09, 2016														  *
 * REVISION:	*
 * ===============================================================================*/
#ifndef GPIOF_H_
#define GPIOF_H_

/* Interrupt 0-31 Set Enable Register */
#define NVIC_EN0_R (*((volatile unsigned long *)0xE000E100))
#define NVIC_PORTF_PIN 30

// Defines for individual pins
#define PIN0		0x01
#define PIN1		0x02
#define PIN2		0x04
#define PIN3		0x08
#define PIN4		0x10
#define PIN5		0x20
#define PIN6		0x40
#define PIN7		0x80
#define ALL_PINS	0xFF

#define BIT0		0
#define BIT1		1
#define BIT2		2
#define BIT3		3
#define BIT4		4
#define BIT5		5
#define BIT6		6
#define BIT7		7


#define DEACTIVATE	0
#define ACTIVATE	1

#define DISABLE		0
#define ENABLE		1

#define CLEAR		0
#define SET 		1

struct PORTF_GPIODR		// GPIO Data bits
{
	volatile unsigned DATA : 8; 	// GPIO Data
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTF_GPIODIR		// GPIO Direction bits
{
	volatile unsigned DIR : 8; 		// GPIO Data Direction
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTF_GPIOIS 		// GPIO Interrupt Sense
{
	volatile unsigned IS : 8; 		// GPIO Interrupt Sense
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTF_GPIOIEV
{
	volatile unsigned IEV : 8; 		// GPIO Interrupt Event
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTF_GPIOIM 
{
	volatile unsigned IME : 8; 		// GPIO Interrupt Mask Enable
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTF_GPIOICR
{
	volatile unsigned IC : 8; 		// GPIO Interrupt Clear
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTF_GPIOAFSEL		// GPIO Alternate Function Select bits
{
	volatile unsigned AFSEL : 8;	// GPIO Alternate Function Select
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTF_GPIODEN		// GPIO Digital Enable
{
	volatile unsigned DEN : 8;		// Digital Enable
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTF_GPIOPCTL		// GPIO Port Control
{
	volatile unsigned PMC0 : 4; 	// Port Mux Control 0
	volatile unsigned PMC1 : 4; 	// Port Mux Control 1
	volatile unsigned PMC2 : 4; 	// Port Mux Control 2
	volatile unsigned PMC3 : 4; 	// Port Mux Control 3
	volatile unsigned PMC4 : 4; 	// Port Mux Control 4
	volatile unsigned PMC5 : 4; 	// Port Mux Control 5
	volatile unsigned PMC6 : 4; 	// Port Mux Control 6
	volatile unsigned PMC7 : 4; 	// Port Mux Control 7
};	

extern struct PORTF_GPIODR * const PORTF_GPIODR_R;
extern struct PORTF_GPIODIR * const PORTF_GPIODIR_R;

// Interrupt Sense Registers
extern struct PORTF_GPIOIS * const PORTF_GPIOIS_R;
extern struct PORTF_GPIOIEV * const PORTF_GPIOIEV_R;
extern struct PORTF_GPIOIM * const PORTF_GPIOIM_R;
extern struct PORTF_GPIOICR * const PORTF_GPIOICR_R;

extern struct PORTF_GPIOAFSEL * const PORTF_GPIOAFSEL_R;
extern struct PORTF_GPIODEN * const PORTF_GPIODEN_R;
extern struct PORTF_GPIOPCTL * const PORTF_GPIOPCTL_R;

void PORTF_Init(void);

#endif /*GPIOF_H_*/
