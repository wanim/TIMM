#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "timm.h"

extern void print_map(void);

int main()
{
    char* pt = (char*)malloc(4096);
    #ifdef STATICALLY_ALLOCATED_HEAP
    timm_init();
    #endif // STATICALLY_ALLOCATED_HEAP
    #ifdef DYNAMICALLY_ALLOCATED_HEAP
    timm_init(pt, 4096, 8);
    #endif // DYNAMICALLY_ALLOCATED_HEAP


    int* my_ptr = (int*)timm_new(12);
    //printf("%pt, %p\n", pt, my_ptr);
    //*my_ptr = 0x11223344;
    int* my_ptr1 = (int*)timm_new(16);
    //*my_ptr1 = 0x66778899;
    timm_delete(my_ptr, 12);
    double* my_ptr2 = (double*)timm_new(40);

    print_map();
    LenT freeBlk = timm_get_free_blocks();
    LenT blkCnt = timm_get_blocks_count();
    printf("\n\n\t%d blocks of %d are free\n\n", freeBlk, blkCnt);

    timm_delete(my_ptr1, 16);
    timm_delete(my_ptr2, 40);
    print_map();
    freeBlk =  timm_get_free_blocks();
    blkCnt = timm_get_blocks_count();
    printf("\n\n\t%d blocks of %d are free\n\n", freeBlk, blkCnt);


    getchar();
    free(pt);
    return 0;
}
