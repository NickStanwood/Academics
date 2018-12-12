/*================================================================================*
 * TITLE:	train_control.c												 	  	  *
 * PURPOSE:	controls the trains so that they don't hit each other and they can 	  *
 * 			still perform their respective tasks. it does this by determining 	  *
 * 			which path they should enter next. the express train almost always    *
 * 			stays on the main track and never takes the alternate routes, however *
 * 			the local train goes at a much slower pace and more often than not    *
 * 			will take the alternate route because the express train is approaching*
 * 			and needs to give priority to it.									  *
 * GLOBALS:	paths[] -  *
 *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Nov 11, 2016														  *
 * REVISION:	*
 * ===============================================================================*/

#include "train_control.h"
#include "UART.h"
#include "Systick.h"
#include "train_display.h"
#include "kernel.h"
#include "processcalls.h"
#include "spi.h"
#include "debugger.h"
#include "msg_queue.h"

const struct path paths[NUM_PATHS] = {
	/*  | ID  |  hs array | SW  |      state      | collision area|  	*/
	/*  |-----|-----------|-----|-----------------|---|        			*/
		{ 0 , { 0 , 0 , 0 } , 0 ,      BRAKE      , 0 } ,	//Path 0 (i.e. dwell)
		{ 1 , { 16, 16, 15} , 0 , DIR_CW | SPEED5 , 0 } ,	//Path 1
		{ 2 , { 14, 10, 11} , 0 , DIR_CW | SPEED5 , 1 } , 	//Path 2
		{ 3 , { 8 , 8 , 7 } , 0 , DIR_CW | SPEED5 , 0 } , 	//Path 3
		{ 4 , { 6 , 2 , 3 } , 0 , DIR_CW | SPEED5 , 2 } ,  	//Path 4
		{ 5 , { 14, 20, 20} , 1 , DIR_CW | SPEED5 , 0 } ,  	//Path 5
		{ 6 , { 19, 9 , 9 } , 3 , DIR_CW | SPEED4 , 1 } ,  	//Path 6
		{ 7 , { 6 , 18, 18} , 4 , DIR_CW | SPEED5 , 0 } ,  	//Path 7
		{ 8 , { 17, 1 , 1 } , 6 , DIR_CW | SPEED4 , 2 } ,  	//Path 8
		{ 9 , { 23, 10, 11} , 2 , DIR_CW | SPEED3 , 1 } ,  	//Path 9
		{ 10, { 11, 23, 23} , 2 , DIR_CCW| SPEED6 , 0 } ,  	//Path 10
		{ 11, { 21, 2 , 3 } , 5 , DIR_CW | SPEED3 , 2 } ,  	//Path 11
		{ 12, { 3 , 21, 21} , 5 , DIR_CCW| SPEED6 , 0 } ,  	//Path 12
};

/*================================================================================*
 * controls the express train. tries to maintain an average speed of 5 as well as *
 * keep the train running on the main track. in the unlikely even the express	  *
 * needs to avoid a collision it will slow to speed 3 							  *
 * ===============================================================================*/
