#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "timm.h"

extern void print_map(void);

int main()
{
    char* pt = (char*)malloc(4096);
    #ifdef STATICALLY_ALLOCATED_HEAP
    heap_init();
    #endif // STATICALLY_ALLOCATED_HEAP
    #ifdef DYNAMICALLY_ALLOCATED_HEAP
    heap_init(pt, 4096, 8);
    #endif // DYNAMICALLY_ALLOCATED_HEAP


    int* my_ptr = (int*)heap_new(12);
    printf("%pt, %p\n", pt, my_ptr);
    //*my_ptr = 0x11223344;
    int* my_ptr1 = (int*)heap_new(16);
    //*my_ptr1 = 0x66778899;
    heap_delete(my_ptr, 12);
    double* my_ptr2 = (double*)heap_new(40);

    print_map();
    LenT freeBlk =  heap_get_free_blocks();
    LenT blkCnt = heap_get_blocks_count();
    printf("\n\n\t%d blocks of %d are free\n\n", freeBlk, blkCnt);

    heap_delete(my_ptr1, 16);
    heap_delete(my_ptr2, 40);
    print_map();
    freeBlk =  heap_get_free_blocks();
    blkCnt = heap_get_blocks_count();
    printf("\n\n\t%d blocks of %d are free\n\n", freeBlk, blkCnt);


    getchar();
    free(pt);
    return 0;
}
