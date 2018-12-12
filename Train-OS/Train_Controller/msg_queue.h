/*
 * msg_queue.h
 *
 *  Created on: 2016-11-24
 *      Author: Victor Maslov
 */

#ifndef MSG_QUEUE_H_
#define MSG_QUEUE_H_


#define SOCK_AMOUNT 20
#define MSG_QUEUE_SIZE 30
#define MAX_PAYLOAD_SIZE 64

struct msg_packet {
	unsigned dst;
	unsigned src;
	unsigned size;
	char payload[MAX_PAYLOAD_SIZE];
};

struct sock {
	struct pcb *binded_pcb;
	unsigned spin_lock;
	struct msg_packet msg_queue[MSG_QUEUE_SIZE];
	unsigned sock_head;
	unsigned sock_tail;
};

enum socket_assign {
	HAL_SOCKET,					//used by the hall sensor server
	CONDUCTOR_SOCKET,			//used by TheFatController
	EXP_TR_SOCKET,				//used by the express train controller
	LOC_TR_SOCKET,				//used by the local train controller
	TR_INIT_SOCKET,				//used by the train initializer
	DISPLAY_SOCKET,				//used by the display controller
	INPUT_SOCKET,				//used by the
	SWITCH_SOCKET,				//used by the switch controller
	ERR_SOCKET,					//used by the error handler
	XMIT_SOCKET					//used by the spi controller
};

/* these are the possible messages that can be sent between processes *
 * if arguments are required for the message they are packaged into a *
 * string with the enumeration as the first character				  *
 */
enum msgs{
	WAKEUP,					//no arguments needed
	TRIGGERED_HS,			//arg1 = HS  , arg2 = train
	SW_STATE_CHANGE,		//arg1 = SW
	FULL_PATH,				//arg1 = path, arg2 = train
	EMPTY_PATH,				//arg1 = path
	NEW_TR_HS,				//arg1 = HS  , arg2 = train
	COLLISION,				//arg1 = train
	TX_SPI,					//arg1 =  cmd , arg2 = arg1 , arg3 = arg2
	TX_ERR					//no arguments
};

extern struct sock socket_list[SOCK_AMOUNT];

extern void k_recv_message(struct kcallargs *);
extern void k_send_message(struct kcallargs *);
extern void k_bind_message(struct kcallargs *);

extern int recv(unsigned, unsigned *, void *, unsigned);
extern int send(unsigned, unsigned, void *, unsigned);
extern int bind(unsigned);

#endif /* MSG_QUEUE_H_ */
