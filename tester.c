#include <stdio.h>
#include <stdlib.h>
#include "buddyallocator.h"
#define BUFFER_SIZE 530
#define BUDDY_LEVELS 6
#define MEMORY_SIZE 256
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
    printf("\t*INIT ALLOCATOR*\n");
    int init = BuddyAllocator_init(&alloc,myLevels,buffer,myBufferSize,memory,MEMORY_SIZE,myMinBucketSize);
    printf("DONE\n");
    if (init==0)return 0; //init fallita in questo caso
    printf("\t*PRIMA ALLOCAZIONE*\n");
    int* a1=BuddyMalloc(&alloc,24);
    int* a2=BuddyMalloc(&alloc,8);
    int* a3=BuddyMalloc(&alloc,8);
    int* a4=BuddyMalloc(&alloc,15);
    int* a5=BuddyMalloc(&alloc,8);
    int* a6=BuddyMalloc(&alloc,8);
    BuddyFree(&alloc,a3);
    BuddyFree(&alloc,a1);
    BuddyFree(&alloc,a2);
    BuddyFree(&alloc,a4);
    BuddyFree(&alloc,a6);
    BuddyFree(&alloc,a5);
    printf("\t*SECONDA ALLOCAZIONE*\n");
    a1=BuddyMalloc(&alloc,128);
    a2=BuddyMalloc(&alloc,256);
    a3=BuddyMalloc(&alloc,64);
    a4=BuddyMalloc(&alloc,64);
    a5=BuddyMalloc(&alloc,1);
    BuddyFree(&alloc,a4);
    BuddyFree(&alloc,a2);
    BuddyFree(&alloc,a5);
    BuddyFree(&alloc,a3);
    BuddyFree(&alloc,a1);
    printf("\t*TEST ERRORI*\n");
    a1=BuddyMalloc(&alloc,800);//errore perchè la size massima è 512
    a2=BuddyMalloc(&alloc,508);//sto allocando tutto lo spazio disponibile, rendendo inutilizzabile anche lo spazio rimanente
    a3=BuddyMalloc(&alloc,1);//errore perchè memoria terminata
    BuddyFree(&alloc,a2);
    if(myBufferSize>=530)BuddyFree(&alloc,a3);
    printf("\t*TEST TERMINATO*\n");



}
