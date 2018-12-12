/*================================================================================*
 * TITLE:	debugger.h														  	  *
 * PURPOSE:	contains all aspects to do with debugging 						  	  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Nov 14, 2016														  *
 * REVISION:	*
 * ===============================================================================*/

#ifndef DEBUGGER_H_
#define DEBUGGER_H_

/* uncomment _DEBUG_ to show diagnostics on the screen */
#define _DEBUG_

/* where to begin printing out the debug console */
#define DEBUG_ROW 		13
#define DEBUG_COL 		49
#define D_MAX_ROW 		34
#define D_STR_SZ		51

enum diag_indx {
	GEN_SPI_TX,			//a general message for any SPI transmission
	GEN_SPI_RX,			//a general message for any SPI receive
	L_SUC_SPI , 		//successful SPI transmission from local train
	E_SUC_SPI , 		//successful SPI transmission from express train
	L_ERR_SPI , 		//error during local train SPI transmission
	E_ERR_SPI , 		//error during express train SPI transmission
	INIT_ERR  , 		//error during train initialization
	INIT_SUC  ,			//success during train initialization
	NO_ERR	  ,			//no error in atmel error queue
	TR_TX_ERR ,			//error in atmel error queue
	SW_SUC	  ,			//switch control spi packet sent successfully
	SW_ERR	  ,			//switch control spi packet not sent properly
	HS_RST_SUC,			//successfully reset a hall sensor
	HS_RST_ERR,			//failure while reseting a hall sensor
	NUM_DIAG  			//keeps track of the number of diagnostics as more get added
};

#ifdef _DEBUG_

extern char *debug_console[];			//printed out in the middle of the track
extern int d_row;						//row to print out diagnostic
extern int d_col;						//column to print out diagnostic
extern char diagnostics[][D_STR_SZ];	//diagnostics strings

#endif	/* _ DEBUG_ */


#endif /* DEBUGGER_H_ */