void express_controller(void)
{

	/* for messages */
	unsigned from;
	unsigned msg[4];

	/* for the express train route */
	unsigned route[] = { 3 , 4 , 1 , 2};
	int dst_indx = 0;

	/* set the initial express train state */
	struct path cur_path = paths[2];
	struct path nxt_path = paths[route[dst_indx]];
	struct path prv_path = paths[0];	//set previous to no path initially

	int hs_indx = 0;	//to know which milestone of the path the train has hit
	int hs_num  = 0;	//sent from the controller
	int loc_cur = 0;	//sent from the controller

	bind(EXP_TR_SOCKET);

	dst_indx = (dst_indx + 1) % NUM_EXP_PATHS;

	/* set the express initial state */
	xmit_spi(TR_CTRL , EXPRESS , cur_path.state , EXP_TR_SOCKET);

	while(1) {
		recv(EXP_TR_SOCKET, &from, &msg, sizeof(msg));
		hs_num  = msg[1];
		loc_cur = msg[2];
		if(hs_indx == START_OF_PATH)
		{	/* start of path */

			/* set switch to turned if needed */
			if(cur_path.sw_num != 0){
				switch_track(cur_path.sw_num , TURNED , EXP_TR_SOCKET);
			}

			/* collision is imminent */
			if(cur_path.col_area & paths[loc_cur].col_area){

				/* set new speed for the path */
				xmit_spi(TR_CTRL , EXPRESS , SPEED3 , EXP_TR_SOCKET);

				/* send a collision avoidance message to the conductor */
				msg[0] = COLLISION;
				msg[1] = cur_path.col_area;
				msg[2] = EXPRESS;
				send(CONDUCTOR_SOCKET , EXP_TR_SOCKET , msg , sizeof(msg));

				while(1){
					/* wait for the local to get out of the way */
					recv(EXP_TR_SOCKET , &from , msg , sizeof(msg));
					if(msg[0] != FULL_PATH){
						hs_indx++;	//maintain the number of checkpoint sensors hit
					}else{
						break;
					}
				}

				xmit_spi(TR_CTRL , EXPRESS , cur_path.state , EXP_TR_SOCKET);
			}	//end of collision detection

			/* acquire next path */
			msg[0] = FULL_PATH;
			msg[1] = nxt_path.ID;
			msg[2] = EXPRESS;
			send(CONDUCTOR_SOCKET , EXP_TR_SOCKET , msg , sizeof(msg));

			/* update terminal with new position for express */
			msg[1] = prv_path.ID;
			msg[2] = NO_TRAIN;
			send(DISPLAY_SOCKET , EXP_TR_SOCKET , msg , sizeof(msg));

			msg[1] = cur_path.ID;
			msg[2] = EXPRESS;
			send(DISPLAY_SOCKET , EXP_TR_SOCKET , msg , sizeof(msg));
		}	//end of 'start of path' check

		if((hs_indx == END_OF_PATH))
		{	/* at end of path */
			/* change current path to the next path */
			prv_path = cur_path;
			cur_path = nxt_path;
			nxt_path = paths[route[dst_indx]];
			dst_indx = (dst_indx + 1) % NUM_EXP_PATHS;
			hs_indx = -1;

			/* set speed and direction for the path */
			xmit_spi(TR_CTRL , EXPRESS , cur_path.state , EXP_TR_SOCKET);

			if(prv_path.sw_num != 0)
			{	/* switch needs to be set back to straight */
				switch_track(prv_path.sw_num , STRAIGHT , EXP_TR_SOCKET);
			}

		}	//end of 'end of path' check

		hs_indx++;

		msg[0] = NEW_TR_HS;
		msg[1] = cur_path.hs[hs_indx];
		msg[2] = EXPRESS;
		send(CONDUCTOR_SOCKET , EXP_TR_SOCKET , msg , sizeof(msg));
		}
}


/*================================================================================*
 * controls the local train. the local train has a lower priority than the express*
 * train it stops at dseignated locations and avoids the express when a collision *
 * is going to happen															  *
 * ===============================================================================*/
