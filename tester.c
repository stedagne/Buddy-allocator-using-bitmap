#include <stdio.h>
#include <stdlib.h>
#include "buddyallocator.h"
#define BUFFER_SIZE 256
#define BUDDY_LEVELS 5
#define MEMORY_SIZE 128
#define MIN_BUCKET_SIZE (BUFFER_SIZE >>(BUDDY_LEVELS))

char memory[MEMORY_SIZE];
BuddyAllocator alloc;
int main(int argc, char** argv){
    int myBufferSize = BUFFER_SIZE;
    int myMinBucketSize = MIN_BUCKET_SIZE;
    int myLevels = BUDDY_LEVELS;
    if (argc>1){
        myBufferSize = atoi(argv[1]);
        printf("Buffer di dimensione %d\n",myBufferSize);
        if (argc>2){
            myLevels=atoi(argv[2]);
            printf("Numero di livelli: %d\n", myLevels);
        }
    }
    char buffer[myBufferSize];
    if(myBufferSize!=BUFFER_SIZE){
        myMinBucketSize=(myBufferSize>>(myLevels));
    }
    if (myLevels>5) printf("Stampo a schermo fino al livello 5\n");
    //
    printf("*INIT ALLOCATOR*\n");
    int init = BuddyAllocator_init(&alloc,myLevels,buffer,myBufferSize,memory,MEMORY_SIZE,myMinBucketSize);
    printf("DONE\n");
    if (init==0)return 0; //init fallita in questo caso
    printf("*PRIMA ALLOCAZIONE*\n");
    int* a1=BuddyMalloc(&alloc,4);
    int* a2=BuddyMalloc(&alloc,16);
    int* a3=BuddyMalloc(&alloc,8);
    int* a4=BuddyMalloc(&alloc,4);
    int* a5=BuddyMalloc(&alloc,16);
    BuddyFree(&alloc,a3);
    BuddyFree(&alloc,a1);
    BuddyFree(&alloc,a5);
    BuddyFree(&alloc,a4);
    BuddyFree(&alloc,a2);
    printf("*SECONDA ALLOCAZIONE*\n");
    a1=BuddyMalloc(&alloc,32);
    a2=BuddyMalloc(&alloc,64);
    a3=BuddyMalloc(&alloc,2);
    a4=BuddyMalloc(&alloc,2);
    a5=BuddyMalloc(&alloc,32);
    BuddyFree(&alloc,a4);
    BuddyFree(&alloc,a2);
    BuddyFree(&alloc,a5);
    BuddyFree(&alloc,a3);
    BuddyFree(&alloc,a1);
    printf("*TEST ERRORI*\n");
    a1=BuddyMalloc(&alloc,512);//errore perchè la size massima è 256
    a2=BuddyMalloc(&alloc,248);//sto allocando tutto lo spazio disponibile, rendendo inutilizzabile anche lo spazio rimanente
    a3=BuddyMalloc(&alloc,64);//errore perchè memoria terminata
    BuddyFree(&alloc,a2);
    if(myBufferSize>=512)BuddyFree(&alloc,a3);
    printf("*TEST TERMINATO*\n");



}