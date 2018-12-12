/*================================================================================*
 * TITLE:	main.c															      *
 * PURPOSE:	initializes the memory, and the kernel. in the kernel initialization  *
 * 			the interrupts are set up (systick, uart, portF, SPI). the processes  *
 * 			that control the trains are also initialized here.					  *
 * AUTHOR:	Victor Maslov, Nick Stanwood									  	  *
 * CREATED:	Nov 09, 2016														  *
 * ===============================================================================*/

#include <stdio.h>

#include "kernel.h"
#include "mem_manager.h"

int main(void) {
	init_mem();
	Kernel_Initialization();
}

