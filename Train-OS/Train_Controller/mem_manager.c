/*================================================================================*
 * TITLE:	mem_alloc.c														  	  *
 * PURPOSE:	the purpose of this source file is to allocate and deallocate memory  *
 *			in the heap when requested by the kernel							  *
 * GLOBALS: heap[]	-	an array of 248 memory blocks that have varying sizes	  *																																																																																ying size	  *
 *						This is the array that allocate() and deallocate() use to *
 *						set and free memory respectively.  						  *
 * AUTHOR:	Victor Maslov, Nick Stanwood										  *
 * CREATED:	Sep 17, 2016														  *
 * ===============================================================================*/

#include "mem_manager.h"

#include <stdio.h>
#include <math.h>


struct mem_block heap[NUM_BLOCKS];

extern void init_mem(void)
{
	int mem_loc = SYSMEM_ENTRY;
	int i = 0;
	
	/* initialize 128 blocks in memory of size 0.125Kib */
	for(i=0 ; i < 128 ; i++){
		heap[i].entry_pnt = mem_loc;
		heap[i].free = true;
		mem_loc += 0x80;
	}

	/* initialize 64 blocks in memory of size 0.25Kib */
	for( ; i < 192 ; i++){
			heap[i].entry_pnt = mem_loc;
			heap[i].free = true;
			mem_loc += 0x100;
	}

	/* initialize 32 blocks in memory of size 0.5Kib */
	for( ; i < 224 ; i++){
			heap[i].entry_pnt = mem_loc;
			heap[i].free = true;
			mem_loc += 0x200;
	}
	/* initialize 16 blocks in memory of size 1.0Kib */
	for( ; i < 240 ; i++){
			heap[i].entry_pnt = mem_loc;
			heap[i].free = true;
			mem_loc += 0x400;
	}

	/* initialize 8 blocks in memory of size 2.0Kib */
	for( ; i < 248 ; i++){
			heap[i].entry_pnt = mem_loc;
			heap[i].free = true;
			mem_loc += 0x800;
	}
}

int allocate(int num_bytes)
{
	bool allocated = false;
	bool error = false;
	int heap_loc;				//current block in the heap
	int alloc_mem_entry;		//entry point for free block

	if(num_bytes <= 0){				/* invalid size */
		error = true;
		alloc_mem_entry = -1;
	}else if(num_bytes <= 128){		/* block size to allocate is .125Kib */
		heap_loc = 0;
	}else if(num_bytes <= 256){		/* block size to allocate is .25Kib */
		heap_loc = 128;
	}else if(num_bytes <= 512){		/* block size to allocate is .5Kib */
		heap_loc = 192;
	}else if(num_bytes <= 1024){	/* block size to allocate is 1.0Kib */
		heap_loc = 224;
	}else if(num_bytes <= 2048){	/* block size to allocate is 2.0Kib */
		heap_loc = 240;
	}else{						/* need to allocate more than one contigous block */
		error = true;
	}
	
	/* this while() loop is executed if the amount requested 	*
	 * can fit into a single block of memory					*/
	while(!allocated && !error){
		if(heap[heap_loc].free){
			heap[heap_loc].free = false;
			allocated = true;
			alloc_mem_entry = heap[heap_loc].entry_pnt;
		}else if(++heap_loc > 248){		//looked through all of memory
			error = true;
			alloc_mem_entry = -1;
		}
	}
	
	return alloc_mem_entry;
}

bool deallocate(int addr)
{
	int i;
	bool error = true; 
	
	/* search through mem for the address */
	for(i = 0 ; i < NUM_BLOCKS ; i++){
		if(heap[i].entry_pnt == addr && !heap[i].free){
			heap[i].free = true;
			error = false;
		}
	}

	return error;
}
