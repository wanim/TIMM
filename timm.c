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
    char    map_table[H_MAP_SIZE];
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
uint8_t timm_init(void* loc, LenT siz, BnumT bnum)
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
    while(i < H_MAP_SIZE)
    {
        MHeap.map_table[i] = FREE_BLOCK;
        i++;
    }
    return 1;
}
#endif /* DYNAMICALLY_ALLOCATED_HEAP */

#ifdef STATICALLY_ALLOCATED_HEAP
void timm_init()
{
    LenT i = 0;
    while( i < H_MAP_SIZE)
    {
        MHeap.map_table[i] = FREE_BLOCK;
        i++;
    }
}
#endif /* STATICALLY_ALLOCATED_HEAP */

void* timm_new(LenT siz)
{
    char* blk_st;
    LenT len;
    LenT offset;
    char* ptr = MHeap.map_table;

    if(siz)
    {
        siz = (siz % B_PER_BLOCK) ? ((siz / B_PER_BLOCK) + 1) : (siz / B_PER_BLOCK);
        while( ptr < (char*)H_MAP_END)
        {
            while(*ptr != FREE_BLOCK)       /* look for the 1st free block */
            {
                if(ptr >= (char*)H_MAP_END)
                {
                    return NULL;            /* no free blocks */
                }
                ptr++;
            }

            offset = ptr - MHeap.map_table;
            blk_st = ptr;                   /* start of free block */
            len = 0;
            while(*ptr == FREE_BLOCK && ptr < (char*)H_MAP_END)  /* check if block is long enough */
            {
                len++;
                if(len == siz)
                {
                    while(len--)            /* mark block as used */
                    {
                        *blk_st++ = RESERVED_BLOCK;
                    }
                    return ((char*)HEAP_START + offset * B_PER_BLOCK); /* memory has been allocated */
                }
                ptr++;
            }
        }
    }
    return NULL;
}

void timm_delete(void* dst, LenT siz)
{
    if(siz && dst >= (void*)HEAP_START && dst < (void*)HEAP_END)
    {
        LenT offset;
        char* map_ptr;

        offset = (dst - (void*)HEAP_START) / B_PER_BLOCK;    /* get offset in memory map */
        siz = (siz % B_PER_BLOCK) ? ((siz / B_PER_BLOCK) + 1) : (siz / B_PER_BLOCK);
        map_ptr = MHeap.map_table + offset;

        while(siz-- && map_ptr < (char*)H_MAP_END)
        {
            *map_ptr++ = FREE_BLOCK;
        }
    }
}

LenT timm_get_blocks_count()
{
    return H_MAP_SIZE;
}

int8_t timm_get_block_status(void* ptr)
{
    if(ptr >= (void*)H_MAP_START && ptr < (void*)H_MAP_END)
    {
        return (*((char*)ptr) == FREE_BLOCK)? 0 : 1;
    }
    return -1;
}

LenT timm_get_free_blocks()
{
    LenT cnt = 0;
    LenT i = 0;

    while(i < H_MAP_SIZE)
    {
        if(MHeap.map_table[i] == FREE_BLOCK)
        {
            cnt++;
        }
        i++;
    }
    return cnt;
}

void* timm_virt2real(void* ptr)
{
    LenT offset;
    if(ptr >= (void*)H_MAP_START && ptr < (void*)H_MAP_END)
    {
        offset = ptr - H_MAP_START;
        ptr = (char*)HEAP_START + offset * B_PER_BLOCK;
        return ptr;
    }
    return NULL;
}

void print_map()        /* bonus utility for printing memory map to standard output */
{
    uint16_t row = 0;
    uint16_t len = 0;

    printf("\t0\t8\t16\t24");
    while( len < H_MAP_SIZE)
    {
        if((len % 32) == 0)
        {
            printf("\n %u\t", row);
            row += 32;
        }
        printf("%c" ,(MHeap.map_table[len])? '1': '0');
        len++;
    }
}
