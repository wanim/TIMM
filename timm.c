/**
 * @file	timm.c
 * @brief	This file contain source codes for a Tiny Memory Manager (TIMM)
 *
 * Created 2015-04-18 by Kyrylo Nykytuk aka wanim
 */


#include "timm.h"
#include <stdio.h>

static struct
{
#ifdef STATICALLY_ALLOCATED_HEAP
    char    map_table[HEAP_MAP_SIZE];
#endif /* STATICALLY_ALLOCATED_HEAP */
#ifdef DYNAMICALLY_ALLOCATED_HEAP
    char    map_table[MAX_MAP_SIZE];
    void*   hstart;
    void*   hend;
    void*   mend;
    LenT    msize;
    BnumT   blcks;
#endif /* DYNAMICALLY_ALLOCATED_HEAP */
} MHeap;

#ifdef DYNAMICALLY_ALLOCATED_HEAP
uint8_t heap_init(void* loc, LenT siz, BnumT bnum)
{
    if(siz/bnum > MAX_MAP_SIZE || loc == 0)
    {
        return 0;
    }
    MHeap.hstart = loc;
    MHeap.hend   = (char*)loc + siz;
    MHeap.blcks  = bnum;
    MHeap.msize  = siz/bnum;
    MHeap.mend   = MHeap.map_table + MHeap.msize;
    LenT i = 0;
    while(i < HEAP_MAP_SIZE)
    {
        MHeap.map_table[i] = FREE_BLOCK;
        i++;
    }
    return 1;
}
#endif /* DYNAMICALLY_ALLOCATED_HEAP */

#ifdef STATICALLY_ALLOCATED_HEAP
void my_heap_init()
{
    LenT i = 0;
    while( i < HEAP_MAP_SIZE)
    {
        MHeap.map_table[i] = FREE_BLOCK;
        i++;
    }
}
#endif /* STATICALLY_ALLOCATED_HEAP */

void* heap_new(LenT siz)
{
    char* blk_st;
    LenT len;
    LenT offset;
    char* ptr = MHeap.map_table;

    if(siz)
    {
        siz = (siz % BYTES_PER_BLOCK) ? ((siz / BYTES_PER_BLOCK) + 1) : (siz / BYTES_PER_BLOCK);
        while( ptr < (char*)HEAP_MAP_END)
        {
            while(*ptr != FREE_BLOCK)       /* look for the 1st free block */
            {
                if(ptr >= (char*)HEAP_MAP_END)
                {
                    return NULL;            /* no free blocks */
                }
                ptr++;
            }

            offset = ptr - MHeap.map_table;
            blk_st = ptr;                   /* start of free block */
            len = 0;
            while(*ptr == FREE_BLOCK && ptr < (char*)HEAP_MAP_END)  /* check if block is long enough */
            {
                len++;
                if(len == siz)
                {
                    while(len--)            /* mark block as used */
                    {
                        *blk_st++ = RESERVED_BLOCK;
                    }
                    return ((char*)HEAP_START + offset*BYTES_PER_BLOCK); /* memory has been allocated */
                }
                ptr++;
            }
        }
    }
    return NULL;
}

void heap_delete(void* dst, LenT siz)
{
    if(siz && dst >= (void*)HEAP_START && dst < (void*)HEAP_END)
    {
        LenT offset;
        char* map_ptr;

        offset = ((char*)dst - (char*)HEAP_START) / BYTES_PER_BLOCK;    /* get offset in memory map */
        siz = (siz % BYTES_PER_BLOCK) ? ((siz / BYTES_PER_BLOCK) + 1) : (siz / BYTES_PER_BLOCK);
        map_ptr = MHeap.map_table + offset;

        while(siz-- && map_ptr < (char*)HEAP_MAP_END)
        {
            *map_ptr++ = FREE_BLOCK;
        }
    }
}

LenT heap_get_blocks_count()
{
    return HEAP_MAP_SIZE;
}

int8_t get_block_status(void* ptr)
{
    if(ptr >= (void*)MHeap.map_table && ptr < HEAP_MAP_END)
    {
        return (*((char*)ptr) == FREE_BLOCK)? 0 : 1;
    }
    return -1;
}

LenT heap_get_free_blocks()
{
    LenT cnt = 0;
    LenT i = 0;

    while(i < HEAP_MAP_SIZE)
    {
        if(MHeap.map_table[i] == FREE_BLOCK)
        {
            cnt++;
        }
        i++;
    }
    return cnt;
}

void print_map()        /* bonus utility for printing memory map to standard output */
{
    uint16_t row = 0;

    printf("\t0\t8\t16\t24");
    for(uint16_t i = 0; i < HEAP_MAP_SIZE; i++)
    {
        if((i % 32) == 0)
        {
            printf("\n %u\t", row);
            row += 32;
        }
        printf("%c" ,(MHeap.map_table[i])? '1': '0');
    }
}
