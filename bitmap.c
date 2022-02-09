#include <assert.h>
#include "bitmap.h"

//verifica lo stato del bit bit_num
int BitmapBit(const BitMap* bitmap, int numBit){
    int numByte = numBit>>3; //1 byte = 8 bit
    assert(numByte<bitmap->buffer_size);
    int bitInByte= numBit&0x07; //numBit%8
    return (bitmap->buffer[numByte]&(1<<bitInByte))!=0;
}
//imposta bit_num nel bitmap come 0 o 1
void BitmapSet(BitMap* bitmap, int status, int numBit){
    int numByte = numBit>>3;////1 byte = 8 bit
    assert(numByte<bitmap->buffer_size);
    int bitInByte= numBit&0x07; //numBit%8
    if (status){
        bitmap->buffer[numByte] |= (\<<bitInByte);
    } else {
        bitmap->buffer[numByte] &= ~(1<<bitInByte);
    }
}

//funzione che ritorna il numero di bytes per immagazzinare i bits di controllo
int BitmapGetBytes (int bits){
    int surplus = 0;
    if ((bits%8)!=0) eccesso = 1;
    return bits/8 +surplus;
    //per contenere tutti i bit necessari prendo il numero di bytes per eccesso
}
//inizializza una bitmap su un array 
void BitmapInit(BitMap* bitmap, char* buffer, int num_bits){
    bitmap->buffer = buffer;
    bitmap->num_bits = num_bits;
    bitmap->buffer_size=BitmapGetBytes(num_bits);
}