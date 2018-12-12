/*================================================================================*
 * TITLE:	gpioe.h																  *
 * PURPOSE:	contains decllerations of all GPIO registers on PortE				  *
 * AUTHOR:	Dr. Larry Hughes													  *
 * CREATED:	Nov 09, 2016														  *
 * ===============================================================================*/

#ifndef GPIOE_H_
#define GPIOE_H_

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

struct PORTE_GPIODR		// GPIO Data bits
{
	volatile unsigned DATA : 8; 	// GPIO Data
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTE_GPIODIR		// GPIO Direction bits
{
	volatile unsigned DIR : 8; 		// GPIO Data Direction
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTE_GPIOAFSEL		// GPIO Alternate Function Select bits
{
	volatile unsigned AFSEL : 8; 	// GPIO Alternate Function Select
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTE_GPIODEN		// GPIO Digital Enable
{
	volatile unsigned DEN : 8;		// Digital Enable
	const unsigned RES8_31 : 24;	// Reserved
};

struct PORTE_GPIOPCTL		// GPIO Port Control
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
	
extern struct PORTE_GPIODR * const PORTE_GPIODR_R; //= (struct PORTE_GPIODR *) 0x400243FC;
extern struct PORTE_GPIODIR * const PORTE_GPIODIR_R; //= (struct PORTE_GPIODIR *) 0x40024400;
extern struct PORTE_GPIOAFSEL * const PORTE_GPIOAFSEL_R; //= (struct PORTE_GPIOAFSEL *) 0x40024420;
extern struct PORTE_GPIODEN * const PORTE_GPIODEN_R; //= (struct PORTE_GPIODEN *) 0x4002451C;
extern struct PORTE_GPIOPCTL * const PORTE_GPIOPCTL_R; //= (struct PORTE_GPIOPCTL *) 0x4002452C;

#endif /*GPIOE_H_*/
