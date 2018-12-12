/*================================================================================*
 * TITLE:	debugger.c															  *
 * PURPOSE:	this header file is essentially empty when _DEBUG_ is not #defined    *
 *			but stores all diagnostics messages and the debug window			  *
 * GLOBALS:	d_row - keeps track of the current row for debugging diagnostics to   *
 * 					be printed													  *
 * 			d_col - keeps track of the diagnostic column 						  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Nov 14, 2016														  *
 * REVISION:Nov 21, 2016 - diagnostics now print out an spi packet along with the *
 * 						   message, the start byte is not included when the packet*
 * 						   is displayed											  *
 * ===============================================================================*/

#include "debugger.h"

#ifdef _DEBUG_

int d_row = DEBUG_ROW;
int d_col = DEBUG_COL;

/*format for the diagnostic strings are as follows:             *
 * <FROM> -- <diagnostic message>: <cmd> <resp1> <resp2> <succ> *
 * 														 		*
 * <FROM> is object the diagnostic pertains to             		*
 * <diagnostic message> is a more specific message         		*
 * <cmd> is the spi command that was sent 			     		*
 * <resp1> <resp2> are the responses fro mthe atmel        		*
 * <succ> is the end byte (0x99 | 0x3F)					 		*
 * all values are written in hex								*/
char diagnostics[NUM_DIAG][D_STR_SZ] = {
		"GEN  -- SPI packet transmitted     : 00 00 00 00 |" ,
		"GEN  -- SPI packet received        : 00 00 00 00 |" ,
		"LOC  -- Successful SPI transmission: 00 00 00 00 |" ,
		"EXP  -- Successful SPI transmission: 00 00 00 00 |" ,
		"LOC  -- Failed SPI transmission    : 00 00 00 00 |" ,
		"EXP  -- Failed SPI transmission    : 00 00 00 00 |" ,
		"INIT -- initialization error       : 00 00 00 00 |" ,
		"INIT -- initialization successful  : 00 00 00 00 |" ,
		"ERRQ -- no errors in error queue   : 00 00 00 00 |" ,
		"ERRQ -- error Txing. resent packet : 00 00 00 00 |" ,
		"SW   -- Successful SPI transmission: 00 00 00 00 |" ,
		"SW   -- Failed SPI transmission    : 00 00 00 00 |" ,
		"HS   -- reset hall sensor          : 00 00 00 00 |" ,
		"HS   -- failed to reset hall sensor: 00 00 00 00 |" ,
};

char* debug_console[22] = {
		"____________________________________________________________________________________________________ " ,
		"|///////////////////////////////////////////DEBUG WINDOW////////////////////////////////////////////|" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|                                                 |                                                 |" ,
		"|_________________________________________________|_________________________________________________|" ,
};

#endif	/* _ DEBUG_ */


