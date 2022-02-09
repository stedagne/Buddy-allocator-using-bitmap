#pragma once
#include "bitmap.h"
#define MAX_LEVEL 16 //limite al numero di livelli dell'albero

//struttura bitmap
typedef struct BuddyAllocator{
    BitMap bitmap;
    int num_levels;
    char* buffer; 
    int buffer_size;
    int min_bucket_size; //la pagina di RAM minore che può essere ritornata
}BuddyAllocator;

//segnatura funzioni

//funzione malloc per allocare memoria
void* BuddyMalloc(BuddyAllocator* alloc, int size);
//free, rilascia la memoria allocata
void BuddyFree(BuddyAllocator* alloc, void* memory);
//merge, unisce bitmap e blocco libero in memoria
void BitmapMerge(BitMap* bitmap, int idx);
//imposta lo stato di tutti i nodi parents
void SetParents(BitMap *bitmap1, int status, int numBit);
//imposta lo stato di tutti i nodi figli
void SetParents(BitMap *bitmap1, int status, int numBit);
