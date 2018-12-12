/*================================================================================*
 * TITLE:		kernelcalls.c													  *
 * PURPOSE:		this source file contains all of the kernel functions.			  *
 * 				this module resides in kernel space and the functions are not 	  *
 * 				able to be called by processes									  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Nov 09, 2106														  *
 * REVISION:Nov 19, 2016 - changed xmit_spi_pkt() return value to be the received *
 * 						   packet instead of an error value. the error value is   *
 * 						   now stored in recv_pkt.end_byte (0x99 | 0x3F)          *
 * ===============================================================================*/

#include "kernelcalls.h"
#include "kernel.h"
#include "UART.h"
#include "mem_manager.h"
#include "Systick.h"
#include "train_control.h"
#include "train_display.h"
#include "gpiof.h"
#include "spi.h"
#include "debugger.h"

#include <stdio.h>

/* kernel function - initializes kernel */
void Kinit(struct kcallargs *kcall)
{
	int i = 0;
	void (*funcptr)();

	NVIC_SYS_PRI3_R |= PENDSV_LOWEST_PRIORITY;

	for(i = 0 ; i < MAX_NUM_PRI ; i++){
		wtr[i] = NULL;
		wtr[i]->next = wtr[i];
		wtr[i]->prev = wtr[i];
	}

	/* create null process at bottom priority */
	funcptr = idle_process;
	create_proc(funcptr , MAX_NUM_PRI-1);

	funcptr = init_display;
	create_proc(funcptr , PRI0);	//create display initialization process

	funcptr = spi_controller;
	create_proc(funcptr , PRI1);	//create process for handling spi transmissions

	funcptr = train_control_init;
	create_proc(funcptr , PRI1);	//create train control initialization process

	funcptr = express_controller;
	create_proc(funcptr, PRI1);		//create process that runs express train

	funcptr = local_controller;
	create_proc(funcptr, PRI1);		//create process that runs local train

	funcptr = hal_server;
	create_proc(funcptr, PRI1);		//create process that acknowledges hall sensors

	funcptr = TheFatController;
	create_proc(funcptr, PRI1);		//create process that keeps state of track

	funcptr = display_controller;
	create_proc(funcptr, PRI2);		// create process for display control

	/* initialize SysTick, uart and GPIO on PortF */
	systick_init();
	SysTickIntDisable();
	UART_full_init();
	PORTF_Init();
	SPI_Init();
	enable();

	return;
}

/* kernel function - returns id of current process */
void Kgetid(struct kcallargs *kcall)
{
	kcall->rtnvalue = running->PID;
}

/* kernel function - changes the priority of the current process  */
/* 				   - priority 0 is the highest down to priority 3 */
void Knice(struct kcallargs *kcall)
{
	dequeue_pcb(running , cur_priority);

	switch(kcall->arg1){
		case PRI0:
		case PRI1:
		case PRI2:
		case PRI3:
			cur_priority = kcall->arg1;	//set priority to the specified priority
			break;
		case INC:						//increase the priority level
			if(cur_priority != 0)		//PRI0 is highest level
				cur_priority--;			//decrease pri index (i.e. increase pri level)
			break;
		case DEC:						//decrease priority level
			if(cur_priority != 3)		//PRI3 is lowest priority
				cur_priority++;			//increase pri index (i.e. decrease pri level)
			break;
		case GUCPU:						//give up CPU (dequeue then enqueue at end of WTR[])
		default:
			break;
	}

	enqueue_pcb(running, cur_priority);

	NVIC_INT_CTRL_R |= TRIGGER_PENDSV;	//trigger pendSV

	return;
}

/* kernel function - terminates the current process */
void Kterminate(struct kcallargs *kcall){
	
	dequeue_pcb(running, cur_priority);
	deallocate(running->stack_entry);
	deallocate((int)running);

	NVIC_INT_CTRL_R |= TRIGGER_PENDSV;	//trigger pendSV

	return;
}

/* kernel function - sends chars to the uart */
void Kprint(struct kcallargs *kcall){
	
	char *outstring = (char*)kcall->arg1;

	while(*outstring != '\0'){
		if(xmit_busy){
			disable();
			enqueue_uart_out(*outstring);
			enable();
		}else{
			xmit_busy = true;
			UART0_DR_R = *outstring;
		}
		outstring++;
	}
	return;
}


