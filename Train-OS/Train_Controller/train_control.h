/*================================================================================*
 * TITLE:		train_control.h													  *
 * PURPOSE:		defines all the commands that can be passed to he trains and the  *
 * 				arguments that pertain to them. (i.e. SPEED, DIR, TRAIN_NUM)	  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Nov 11, 2016														  *
 * REVISION:	*
 * ===============================================================================*/

#ifndef TRAIN_CONTROL_H_
#define TRAIN_CONTROL_H_

#include "kernel.h"

#define NUM_SWITCHES	6		// number of switches on track
#define NUM_SENSORS		24		//number of sensors on track
#define NUM_PATHS		13		//number of paths to take around the track
#define NUM_EXP_PATHS	4		//num of paths for the express to make a full circuit
#define NUM_LOC_PATHS  	14		//num of paths for the local to make a full circuit
#define END_OF_PATH		2
#define START_OF_PATH	0

/* magnitudes for train speed */
#define BRAKE  0x00
#define SPEED1 0x01
#define SPEED2 0x02
#define SPEED3 0x03
#define SPEED4 0x04
#define SPEED5 0x05
#define SPEED6 0x06
#define SPEED7 0x07

/* direction of train */
#define DIR_CW  0x00
#define DIR_CCW 0x10

/* spi commands to be sent from cortex to the atmel */
#define HS_INPUT	0xA0		//get triggered HS number
#define HS_QRST		0xAE		//reset all Atmel HS queues (for startup)
#define HS_ACK		0xC0		//acknowledge a hall sensor
#define HS_ACKALL	0xCE		//acknowledge all hall sensors
#define TR_CTRL		0xB0		//control a single trains speed and dir
#define ATR_CTRL	0xBE		//control all trains speed and dir
#define SW_CTRL		0xD0		//control the state of a switch
#define ERR_RECV	0xE0		//retrieve an error if there is one
#define ERR_QRST	0xEE		//reset atmel error queues

/* spi start and end packet bytes */
#define START_SPI 	0xAA
#define END_SPI 	0x55
#define EMPTY_ARG	0xFF

/* spi standard response values */
#define RESP_SUC	0x99
#define RESP_ERR	0x3f
#define RESP_START 	0xCC
#define ERR_CODE	0x04	//for atmel error queue

enum train_types { EXPRESS = 1 , LOCAL , NO_TRAIN};
enum switch_states{TURNED, STRAIGHT};

struct path{
	int ID;
	int hs[3];
	int sw_num;
	int state;
	int col_area;
};

extern const struct path paths[];

void train_control_init(void);
void TheFatController(void);
void express_controller(void);
void local_controller(void);
void hal_server(void);
int switch_track(unsigned switch_num, unsigned state , unsigned socket);

#endif /* TRAIN_CONTROL_H_ */
