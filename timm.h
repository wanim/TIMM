/**
* @file   timm.h
* @brief  Header file for a Tiny Memory Manager (TIMM)
* @brief  This file contains description, function prototypes, constants and other definitions.
*
*  TIMM is a memory manager for embedded devices.
*  Its main purpose is to allocation/deallocation of memory blocks of customizable sizes.
*  TIMM is designed to be very quickly and efficient since it uses map table of free blocks
*  and uses first found free block of appropriate size, not wasting memory and not leading
*  to excessive fragmentation.
*  TIMM is implemented to be high portable and supportable by different platforms.
*  Not excessive set of options make it possible for TIMM to be fully customizable.
*  It is possible easily to choose between static (better performance)
*  and dynamic (configurable at runtime) models of TIMM.
*  This enables possibility to run TIMM even on budget 8-bit microcontrollers.

* Created 2015-04-18 by Kyrylo Nykytuk aka wanim
*/

#ifndef MHEAP_H_INCLUDED
#define MHEAP_H_INCLUDED
#include <stdint.h>

/** ---------------Common defines --------------- */
/** Define only one of next */
#define DYNAMICALLY_ALLOCATED_HEAP
/*#define STATICALLY_ALLOCATED_HEAP*/
/** --------------------------------------------- */
typedef uint16_t LenT;  /* type for size of managed by TIMM memory */
typedef uint8_t BnumT;  /* type for holding number of bytes in block */
#define FREE_BLOCK      0x00U
#define RESERVED_BLOCK  0xFFU
#define H_MAP_START     (void*)MHeap.map_table
#ifndef NULL
#define NULL ((void *)0)
#endif /* NULL */
/** --------------------------------------------- */

#if defined DYNAMICALLY_ALLOCATED_HEAP && defined STATICALLY_ALLOCATED_HEAP
#error Please define only one between DYNAMICALLY_ALLOCATED_HEAP and STATICALLY_ALLOCATED_HEAP
#endif

#if !defined DYNAMICALLY_ALLOCATED_HEAP && !defined STATICALLY_ALLOCATED_HEAP
#error Please define one between DYNAMICALLY_ALLOCATED_HEAP and STATICALLY_ALLOCATED_HEAP
#endif

/** --------Defines for static mode only--------- */
#ifdef STATICALLY_ALLOCATED_HEAP
#define HEAP_START      0x903930
#define HEAP_SIZE       4096
#define B_PER_BLOCK     8
#define HEAP_END        ((HEAP_START)+(HEAP_SIZE))
#define H_MAP_SIZE      ((HEAP_SIZE)/(B_PER_BLOCK))
#define H_MAP_END       ((HEAP_START) + (H_MAP_SIZE))
#endif /* STATICALLY_ALLOCATED_HEAP */
/** --------------------------------------------- */

/** --------Defines for dynamic mode only-------- */
#ifdef DYNAMICALLY_ALLOCATED_HEAP
#define MAX_MAP_SIZE    2048
#define HEAP_START      MHeap.hstart
#define HEAP_END        MHeap.hend
#define H_MAP_SIZE      MHeap.msize
#define H_MAP_END       MHeap.mend
#define B_PER_BLOCK     MHeap.blcks
#endif /* DYNAMICALLY_ALLOCATED_HEAP */
/** --------------------------------------------- */


/**
* @brief	Heap initialization function used when static mode is defined
* @param	Accepts no parameters,all needed definitions are in header file
* @pre		Defines should contain valid data
* @return	Returns no value
* @remark	Prepares memory map block by initializing with zero.
*/
#ifdef STATICALLY_ALLOCATED_HEAP
void timm_init();
#endif /* STATICALLY_ALLOCATED_HEA */

/**
* @brief	Heap initialization function used when dynamic mode is defined
* @param	[loc]   Address of the beginning of the heap
* @param	[size]  Length in bytes of heap
* @param	[bnum]  Block size in bytes
* @return	Returns 1 if is initialization was successful, or 0 if failed
* @remark	Prepares memory map block by initializing with zero.
*/
#ifdef DYNAMICALLY_ALLOCATED_HEAP
uint8_t timm_init(void* loc, LenT siz, BnumT bnum);
#endif /* DYNAMICALLY_ALLOCATED_HEAP */

/**
* @brief	Function for memory allocation in heap.
* @param	[siz]   Size in bytes
* @return	Pointer to allocated memory or null pointer if memory allocation has failed
* @pre		Heap initialization was successful
* @remark	Scans memory map from the beginning to the end and returns pointer
*	        to the first chunk of memory of appropriate size or null pointer
*	        if there is no free space
*/
void* timm_new(LenT siz);

/**
* @brief	Function for memory deallocation
* @param	[dst]   Start address
* @param	[siz]   Size in bytes
* @return	void
* @warning	Be careful, make sure you do not delete more than needed
* @pre		Heap initialization was successful
* @remark	Checks if provided address is indeed in heap
*/
void timm_delete(void* dst, LenT siz);

/**
* @brief    Function for getting the number of blocks of memory available for memory manager.
*/
LenT timm_get_blocks_count();

/**
* @brief	Function for checking block status
* @param	[ptr]   Points to particular block in memory map segment
* @return	[0]     if block is free
* @return	[1]     if block is reserved
* @return	[-1]    if address is out of memory map range
*/
int8_t timm_get_block_status(void* ptr);

/**
* @brief    Function for getting the number of free blocks of memory.
*/
LenT timm_get_free_blocks();

/**
* @brief	Function that converts virtual address to real
* @param	[ptr]   Holds virtual address
* @return	[0]     if address is out of range
* @return	void*   Real Address
*/
void* timm_virt2real(void* ptr);

#endif /* MHEAP_H_INCLUDED */