void kxmit_spi(struct kcallargs *kcall){
	struct spi_pkt *outpkt = (struct spi_pkt*)kcall->arg1;
	struct spi_pkt resp_pkt;

	SPIDataTransmit(0xAA);						//first response is just the offset

	SPIDataTransmit(outpkt->cmd);
	resp_pkt.start_byte = SPIDataReceive();		//get resp from atmel (0xCC - start)

	SPIDataTransmit(outpkt->arg1);
	resp_pkt.cmd  = SPIDataReceive();			//get resp from atmel (command)

	SPIDataTransmit(outpkt->arg2);
	resp_pkt.arg1 = SPIDataReceive();			//get resp from atmel (resp1)

	SPIDataTransmit(0x55);
	resp_pkt.arg2 = SPIDataReceive();			//get resp from atmel (resp2)

	SPIDataTransmit(0xFF);						//offset byte
	resp_pkt.end_byte = SPIDataReceive();		//get resp from  atmel (0x99/0x3F - suc/err)

	kcall->rtnvalue = (unsigned)&resp_pkt;
	return;
}

void Krun(struct kcallargs *kcall){
	get_new_proc();
	set_PSP(running -> sp + 8 * sizeof(unsigned int));
	SysTickIntEnable();
	__asm("	movw 	LR,#0xFFFD");  /* Lower 16 [and clear top 16] */
	__asm("	movt 	LR,#0xFFFF");  /* Upper 16 only */
	__asm("	bx 	LR");          /* Force return to PSP */
}


#ifdef _DEBUG_
void kprint_diag(struct kcallargs *kcall)
{

	char fmtstr[15] = "\x1b[--;---H\x1b[--m";
	int i = 0;
	char diag_str[D_STR_SZ];
	struct spi_pkt *pkt = (struct spi_pkt*)kcall->arg2;

	/* set diag_str to the proper diagnostic */
	for(i = 0 ; i < D_STR_SZ ; i++){
		diag_str[i] = diagnostics[kcall->arg1][i];
	}
	diag_str[D_STR_SZ - 1] = '\0';

	fmtstr[2] = d_row / 10 + ITOA_CONV;
	fmtstr[3] = d_row % 10 + ITOA_CONV;

	fmtstr[5] = d_col /100 + ITOA_CONV;
	fmtstr[6] = (d_col / 10) % 10 + ITOA_CONV;
	fmtstr[7] = d_col % 10 + ITOA_CONV;

	fmtstr[11] = WHITE / 10 + ITOA_CONV;
	fmtstr[12] = WHITE % 10 + ITOA_CONV;

	fmtstr[14] = '\0';

	i = 0;
	/* output escape code */
	while(fmtstr[i] != '\0'){
		if(xmit_busy){
			disable();
			enqueue_uart_out(fmtstr[i]);
			enable();
		}else{
			xmit_busy = true;
			UART0_DR_R = fmtstr[i];
		}
		i++;
	}


	/* put argument into diagnostic */
	diag_str[37] = HTOA(pkt->cmd >> 4);
	diag_str[38] = HTOA(pkt->cmd & 0x0F);

	diag_str[40] = HTOA(pkt->arg1 >> 4);
	diag_str[41] = HTOA(pkt->arg1 & 0x0F);

	diag_str[43] = HTOA(pkt->arg2 >> 4);
	diag_str[44] = HTOA(pkt->arg2 & 0x0F);

	diag_str[46] = HTOA(pkt->end_byte >> 4);
	diag_str[47] = HTOA(pkt->end_byte & 0x0F);

	i = 0;
	/* output diagnostic  */
	while(diag_str[i] != '\0'){
		if(xmit_busy){
			disable();
			enqueue_uart_out(diag_str[i]);
			enable();
		}else{
			xmit_busy = true;
			UART0_DR_R = diag_str[i];
		}
		i++;
	}

	/* increase the row (& col if needed) */
	d_row = (d_row + 1) % D_MAX_ROW;
	if(d_row == 0){
		d_row = DEBUG_ROW + 2;
		if(d_col == DEBUG_COL+1)
			d_col += 50;
		else
			d_col -= 50;
	}

	return;
}
#endif		/* _DEBUG_ */