void local_controller(void)
{
	unsigned from;
	unsigned msg[4];

	/* for the local train route */
	int route[NUM_LOC_PATHS]  = {6 , 10 , 0 , 9 , 3 , 7 , 0 , 8 , 12 , 0 , 11 , 1 , 5 , 0};
	unsigned dst_indx = 4;

	/* set the initial local train state */
	struct path cur_path = paths[route[dst_indx]];
	struct path nxt_path = paths[route[dst_indx + 1]];
	struct path prv_path = paths[0];

	int hs_num   = 0;
	int hs_indx  = 0;
	int exp_cur  = 0;
	int exp_nxt  = 0;

	bind(LOC_TR_SOCKET);

	dst_indx = (dst_indx + 2) % NUM_LOC_PATHS;

	xmit_spi(TR_CTRL , LOCAL , cur_path.state , LOC_TR_SOCKET);

	while(1){
		recv(LOC_TR_SOCKET, &from , msg , sizeof(msg));
		hs_num = msg[1];
		exp_cur = msg[2];
		exp_nxt = msg[3];
		if(hs_indx == START_OF_PATH)
		{	/* start of path */

			/* collision is imminent*/
			while(cur_path.col_area & (paths[exp_cur].col_area | paths[exp_nxt].col_area))
			{	/* requires a while() loop because if the exp_nxt path caused the block then *
				 * when the express leaves its current path it will notify the local, but it *
				 * is still not out of the way  											 */

				/* stop the train */
				xmit_spi(TR_CTRL , LOCAL , DIR_CCW|SPEED7 , LOC_TR_SOCKET);
				delay(512);
				xmit_spi(TR_CTRL , LOCAL , BRAKE , LOC_TR_SOCKET);

				/* send a collision notification to the conductor */
				msg[0] = COLLISION;
				msg[1] = cur_path.col_area;
				msg[2] = LOCAL;
				send(CONDUCTOR_SOCKET , LOC_TR_SOCKET , msg , sizeof(msg));

				/* wait for the express to get out of the way */
				recv(LOC_TR_SOCKET , &from , msg , sizeof(msg));
				exp_cur = exp_nxt;
				exp_nxt = msg[1];


			}	//end of collision detection

			/* start the train up again */
			xmit_spi(TR_CTRL , LOCAL , cur_path.state , LOC_TR_SOCKET);

			if(cur_path.sw_num != 0)
			{	/* switch needs to be turned */
				switch_track(cur_path.sw_num , TURNED , LOC_TR_SOCKET);
			}

			/* acquire next path */
			msg[0] = FULL_PATH;
			msg[1] = nxt_path.ID;
			msg[2] = LOCAL;
			send(CONDUCTOR_SOCKET , LOC_TR_SOCKET , msg , sizeof(msg));

			/* update the terminal with the new position of the train */
			msg[1] = prv_path.ID;
			msg[2] = NO_TRAIN;
			send(DISPLAY_SOCKET , LOC_TR_SOCKET , msg , sizeof(msg));

			msg[1] = cur_path.ID;
			msg[2] = LOCAL;
			send(DISPLAY_SOCKET , LOC_TR_SOCKET , msg , sizeof(msg));


		}
		else if(hs_indx == END_OF_PATH)
		{	/* at end of path */

			/* change current path to the next path */
			prv_path = cur_path;
			cur_path = nxt_path;
			nxt_path = paths[route[dst_indx]];
			dst_indx = (dst_indx + 1) % NUM_LOC_PATHS;
			hs_indx  = -1;

			/* set speed and direction for the path */
			xmit_spi(TR_CTRL , LOCAL , cur_path.state , LOC_TR_SOCKET);

			if(prv_path.sw_num != 0)
			{	/* switch needs to be set back to straight */
				switch_track(prv_path.sw_num , STRAIGHT , LOC_TR_SOCKET);
			}

			if(cur_path.ID == 0)
			{	/* local was just stopped */

				/* update the terminal with the new position of the train */
				msg[0] = FULL_PATH;
				msg[1] = prv_path.ID;
				msg[2] = NO_TRAIN;
				send(DISPLAY_SOCKET , LOC_TR_SOCKET , msg , sizeof(msg));

				/* delay to let all the passengers off */
				delay(2048);

				/* set the next path */
				prv_path = cur_path;
				cur_path = nxt_path;
				nxt_path = paths[route[dst_indx]];
				dst_indx = (dst_indx + 1) % NUM_LOC_PATHS;

				/* set speed and direction for the path */
				xmit_spi(TR_CTRL , LOCAL , cur_path.state , LOC_TR_SOCKET);

				/* acquire next path */
				msg[0] = FULL_PATH;
				msg[1] = cur_path.ID;
				msg[2] = LOCAL;
				send(CONDUCTOR_SOCKET , LOC_TR_SOCKET , msg , sizeof(msg));

			}	/* end of dwell */
		}	/* end of 'end of path' check */

		hs_indx++;

		msg[0] = NEW_TR_HS;
		msg[1] = cur_path.hs[hs_indx];
		msg[2] = LOCAL;
		send(CONDUCTOR_SOCKET , LOC_TR_SOCKET , msg , sizeof(msg));
	}
}

/*================================================================================*
 * initializes the train set by resetting the error queue, acknowledging all hall*
 * sensors and reseting the HS queue											  *
 * ===============================================================================*/
void train_control_init(void)
{
	int i;

	nice(PRI0); //to run after display_init() and before train controllers

	bind(TR_INIT_SOCKET);

	/* acknowledge all hall sensors */
	xmit_spi(HS_ACKALL , EMPTY_ARG , EMPTY_ARG , TR_INIT_SOCKET);

	/* hall sensor Queue reset */
	xmit_spi(HS_QRST , EMPTY_ARG ,EMPTY_ARG , TR_INIT_SOCKET);

	/* error queue reset */
	xmit_spi(ERR_QRST , EMPTY_ARG ,EMPTY_ARG , TR_INIT_SOCKET);

	for( i = 1 ; i <= 6 ; i++)
	{
		switch_track(i , STRAIGHT , TR_INIT_SOCKET);
	}

	terminate();
}


/*================================================================================*
 * Receives interrupt from GPIO notifying the HAL server to send a request to     *
 * The Atmel. It then receives the HAL sensor triggered, and sends that character *
 * to the controller who finds out which train triggered the HS.			      *
 * ===============================================================================*/
void hal_server() {

	unsigned packet_source;
	unsigned intr_pkt;
	unsigned hs_msg[2];
	unsigned hs_num;

	struct spi_pkt *hs_recv;
	struct spi_pkt xmit_pkt;

	bind(HAL_SOCKET); // HAL server assigned first socket

	while(1) {
		/* wait for hall sensor interrupt, will just be a wake up call (no args) */
		recv(HAL_SOCKET, &packet_source, &intr_pkt, sizeof(intr_pkt));

		/* poll for HS number  */
		xmit_pkt.cmd  = HS_INPUT;
		xmit_pkt.arg1 = EMPTY_ARG;
		xmit_pkt.arg2 = EMPTY_ARG;
		hs_recv = (struct spi_pkt *)xmit_spi_pkt(xmit_pkt);

		hs_num = hs_recv->arg1;

		hs_msg[0] = TRIGGERED_HS;
		hs_msg[1] = hs_num;			//set hall sensor number

		/* send the triggered hall sensor to the train conductor */
		send(CONDUCTOR_SOCKET, HAL_SOCKET, &hs_msg, sizeof(hs_msg));

		/* acknowledge the triggered hall sensor */
		xmit_spi(HS_ACK , hs_num , EMPTY_ARG , HAL_SOCKET);
	}
}

