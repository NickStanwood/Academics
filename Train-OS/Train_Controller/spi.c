/*================================================================================*
 * TITLE:	spi.c															  	  *
 * PURPOSE:	initializese the SPI and allows data to be transmitted to it and  	  *
 * 			received from it												 	  *
 * AUTHOR:	Emad Khan, Modified by Nick Stanwood								  *
 * CREATED:	April 2, 2012														  *
 * REVISION:Nov 13, 2016 - moved the interrupt handler to intr_handlers.c   	  *
 * ===============================================================================*/

#include "gpioe.h"
#include "sysctl.h"
#include "spi.h"
#include "train_control.h"
#include "msg_queue.h"
#include "processcalls.h"
#include "systick.h"

// Structures associated with spi.h

struct SSI1_CR0  * const SSI1_CR0_R  = (struct SSI1_CR0 *) 0x40009000;	// SSI1 Control 0 Register
struct SSI1_CR1  * const SSI1_CR1_R  = (struct SSI1_CR1 *) 0x40009004;	// SSI1 Control 1 Register
struct SSI1_DR   * const  SSI1_DR_R  = (struct SSI1_DR  *) 0x40009008;	// SSI1 Data Register
struct SSI1_SR   * const  SSI1_SR_R  = (struct SSI1_SR  *) 0x4000900C;	// SSI1 Status Register
struct SSI1_CPSR * const SSI1_CPSR_R = (struct SSI1_CPSR*) 0x40009010; // SSI1 Clock Prescale Register
struct SSI1_IM   * const SSI1_IM_R   = (struct SSI1_IM  *) 0x40009014;	// SSI1 Interrupt Mask Register
struct SSI1_MIS  * const SSI1_MIS_R  = (struct SSI1_MIS *) 0x4000901C;	// SSI1 Masked Interrupt Status Register
struct SSI1_ICR  * const SSI1_ICR_R  = (struct SSI1_ICR *) 0x40009020;	// SSI1 Interrupt Clear Register


/*================================================================================*
 *
 *
 * ===============================================================================*/
void SPI_Init(void)	// initializes SPI
{
	// Activate SSI1
	SYSCTL_RCGC1_R -> SSI1 = ACTIVATE; 		
	
	// Activate Port E
	SYSCTL_RCGC2_R -> GPIOE = ACTIVATE;		
	
	// Activate alternate function select for Pins E0, E2, E3 
	PORTE_GPIOAFSEL_R -> AFSEL |= (PIN0 | PIN2 | PIN3);	
	PORTE_GPIOAFSEL_R -> AFSEL &= ~( (1<<BIT4) | (1<<BIT5) | (1<<BIT6) );
	
	// Set Pin Multiplexing (PMCx Bit Field = 2) for Pins E0, E2, E3
	PORTE_GPIOPCTL_R -> PMC0 = 0x02;
	PORTE_GPIOPCTL_R -> PMC2 = 0x02;
	PORTE_GPIOPCTL_R -> PMC3 = 0x02;
	
	// Set Pins E4, E5, E6 as output for Slave Selects 1, 2, 3
	PORTE_GPIODIR_R -> DIR |= ( (1<<BIT4) | (1<<BIT5) | (1<<BIT6) );
	
	// Set all pins on Port E to Digital I/O
	PORTE_GPIODEN_R -> DEN = ALL_PINS; 
	
	// Set all Slave Selects High
	PORTE_GPIODR_R -> DATA |= ( (1<<BIT4) | (1<<BIT5) | (1<<BIT6) );
	
	// Disable SSI
	SSI1_CR1_R -> SSE = DISABLE; 
  
  	// Set Master Mode
  	SSI1_CR1_R -> MS = CLEAR;
  	  
  	// Set CPSDVSR = 160 for Master Clock of 100KHz
  	SSI1_CPSR_R -> CPSDVSR = 160;
  	
  	// Set SCR = 0, SPH = 0, SPO = 0
  	SSI1_CR0_R -> SCR = 0;
  	SSI1_CR0_R -> SPH = 0;
  	SSI1_CR0_R -> SPO = 0;
  
  	// Set Data Size to 8 bits
  	SSI1_CR0_R -> DSS = 0x7;
  	
  	// Set Frame Format to SPI
  	SSI1_CR0_R -> FRF = 0;
  	
  	// Enable SSI
  	SSI1_CR1_R -> SSE = ENABLE;
} 

