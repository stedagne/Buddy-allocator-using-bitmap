#pragma once
#include <stdint.h>

//array di bit
typedef struct{
    char* buffer;
    int buffer_size;
    int num_bits;
} BitMap;

//verifica lo stato del bit bit_num
int BitmapBit(const BitMap* bitmap, int numBit);

//imposta bit_num nel bitmap come 0 o 1
void BitmapSet(BitMap* bitmap, int status, int numBit);

//funzione che ritorna il numero di bytes per immagazzinare i bits di controllo
int BitmapGetBytes (int bits);

//inizializza una bitmap su un array 
void BitmapInit(BitMap* bitmap, char* buffer, int num_bits);
