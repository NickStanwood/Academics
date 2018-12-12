/*
 * command_line.c
 *
 *  Created on: 2016-11-27
 *      Author: Victor Maslov
 */


#include "kernel.h"
#include "mem_manager.h"
#include "UART.h"
#include "Systick.h"
#include "kernelcalls.h"
#include "processcalls.h"
#include "gpiof.h"
#include "train_control.h"

#define COM_ENTRY 3
#define MAX_BUFF 16

#define TIME 0
#define GO 1
#define SET 3

#define TRIG_ESC '\r'

#define NUL 0
#define BACK_SPC 127

char * uart_str_ptr;
char str_built[MAX_BUFF]; // String building char buffer
volatile int str_count = 0; // Counter for string buffer

struct command_entry {
    char * name;
    int (*func)(char *);
    int size;
};

const struct command_entry command_list[COM_ENTRY] =
{
    {"GOTO",train_goto,4},
    {"SETLOCATION",train_set,11},
    {"TRACKSWITCH",track_switch_,11},
};

void input_server() {
	unsigned packet_source;
	char interrupt_packet;

	struct spi_pkt *hs_recv;
	struct spi_pkt hs_cmd;


	/* Might want to do a hall reset here */

	bind(INPUT_SOCKET); // HAL server assigned first socket

	while(1) {
			recv(INPUT_SOCKET, &packet_source, &interrupt_packet, sizeof(char));
			string_parse( interrupt_packet );
		}
}

int string_parse( char ch ) {
	/* Function serves as UART control, to properly
	 * manage dequeued characters from the UART interrupt
	 */

	int i; // some index
	int func_flag = TRUE;
	char uart_str[MAX_BUFF]; /* initialze string of characters to be built */

	sprintf(uart_str,"%c\0",ch); /* Create Echo for character */

	/* If Backspace is found, decrement array counter and set to nul */
    if ( ch == BACK_SPC && str_count != 0 ) {
        str_count--;
        uart_str[str_count] = NUL;
        print("\b");
        return 0;
    }

    /* Str buffer is full */

	print(uart_str); /* Enqueue Char echo */
	/* Look for triggering esc char */
	if(ch == TRIG_ESC ) {
		/* Loop to find appropriate command if any */
		for ( i=0; i<COM_ENTRY; i++ ) {
			if ( strncmp(str_built,command_list[i].name,command_list[i].size) == 0) {

				func_flag = FALSE; // set func flag to FALSE notifying rest of routine that func has be found */
				print("\n\r");
				str_built+= command_list[i].size + 1;
				func_flag = command_list[i].func(str_built); // Call function from string found
				break;

			}
		}

		/* If func flag returned true anywhere, we will get a ? */
		if (func_flag == TRUE ) {
			print("\n\r?\n\r");
		}

		/* Carriage return has been found
		*find if belongs to any function
		*/
		str_count = 0; //Reset string counter
		memset(&str_built[0], 0, sizeof(str_built)); // Set array to NUL

		return 1; // returns TRUE if string is properly reset
	}

    if (str_count > MAX_BUFF ) {
    	return 0;
    }
	ch = toupper(ch); // assuring case insensitivity
	str_built[str_count] = ch;
	str_count++;

	return 0;
}

int train_goto(char * string) {
	/* Set train location */

}

int train_set(char * string) {
	/* Set current train location */

}

int track_switch(char * string) {
	/* Set current train location */

}

