/*================================================================================*
 * TITLE:		train_display.h													  *
 * PURPOSE:		defines all structs related to printing out the train and train   *
 * 				track states													  *
 * STRUCTS:		switch_display - has two strings and a state. the strings are the *
 * 								 different switch states and the state is which   *
 * 								 orientation should be printed to the screen	  *
 * 				sensor_display - contains the row and coloumn where the sensor    *
 * 								 should be displayed on the screen				  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Nov 11, 2016														  *
 * REVISION:
 * ===============================================================================*/

#ifndef TRAIN_DISPLAY_H_
#define TRAIN_DISPLAY_H_

#include "train_control.h"

/* VT100 escape codes for foreground colors */
#define BLACK 30
#define RED 31
#define GREEN 32
#define YELLOW 33
#define BLUE 34
#define MAGENTA 35
#define WHITE 37

struct switch_display{
	char* straight;			//string for straight switch displaying on terminal
	char* turned;			//string for displaying turned switch on terminal
	int state;				//current state of switch
};

struct sensor_display{
	int row;				//row to begin printing sensor
	int col;				//column to begin printing sensor
};

struct path_display{
	int sensors[5];			//all the sensors that are a part of the path
	int num_sensors;		//number of sensors in path
};

extern struct switch_display switches[];
extern const struct sensor_display sensors[];
extern const struct path_display print_paths[];
extern char *layout[];

void display_controller(void);
void init_display(void);

void print_sensor(struct sensor_display sensor, int train);
void print_switch(struct switch_display sw);


#endif /* TRAIN_DISPLAY_H_ */
