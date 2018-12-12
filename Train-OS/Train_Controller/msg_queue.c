
/*
 * msg_queue.c
 *
 *  Created on: 2016-11-22
 *      Author: Victor Maslov
 *
 *      Implementation of Receiving and sending messages via a pseudo-socket interface
 *      A single process can bind to one socket, no more.
 *      This serves as a circular queue of packets containing the msg_packet structure
 *
 *
 *
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "UART.h"
#include "kernel.h"
#include "processcalls.h"
#include "msg_queue.h"

#define SOCK_AMOUNT 20
#define MSG_QUEUE_SIZE 30
#define MAX_PAYLOAD_SIZE 64

struct sock socket_list[SOCK_AMOUNT];


/* Kernel entry point for receiving messages  */
void k_recv_message(struct kcallargs *kcall) {
	/* Kernel entry point for receiving messages
	 * takes in kcall pointer from R7 of the process  */

	/* creates a msg_packet pointer assigned to kcall arg */
	struct msg_packet *recv_packet;
	recv_packet = (struct msg_packet *) kcall->arg1;
	unsigned temp_tail;

	/* set temporary tail for simplification */
	temp_tail = (socket_list[recv_packet->dst].sock_tail + 1) % MSG_QUEUE_SIZE;

	/* Check to see if PCB is binded to said socket */
	if (socket_list[recv_packet->dst].binded_pcb != running ) {
		kcall->rtnvalue = (unsigned)-1;
		return;
	}

	/* Check to see if empty queue */
	else if(temp_tail == socket_list[recv_packet->dst].sock_head){

		/* If empty , set a spin lock, and dequeue pcb from WTR,
		 * Trigger a PendSV to change running process after leaving kernel call */

		socket_list[recv_packet->dst].spin_lock = true;
		dequeue_pcb(socket_list[recv_packet->dst].binded_pcb, socket_list[recv_packet->dst].binded_pcb->priority);
		NVIC_INT_CTRL_R |= TRIGGER_PENDSV;	//trigger pendSV
		kcall->rtnvalue = false;
	}
	else{
		/* Copy size bytes from message queue to location specified by msg */
		memcpy(recv_packet, &socket_list[recv_packet->dst].msg_queue[temp_tail], sizeof(struct msg_packet));
		socket_list[recv_packet->dst].sock_tail = temp_tail;
		kcall->rtnvalue = true;
	}

	return;

}

void k_send_message(struct kcallargs *kcall) {
	/* Send kernel call entry point */

	/* Creates pointer for msg_packet */
	struct msg_packet *send_packet;

	/* Assigned Arg1 holding address of msg_packet */
	send_packet =  (struct msg_packet*) kcall->arg1;

	/* Assigned Temporary head to socket head */
	unsigned temp_head = socket_list[send_packet->dst].sock_head;


	/* These are done after all the checks
	 * Copy message from queue to send_packet */
	memcpy(&socket_list[send_packet->dst].msg_queue[temp_head],send_packet, sizeof(struct msg_packet));

	/* Circular list */
	socket_list[send_packet->dst].sock_head = (++temp_head) % MSG_QUEUE_SIZE;

	/* If the spin lock was set for the queue set the spin lock to false and POST the PCB binded to the socket */
	if ( socket_list[send_packet->dst].spin_lock == true ) {
		socket_list[send_packet->dst].spin_lock = false;
		/* Back to running queue */
		enqueue_pcb(socket_list[send_packet->dst].binded_pcb, socket_list[send_packet->dst].binded_pcb->priority);
		NVIC_INT_CTRL_R |= TRIGGER_PENDSV;	//trigger pendSV
		/* We know the receiver is currently blocked */
	}

}

void k_bind_message(struct kcallargs *kcall) {

	if ( ( socket_list[kcall->arg1].binded_pcb == 0) && ( kcall->arg1 < SOCK_AMOUNT && running -> binded_socket == -1  ) ) {

			/* Socket open for binding and valid */
			socket_list[kcall->arg1].binded_pcb = running;
			running->binded_socket = kcall->arg1; // Assign binded socket value
			kcall->rtnvalue = true;
			socket_list[kcall->arg1].sock_head = 1;
			socket_list[kcall->arg1].sock_tail = 0;
	}
	else {
		/* Already assigned or invalid number */
		kcall->rtnvalue = false;
	}

}

/* Application call HERE for now */

int send(unsigned dst, unsigned src, void *msg, unsigned size) {
	/* Process call for sending a packet
	 * USAGE : 	send(unsigned destination socket , unsigned source_socket , *msg , sizeof(msg));
	 *
	 * builds a msg_packet, sends the adress of the msg_packet down to the kernel which handles the enqueuing to the
	 * right queue
	 *
	 * */

	volatile struct kcallargs sendarg;

	/* Check for MAX payload size available to the application */
	if ( size > MAX_PAYLOAD_SIZE) {
		/* resize the message there for truncates it */
		size = MAX_PAYLOAD_SIZE;
	}

	/* Creates a packet to be sent */
	struct msg_packet send_packet;

	/* Sets values for destination, source, and size */
	send_packet.dst = dst;
	send_packet.src = src;
	send_packet.size = size;


	/* Copies payload into structure */
	memcpy(&send_packet.payload, msg, size );

	/* General Process call set up for kernel to execute kernal function */
	sendarg.code = SEND;
	sendarg.arg1 = (unsigned)&send_packet;
	assignR7((unsigned long) &sendarg);
	SVC();
	/* returns value from kernel */
	return sendarg.rtnvalue;

}


int recv(unsigned dst, unsigned *src, void *msg, unsigned size) {
	/* Application call for Receiving messages from socket
	 * USAGE :  */

	volatile struct kcallargs recvarg;

	/* Check for max payload size and sets to maximum if to large */
	if ( size > MAX_PAYLOAD_SIZE) {
		size = MAX_PAYLOAD_SIZE;
	}

	/* Builds message packet to be copied from queue */
	struct msg_packet recv_packet;

	/* Sets destination and size to receive packet */
	recv_packet.dst = dst;
	recv_packet.size = size;

	/* Sets values for Kernel to handle call */
	recvarg.code = RECV;
	recvarg.arg1 = (unsigned)&recv_packet;
	recvarg.rtnvalue = false;

	assignR7((unsigned long) &recvarg);
	SVC();

	/* If Returned value from kernel is false, we know queue was empty. process
	 * will be blocked here until a POST was made using a send to said queue
	 * Another SVC will then be executed to attempt a pull from that queue */

	if (recvarg.rtnvalue == false) {
	SVC();
	}

	/* Copy payload and send upstairs */
	memcpy(msg, &recv_packet.payload, size );
	*src = recv_packet.src;
	return recv_packet.size;

}


int bind(unsigned socket) {

		volatile struct kcallargs bindarg;
		bindarg.code = BIND;
		bindarg.arg1 = socket;
		assignR7((unsigned long) &bindarg);
		SVC();
		return bindarg.rtnvalue;
}



