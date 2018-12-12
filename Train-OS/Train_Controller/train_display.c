/*================================================================================*
 * TITLE:		train_display.c													  *
 * PURPOSE:		initializes the track display and allows sensors and switches to  *
 * 				be easily printed to the screen									  *
 * GLOBALS:	switches[] - maintains the state of each switch and also contains the *
 * 						 strings that are printed to display each switch state.   *
 * 						 the strings look messy as they contain multiple escape   *
 * 						 codes throughout										  *
 * 			 sensors[] - contains the starting row and column for each sensor for *
 * 						 printing												  *
 * 			  layout[] - an array of strings that, when printed, shows the full   *
 * 						 layout of the train track								  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Nov 12, 2016														  *
 * ===============================================================================*/

#include "train_display.h"
#include "train_control.h"
#include "UART.h"
#include "Systick.h"
#include "debugger.h"
#include "processcalls.h"
#include "msg_queue.h"

#include <stdio.h>

/* some very ugly looking strings that show the switch orientation when printed */
struct switch_display switches[NUM_SWITCHES] =
{
	{"\033[7;52H///////" , "\033[6;50H/y/\033[5;52H/y/\033[4;54H/y/\033[3;56Hy/"  , STRAIGHT } ,
	{"\033[7;92H///////" , "\033[11;92/y\033[10;93H/y/\033[9;95H/y/\033[8;97H/y/" , STRAIGHT } ,
	{"\033[7;143H//////" , "\033[3;143Ht`\033[4;144H`t`\033[5;146H`t`\033[6;148H`t." , STRAIGHT } ,
	{"\033[43;140H//////", "\033[47;140Hy/\033[46;141H/y/\033[45;143H/y/\033[44;145H/y/" , STRAIGHT },
	{"\033[43;98H///////", "\033[42;96H/y/\033[41;98H/y/\033[40;100H/y/\033[39;102H/y/" , STRAIGHT },
	{"\033[43;52H///////", "\033[44;51H`t`\033[45;53H`t`\033[46;55H`t`\033[47;57H`t" , STRAIGHT }
};


/* starting print location for each sensor */
const struct sensor_display sensors[NUM_SENSORS] =
{
	{42 , 31} , {42 , 65} , {42 , 83} , {42 ,108} ,		//sensors  1 - 4
	{42 ,129} , {42 ,159} , {29 ,188} , {20 ,188} ,		//sensors  5 - 8
	{ 6 ,160} , { 6 ,129} , { 6 ,109} , { 6 , 82} ,		//sensors  9 - 12
	{ 6 , 62} , { 6 , 30} , {20 , 1 } , {27 , 1 } ,		//sensors 13 - 16
	{46 , 62} , {46 ,133} , { 2 ,136} , { 2 , 58} ,		//sensors 17 - 20
	{38 ,105} , {38 ,134} , {10 , 85} , {10 , 57}		//sensors 21 - 24
};

const struct path_display print_paths[NUM_PATHS] =
{
		{{ 0 , 0 , 0 , 0 , 0 } , 0} ,	//path 0
		{{ 16, 15, 0 , 0 , 0 } , 2} ,	//path 1
		{{ 14, 13, 12, 11, 10} , 5} ,	//path 2
		{{ 8 , 7 , 0 , 0 , 0 } , 2} ,	//path 3
		{{ 6 , 5 , 4 , 3 , 2 } , 5} , 	//path 4
		{{ 14, 20, 0 , 0 , 0 } , 2} ,	//path 5
		{{ 19, 1 , 0 , 0 , 0 } , 2} ,	//path 6
		{{ 6 , 18, 0 , 0 , 0 } , 2} , 	//path 7
		{{ 17, 1 , 0 , 0 , 0 } , 2} , 	//path 8
		{{ 24, 23, 11, 10, 0 } , 4} , 	//path 9
		{{ 10, 11, 23, 24, 0 } , 4} , 	//path 10
		{{ 2 , 3 , 21, 22, 0 } , 4} , 	//path 11
		{{ 22, 21, 3 , 2 , 0 } , 4} , 	//path 12
};

/*================================================================================*
 * initializes the train display and then terminates. the strings that hold the   *
 * train display are in a separte source file called train_layout.c				  *
 * ===============================================================================*/
