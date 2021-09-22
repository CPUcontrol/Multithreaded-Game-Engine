#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <chrono>

#include "core/allocator.h"
typedef struct heap_header {
    size_t prev_alloc;
    size_t next_color;
} heap_header;
struct heap_free;
typedef struct heap_free heap_free;
struct heap_free {
    heap_header header;

    heap_free* left;
    heap_free* right;
    heap_free* parent;
};
int validateheap(Enj_HeapAllocatorData *h) {
    heap_header* it = (heap_header*)h->start;
    printf("checking heap\n");
    while (it->next_color & ~1) {
        printf("sz%u",(unsigned int)(it->next_color & ~1));
        if(! (it->prev_alloc & 1)) printf(" free");
        printf("\n");
        it = (heap_header*)((char*)it + (it->next_color & ~1));
    }
    return 0;
}
static int countfree(heap_free* f) {
    if (!f) return 0;

    return countfree(f->left) + countfree(f->right) + 1;
}
int main(){
    srand((unsigned int)time(0));

    void *buf = malloc(2048);
    void *buf2 = malloc(2048);
    void *buf3 = malloc(524288);

    Enj_Allocator stack;
    Enj_StackAllocatorData stackdata;

    Enj_InitStackAllocator(&stack, &stackdata, buf, 2048);

    Enj_Allocator pool;
    Enj_PoolAllocatorData pooldata;
    Enj_InitPoolAllocator(&pool, &pooldata, buf2, 2048, sizeof(int));

    Enj_Allocator heap;
    Enj_HeapAllocatorData heapdata;
    Enj_InitHeapAllocator(&heap, &heapdata, buf3, 524288);

    int *p1 = (int *)Enj_Alloc(&stack, sizeof(int));
    int *p2 = (int *)Enj_Alloc(&stack, sizeof(int));

    *p1 = 4;
    *p2 = 65;

    printf("p1: %0#16zx\np2: %0#16zx\n\n*p1: %d\n*p2: %d\n",
     (size_t)p1, (size_t)p2, *p1, *p2);
    Enj_Free(&stack, p2);
    Enj_Free(&stack, p1);

    //Pool test
    int *p3 = (int *)Enj_Alloc(&pool, sizeof(int));
    int *p4 = (int *)Enj_Alloc(&pool, sizeof(int));

    *p3 = 12;
    *p4 = 666;

    printf("p3: %0#16zx\np4: %0#16zx\n\n*p3: %d\n*p4: %d\n",
     (size_t)p3, (size_t)p4, *p3, *p4);
    Enj_Free(&pool, p3);
    Enj_Free(&pool, p4);

    void *arr[100];

    std::chrono::system_clock::time_point now;
    std::chrono::system_clock::time_point end;

    std::chrono::nanoseconds dur;
    unsigned long long time;
    now = std::chrono::system_clock::now();
    for (int i = 0; i < 100; i++){
        arr[i] = malloc(16);
        //*((unsigned int *)arr[i]) = rand();
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time malloc: %u ns\n", (unsigned int)time);

    now = std::chrono::system_clock::now();
    for (int i = 0; i < 100; i++){
        free(arr[99-i]);
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time free: %u ns\n", (unsigned int)time);


    now = std::chrono::system_clock::now();
    for (int i = 0; i < 100; i++){
        arr[i] = Enj_Alloc(&stack, 16);
        //*((unsigned int *)arr[i]) = rand();
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time Enj_Alloc stack: %u ns\n", (unsigned int)time);

    now = std::chrono::system_clock::now();
    for (int i = 0; i < 100; i++){
        Enj_Free(&stack, arr[99-i]);
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time Enj_Free stack: %u ns\n", (unsigned int)time);


    now = std::chrono::system_clock::now();
    for (int i = 0; i < 100; i++){
        arr[i] = Enj_Alloc(&pool, sizeof(int));
        //*((unsigned int *)arr[i]) = rand();
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time Enj_Alloc pool: %u ns\n", (unsigned int)time);

    now = std::chrono::system_clock::now();
    for (int i = 0; i < 100; i++){
        Enj_Free(&pool, arr[99-i]);
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time Enj_Free pool: %u ns\n", (unsigned int)time);

    //Heap



    unsigned char perm[100];
    for (int i = 0; i < 100; i++){
        perm[i] = 3*i % 100;
    }


    void *arr2[10000];
    unsigned int perm2[10000];
    for (int i = 0; i < 10000; i++){
        perm2[i] = 11*i % 10000;
    }
    now = std::chrono::system_clock::now();
    for (int i = 0; i < 10000; i++){
        arr2[i] = malloc(32);
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time malloc random: %u ns\n", (unsigned int)time);

    now = std::chrono::system_clock::now();
    for (int i = 0; i < 10000; i++){
        free(arr2[perm2[i]]);
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time free random: %u ns\n", (unsigned int)time);
    validateheap(&heapdata);
    now = std::chrono::system_clock::now();
    for (int i = 0; i < 10000; i++){
        arr2[i] = Enj_Alloc(&heap, sizeof(int));
        //*((unsigned int *)arr[i]) = rand();
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time Enj_Alloc heap: %u ns\n", (unsigned int)time);


    now = std::chrono::system_clock::now();
    for (int i = 0; i < 10000; i++){

        Enj_Free(&heap, arr2[perm2[i]]);
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time Enj_Free heap: %u ns\n", (unsigned int)time);
    validateheap(&heapdata);
    now = std::chrono::system_clock::now();
    for (int i = 0; i < 10000; i++){
        arr2[i] = Enj_Alloc(&heap, sizeof(int));
        //*((unsigned int *)arr[i]) = rand();
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time Enj_Alloc heap: %u ns\n", (unsigned int)time);


    now = std::chrono::system_clock::now();
    for (int i = 0; i < 10000; i++){

        Enj_Free(&heap, arr2[perm2[i]]);
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();

    printf("Time Enj_Free heap: %u ns\n", (unsigned int)time);
    validateheap(&heapdata);


    /**/
    now = std::chrono::system_clock::now();
    for (int k = 0; k < 1000; k++) {

        for (int i = 0; i < 100; i++) {
            arr[i] = Enj_Alloc(&heap, sizeof(int)*(i+1));
            memset(arr[i], i+1, sizeof(int)*(i+1));
            ((char *)arr[i])[sizeof(int)*(i+1) - 1] = 0;
            //*((unsigned int *)arr[i]) = rand();
        }
        //printf("Removing...\n");
        for (int i = 0; i < 50; i++) {
            char oord = i * 3 % 100;


            for (int j = i; j < 100; j++) {
                char jord = j * 3 % 100;
                char starr[2] = {(char)jord+1, 0};
                if(strspn((char *)arr[jord], starr)
                != sizeof(int)*(jord+1)-1)
                    return 1;
            }

            Enj_Free(&heap, arr[oord]); arr[oord] = nullptr;
            //validateheap(&heapdata);
        }/**/
        for (int i = 49; i >= 0; i--) {
            char oord = i * 3 % 100;
            arr[oord] = Enj_Alloc(&heap, sizeof(int)*(oord+1));
            memset(arr[oord], oord+1, sizeof(int)*(oord+1));
            ((char *)arr[oord])[sizeof(int)*(oord+1) - 1] = 0;

            for (int j = i; j < 100; j++) {
                char jord = j * 3 % 100;
                char starr[2] = {(char)jord+1, 0};
                if(strspn((char *)arr[jord], starr)
                != sizeof(int)*(jord+1)-1)
                    return 1;
            }
            //validateheap(&heapdata);

        }/**/
        //printf("did 50 free\n");
        /**/
        for (int i = 0; i < 100; i++) {
            char oord = i * 3 % 100;


            for (int j = i; j < 100; j++) {
                char jord = j * 3 % 100;
                char starr[2] = {(char)jord+1, 0};
                if(strspn((char *)arr[jord], starr)
                != sizeof(int)*(jord+1)-1)
                    return 1;
            }
            //printf("szfrees %d\n",countfree((heap_free*)heapdata.root));
            Enj_Free(&heap, arr[oord]); arr[oord] = nullptr;
        }/**/

        //if(k==0)validateheap(&heapdata);
    }
    end = std::chrono::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now);
    time = dur.count();
    //validateheap(&heapdata);
    printf("Time 1,000 heaps: %u ns\n", (unsigned int)time);
    /**/
    free(buf);
    free(buf2);
    free(buf3);
    return 0;
}