/*================================================================================*
 * To switch track values either straight or not                                  *
 * where switch_num is the value of the switch                                    *
 * And state is either STRAIGHT or TURNED                                  	      *
 * ===============================================================================*/
int switch_track(unsigned switch_num, unsigned state , unsigned socket)
{

	unsigned sw_msg[2];			//sending a message to the display proccess

	if(switch_num > NUM_SWITCHES){
		return false;
	}

	/* set the new state for the switch */
	xmit_spi(SW_CTRL , switch_num , state , socket);

	switches[switch_num-1].state = state;		//maintain state of switch in global array

	sw_msg[0] = SW_STATE_CHANGE;
	sw_msg[1] = switch_num;
	send(DISPLAY_SOCKET , socket , &sw_msg , sizeof(sw_msg));

	return true;
}

/*================================================================================*
 * the controller takes in hall sensor numbers that have been triggered. it knows *
 * the state of the track (i.e. where both trains are) and can therefore determine*
 * which train triggered the HS. it send the hall sensor to the respective train  *
 * controller so it can do what it wants with it								  *
 * ===============================================================================*/
void TheFatController(void)
{
	unsigned from;
	unsigned msg[4];

	unsigned hs_num;
	int blocked_train = NO_TRAIN;
	int saved_hs = 0;

	/* current and next paths for each train */
	unsigned exp_cur = 2;	//9  (2)
	unsigned exp_nxt = 3;	//3  (3)
	unsigned exp_hs  = 14;	//24 (14)

	unsigned loc_cur = 3;	//6
	unsigned loc_nxt = 7;	//9
	unsigned loc_hs  = 8;	//19

	bind(CONDUCTOR_SOCKET);

	while(1){
		recv(CONDUCTOR_SOCKET, &from , &msg , sizeof(msg));
		switch(msg[0]){
		case TRIGGERED_HS:
			hs_num = msg[1];					//get the triggered hall sensor

			if(hs_num == loc_hs){
				msg[0] = TRIGGERED_HS;
				msg[1] = hs_num;		//send hs number
				msg[2] = exp_cur;		//send express current path
				msg[3] = exp_nxt;		//send express next path
				send(LOC_TR_SOCKET , CONDUCTOR_SOCKET , msg , sizeof(msg));
			}
			else if(hs_num == exp_hs){
				msg[0] = TRIGGERED_HS;
				msg[1] = hs_num;		//send hs number
				msg[2] = loc_cur;		//send local current path
				send(EXP_TR_SOCKET , CONDUCTOR_SOCKET , msg , sizeof(msg));
			}
			break;
		case FULL_PATH:
			if(msg[2] == EXPRESS){
				exp_cur = exp_nxt;
				exp_nxt = msg[1];		//set express next path to the one just acquired
				if(blocked_train == LOCAL)
				{	/* send new express position to local so it can check if its out of collision area */
					send(LOC_TR_SOCKET , CONDUCTOR_SOCKET , msg , sizeof(msg));
					blocked_train = NO_TRAIN;
					loc_hs = saved_hs;
				}
			}else if(msg[2] == LOCAL){
				loc_cur = loc_nxt;
				loc_nxt = msg[1];		//set local next path to the one just acquired
				if(blocked_train == EXPRESS)
				{	/* wake up the express train. local is out of collision area */
					send(EXP_TR_SOCKET , CONDUCTOR_SOCKET , msg , sizeof(msg));
					blocked_train = NO_TRAIN;
					exp_hs = saved_hs;
				}
			}
			break;
		case NEW_TR_HS:
			if(msg[2] == LOCAL){
				loc_hs = msg[1];
			}else if(msg[2] == EXPRESS){
				exp_hs = msg[1];
			}
			break;
		case COLLISION:
			blocked_train = msg[2];	//set the blocked train to the one that avoided the collision
			if(blocked_train == EXPRESS){
				exp_cur = 0;
				saved_hs = exp_hs;
				exp_hs = 0;						//set the exp_hs to zero so the trains don't confuse who's who
			}else if(blocked_train == LOCAL){
				loc_cur = 0;
				saved_hs = 0;
				loc_hs = 0;
			}
			break;
		}
	}
}