void init_display(void){
	int i = 0;
	char instring[TERM_WIDTH];
	int row = 1;
	int col = 1;

	print("\x1b[08m");

	/* print out the track layout */
	for(i = 0 ; i < 50 ; i++){
		print_string(layout[i] , row , col);
		row++;
	}

	#ifdef _DEBUG_
	for(i = 0 ; i < 22 ; i++){
		print_string(debug_console[i] , d_row + i , d_col);
	}
	d_row += 2;		//move debug row into the debug console
	d_col++;		//move debug column into the debug console
	#endif

	print("\x1b[35m");		//make font purple

	/* print sensor number above sensor location */
	for(i = 0 ; i < NUM_SENSORS ; i++){
		instring[0] = (i+1) / 10 + ITOA_CONV;
		instring[1] = (i+1) % 10 + ITOA_CONV;
		instring[2] = '\0';
		print_string(instring , sensors[i].row-1 , sensors[i].col+2);
	}

	/* print out the switch numbers in their proper locations */
	/* A1 = (08, 49) , B1 = (06, 59) , C1 = (03, 54) */
	print("\x1b[8;49HA1\x1b[06;59HB1\x1b[03;54HC1");

	/* A2 = (06,101) , B2 = (06, 93) , C2 = (11, 95) */
	print("\x1b[6;101HA2\x1b[06;93HB2\x1b[11;95HC2");

	/* A3 = (08,152) , B3 = (06,139) , C3 = (03,145) */
	print("\x1b[8;152HA3\x1b[06;139HB3\x1b[03;145HC3");

	/* A4 = (42,149) , B4 = (42,140) , C4 = (47,143) */
	print("\x1b[42;149HA4\x1b[42;140HB4\x1b[47;143HC4");

	/* A5 = (44, 96) , B5 = (44,105) , C5 = (39,100) */
	print("\x1b[44;96HA5\x1b[44;105HB5\x1b[39;100HC5");

	/* A6 = (42, 50) , B6 = (42, 61) , C6 = (47, 55) */
	print("\x1b[42;50HA6\x1b[42;61HB6\x1b[47;55HC6");

	for(i = 0 ; i < 6 ; i++){
		print_switch(switches[i]);
	}

	terminate();
}


/*================================================================================*
 * displays where the trains currently are. runs at mid priority so local_train() *
 * and express_train() can be above or below it in the priority list depending on *
 * if they are blocking or not													  *
 * ===============================================================================*/
void display_controller(void){
	unsigned disp_msg[4];
	unsigned disp_src;

	int train;
	int path;
	int hs_num;
	int sw_num;

	int i;

	bind(DISPLAY_SOCKET);

	/* check if the states of any switches or sensors have changed */
	/* print out the new sensor/switch states */


	while(1){
		recv(DISPLAY_SOCKET, &disp_src, &disp_msg, sizeof(disp_msg));
		if (disp_msg[0] == TRIGGERED_HS) {
			hs_num = disp_msg[1];
			train  = disp_msg[2];
			if(hs_num != 0 && hs_num <= 24)
				print_sensor(sensors[hs_num-1], train);
		}
		else if(disp_msg[0] == SW_STATE_CHANGE){
			sw_num = disp_msg[1];
			print_switch(switches[sw_num-1]);
		}
		else if(disp_msg[0] == FULL_PATH){
			path = disp_msg[1];
			train = disp_msg[2];
			for(i = 0; i < print_paths[path].num_sensors ; i++){
				print_sensor(sensors[print_paths[path].sensors[i] - 1] , train);
			}
		}

	}
}

/*================================================================================*
 * prints out a sensor in a specific color. colors correspond to the train that   *
 * has activated the sensor.								 					  *
 * ===============================================================================*/
void print_sensor(struct sensor_display sensor , int train){
	int i;
	char out_str[3][13] = {"\x1b[--m||===||" ,	//leading color escape sequence
						   "\x1b[--m||   ||" ,
						   "\x1b[--m||===||"};

	switch(train){
		case EXPRESS:
			for(i = 0 ; i < 3 ; i++){
				out_str[i][2] = BLUE / 10 + ITOA_CONV;		//set color
				out_str[i][3] = BLUE % 10 + ITOA_CONV;
			}

			out_str[1][7] = 'E';
			out_str[1][8] = 'X';					//set inner text
			out_str[1][9] = 'P';

			break;
		case LOCAL:
			for(i = 0 ; i < 3 ; i++){
				out_str[i][2] = YELLOW / 10 + ITOA_CONV;	//set color
				out_str[i][3] = YELLOW % 10 + ITOA_CONV;
			}
			out_str[1][7] = 'L';
			out_str[1][8] = 'O';					//set inner text
			out_str[1][9] = 'C';
			break;
		default:
			for(i = 0 ; i < 3 ; i++){
				out_str[i][2] = WHITE / 10 + ITOA_CONV;	//set color
				out_str[i][3] = WHITE % 10 + ITOA_CONV;
			}
			break;
	}

	for(i = 0 ; i < 3 ; i++){
		print_string(out_str[i], sensor.row + i , sensor.col);
	}

	print("\x1b[37m");	//set color back to white
}

/*================================================================================*
 * prints the state of a switch by blacking out the direction that the switch is  *
 * not pointing to. the strings for each switch are in the switch_display struct  *
 * ===============================================================================*/
void print_switch(struct switch_display sw){
	char temp_str[5] = "\033[--m";	//VT 100 new attribute escape code

	if(sw.state == STRAIGHT){
		temp_str[2] = BLACK / 10 + ITOA_CONV;
		temp_str[3] = BLACK % 10 + ITOA_CONV;
		print(temp_str);				//make font black
		print(sw.turned);

		temp_str[2] = GREEN / 10 + ITOA_CONV;
		temp_str[3] = GREEN % 10 + ITOA_CONV;
		print(temp_str);				//make font green
		print(sw.straight);
	}
	else{
		temp_str[2] = BLACK / 10 + ITOA_CONV;
		temp_str[3] = BLACK % 10 + ITOA_CONV;
		print(temp_str);				//make font black
		print(sw.straight);

		temp_str[2] = GREEN / 10 + ITOA_CONV;
		temp_str[3] = GREEN % 10 + ITOA_CONV;
		print(temp_str);				//make font green
		print(sw.turned);
	}
	print("\x1b[37m");	//set color back to white
}

