/*================================================================================*
 * TITLE:	mem_alloc.h														  	  *
 * PURPOSE:	the purpose of this header file is to allocate and deallocate memory  *
 *			in the heap when requested by the kernel							  *
 * STRUCTS: mem_block -	a single block of memory. can be in a range of 1/8 Kib	  *
 *						to 2Kib													  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Sep 17, 2016														  *
 * ===============================================================================*/


#ifndef MEM_MANAGER_H_
#define MEM_MANAGER_H_

#include "kernel.h"

#define SYSMEM_ENTRY 0x20004000	//system memory entry point
#define NUM_BLOCKS  248			//258 blocks of varying sizes that compose the heap

struct mem_block {
	unsigned long entry_pnt;				//memory point where this block starts
	int size;					//in bytes
	bool free;					//true if unalloacted, false is allocated
};

extern struct mem_block heap[];

extern void init_mem(void);

extern int allocate(int num_bytes);

extern bool deallocate(int addr);


#endif /* MEM_MANAGER_H_ */
