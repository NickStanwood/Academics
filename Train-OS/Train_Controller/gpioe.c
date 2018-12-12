/*================================================================================*
 * TITLE:	gpioe.c																  *
 * PURPOSE:	contains the PortE GPIO registers 									  *
 * AUTHOR:	Dr. Larry Hughes													  *
 * CREATED:	Nov 09, 2016														  *
 * ===============================================================================*/


#include "gpiof.h"
#include "sysctl.h"

struct PORTE_GPIODR * const PORTE_GPIODR_R = (struct PORTE_GPIODR *) 0x400243FC;
struct PORTE_GPIODIR * const PORTE_GPIODIR_R = (struct PORTE_GPIODIR *) 0x40024400;
struct PORTE_GPIOAFSEL * const PORTE_GPIOAFSEL_R = (struct PORTE_GPIOAFSEL *) 0x40024420;
struct PORTE_GPIODEN * const PORTE_GPIODEN_R = (struct PORTE_GPIODEN *) 0x4002451C;
struct PORTE_GPIOPCTL * const PORTE_GPIOPCTL_R = (struct PORTE_GPIOPCTL *) 0x4002452C;