/*================================================================================*
 *
 *
 * ===============================================================================*/
void xmit_spi(int cmd , int arg1 , int arg2 , int sender)
{
	unsigned msg[4];
//	unsigned from;
	unsigned to = sender;

	msg[0] = TX_SPI;
	msg[1] = cmd;
	msg[2] = arg1;
	msg[3] = arg2;

	/* send spi command to the spi controller */
	send(XMIT_SOCKET , to , msg , sizeof(msg));
}


/*================================================================================*
 *
 *
 * ===============================================================================*/
void spi_controller(void)
{
	struct spi_pkt xmit_pkt;
	struct spi_pkt train_ctrl_pkt;		//to keep track of the last train packet sent
	struct spi_pkt *resp_pkt;

	unsigned msg[4];
	unsigned from;

	bind(XMIT_SOCKET);

	while(1){
		recv(XMIT_SOCKET , &from , msg , sizeof(msg));
		if(msg[0] == TX_SPI)
		{	// message contains a new packet to be sent

			/* unpack the spi command to be sent */
			xmit_pkt.cmd  = msg[1];
			xmit_pkt.arg1 = msg[2];
			xmit_pkt.arg2 = msg[3];
			resp_pkt = (struct spi_pkt *)xmit_spi_pkt(xmit_pkt);

			/* packet was sent to a train. retain a copy in case of failure */
			if(xmit_pkt.cmd == TR_CTRL){
				train_ctrl_pkt = xmit_pkt;
			}

			/* wait to ensure packet is handled by atmel before 	*
			 * transmitting the next one 							*/
			delay(16);
		}else{
			/* transmission error, re-send last packet */
			#ifdef _DEBUG_
				print_diag(TR_TX_ERR, train_ctrl_pkt);
			#endif	/* _ DEBUG_ */

			resp_pkt = (struct spi_pkt *)xmit_spi_pkt(train_ctrl_pkt);

			#ifdef _DEBUG_
				print_diag(GEN_SPI_RX, *resp_pkt);
			#endif	/* _ DEBUG_ */

			delay(16);

			/* acknowledge the error */
//			xmit_pkt.cmd  = ERR_RECV;
//			xmit_pkt.arg1 = EMPTY_ARG;
//			xmit_pkt.arg2 = EMPTY_ARG;
//			resp_pkt = (struct spi_pkt *)xmit_spi_pkt(xmit_pkt);
		}

	}
}

/*================================================================================*
 *
 *
 * ===============================================================================*/
void SPIDataTransmit(unsigned char Data)
{
	// Bring SS Low for the Atmel
	PORTE_GPIODR_R -> DATA &= ~(1 << 4);

	while(!(SSI1_SR_R -> TNF)){
		// Wait until there is space in Transmit FIFO
	}

	// Transmit the Character via the Data Register
	SSI1_DR_R -> DATA = Data;

	WaitSPIBusy();

	// Bring SS High for the Atmel
	PORTE_GPIODR_R -> DATA |= (1 << 4);

}


/*================================================================================*
 *
 *
 * ===============================================================================*/
unsigned char SPIDataReceive(void)
{
	// Wait until character is received in Receive FIFO
	while(!(SSI1_SR_R -> RNE)) ;

	// Return the 8 bit character from the 16 bit register
	return (SSI1_DR_R -> DATA & 0xFF);
}


/* Wait until SSI is free */
void WaitSPIBusy(void)
{ 
	while(SSI1_SR_R -> BSY) ;
}
