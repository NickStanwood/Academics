/*================================================================================*
 * TITLE:	spi.h																  *
 * PURPOSE:	defines all the structs related to the performance of the SPI	  	  *
 * AUTHOR:	Emad Khan, Modified by Nick Stanwood								  *
 * CREATED:	April 2, 2012													  	  *
 * REVISION:Nov 13, 2016 - moved the interrupt handler to intr_handlers.c		  *
 * ===============================================================================*/

#ifndef SPI_H_
#define SPI_H_

struct SSI1_CR0		// SSI1 Control 0 Register bits
{
	volatile unsigned DSS : 4;		// SSI Data Size Select
	volatile unsigned FRF : 2;		// SSI Frame Format Select
	volatile unsigned SPO : 1;		// Serial Clock Polarity
	volatile unsigned SPH : 1; 		// Serial Clock Phase
	volatile unsigned SCR : 8; 		// Serial Clock Rate
	const unsigned RES16_31: 16;	// Reserved
};

struct SSI1_CR1	// SSI1 Control 1 Register bits
{
	volatile unsigned LBM : 1; 	// SSI Loopback Mode
	volatile unsigned SSE : 1;	// SSI Synchronous Serial Port Enable
	volatile unsigned MS  : 1;	// SSI Master/Slave Select
	volatile unsigned SOD : 1;	// SSI Slave Mode Output Disable
	volatile unsigned EOT : 1; 	// End of Transmission Interrupt Enable
	const unsigned RES5_31: 27;	// Reserved
};

struct SSI1_DR		// SSI1 Data Register bits
{
	volatile unsigned DATA : 16; 	// SSI Data
	const unsigned RES16_31 : 16; // Reserved
};

struct SSI1_SR		// SSI1 Status Register bits
{
	volatile const unsigned TFE : 1; 	// SSI Transmit FIFO Empty
	volatile const unsigned TNF : 1; 	// SSI Transmit FIFO Not Full
	volatile const unsigned RNE : 1;	// SSI Receive FIFO Not Empty
	volatile const unsigned RFF : 1; 	// SSI Receive FIFO Full
	volatile const unsigned BSY : 1; 	// SSI Busy Bit
	const unsigned RES5_31 : 27;		// Reserved
};

struct SSI1_CPSR	// SSI1 Clock Prescale Register bits
{
	volatile unsigned CPSDVSR : 8;	// SSI Clock Prescale Divisor (2 - 254)	
	const unsigned RES8_31 : 24;	// Reserved
};

struct SSI1_IM		// SSI1 Interrupt Mask Register bits
{
	volatile unsigned RORIM : 1; 	// SSI Receive Overrun Interrupt Mask
	volatile unsigned RTIM : 1;		// SSI Receive Time-Out Interrupt Mask
	volatile unsigned RXIM : 1; 	// SSI Receive FIFO Interrupt Mask
	volatile unsigned TXIM : 1;		// SSI Transmit FIFO Interrupt Mask
	const unsigned RES4_31 : 28;	// Reserved
};

struct SSI1_MIS // SSI1 Masked Interrupt Status bits
{
	volatile const unsigned RORMIS : 1; 	// SSI RecV Overrun Masked Interrupt Status
	volatile const unsigned RTMIS : 1;	// SSI RecV Time-Out Masked Interrupt Status
	volatile const unsigned RXMIS : 1;	// SSI Receive FIFO Masked Interrupt Status
	volatile const unsigned TXMIS : 1; 	// SSI Transmit FIFO Masked Interrupt Status
	const unsigned RES4_31 : 28; 		// Reserved
};

struct SSI1_ICR
{
	volatile unsigned RORIC : 1; 	// SSI Receive Overrun Interrupt Clear
	volatile unsigned RTIC : 1; 	// SSI Receive Time-Out Interrupt Clear
	const unsigned RES2_31 : 30;	// Reserved
};

struct spi_pkt{
	char start_byte;
	char cmd;
	char arg1;
	char arg2;
	char end_byte;
};

// Addresses of SSI1 Registers
extern struct SSI1_CR0 * const SSI1_CR0_R;
extern struct SSI1_CR1 * const SSI1_CR1_R;
extern struct SSI1_DR  * const  SSI1_DR_R;
extern struct SSI1_SR  * const  SSI1_SR_R;
extern struct SSI1_CPSR * const SSI1_CPSR_R; 
extern struct SSI1_IM  * const SSI1_IM_R;  
extern struct SSI1_MIS * const SSI1_MIS_R; 
extern struct SSI1_ICR * const SSI1_ICR_R;

// Function prototypes 
void SPI_Init(void);
void xmit_spi(int cmd , int arg1 , int arg2 , int sender);
void spi_controller(void);

void SPIDataTransmit(unsigned char Data);
//void SPIPacketTransmit(struct spi_pkt packet);

unsigned char SPIDataReceive(void);
void WaitSPIBusy(void);

#endif /*SPI_H_*/
