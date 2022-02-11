#include <stdio.h>
#include <assert.h> //gestione errori
#include <math.h> // serve per floor e log2 nelle funzioni
#include <buddyallocator.h> // definisco qui le funzioni scritte nel file .c

// funzioni per la gestione dell'albero binario

//funzione che ritorna il livello del nodo considerato,cast obbligatorio a int perchè il ris è float;
int levelIdx(size_t idx){
    return (int)floor(log2(idx));
}
// funzione che ritorna l'indice del nodo padre
int parentIdx(int idx){
    return floor(idx/2);
}
// indice del fratello del nodo considerato
int buddyIdx(int idx){
    if (idx&0x1){
        return idx-1;
    }
    return idx+1;
}
// funzione che ritorna la distanza dal primo elemento sul livello dato
int offsetIdx(int idx){
    return (idx-(1<<levelIdx(idx)));
}
//funzione che ritorna l'indice del primo nodo sul livello i; equivale a fare 2^i;
int firstIdx(int idx){
    return 1<<idx;
}

//schema dell'albero bitmap
void bitmapPrint (BitMap *bitmap){
    int missingPrint = 0;
    int currentLvl = -1;
    int totLvl = levelIdx(bitmap->num_bits)-1;
    if (totLvl>5) totLvl = 5; //per visualizzare meglio l'albero
    for (int i=0; i<bitmap->num_bits;i++){
        if(missingPrint == 0){
            if(currentLvl==totLvl) break;
            printf("\nLivello %d (start bit %d):\t", ++currentLvl,i);
            for (int j= 0;j<(1<<totLvl)-(1<<currentLvl);j++) printf("  "); //spazi per formattare testo
            missingPrint =1<<currentLvl; // 2^currentLvl
        }
        printf("%d ", BitmapBit(bitmap,i));
        missingPrint--;
    }
    printf("\n");
}

//imposto stato genitore
void SetParents(BitMap *bitmap1, int status, int numBit){
    BitmapSet(bitmap1,status,numBit);
    if (numBit != 0) SetParents(bitmap1,status,parentIdx(numBit));
}
//imposto stato figlio
void SetChildren(BitMap *bitmap1, int status, int numBit){
    if(numBit<bitmap1->num_bits){
        BitmapSet(bitmap1,status,numBit);
        SetChildren(bitmap1, status, numBit*2+1);
        SetChildren(bitmap1, status, numBit*2+2);
    }
}
//allocazione memoria
void* BuddyMalloc(BuddyAllocator* alloc, int size){
    size+=sizeof(int); //uso 4 bytes per salvare l'indice della bitmap
    if (alloc->buffer_size<size){
        printf("\n***ERRORE*** : il blocco è troppo grande per essere allocato in memoria, la sua dimensione è %d\n ",size);
        return NULL;
    }
    //vedo il livello della pagina partendo dal basso
    int newBlockLvl = alloc->num_levels;
    int sizeStart = alloc->min_bucket_size;
    for (int i=0; i< alloc->num_levels;i++){
        if(sizeStart>=size) break;
        else{
            sizeStart*=2;
            newBlockLvl--; //level up
        }
    }
    printf("\nAllocando il blocco size %d nel lvl %d utilizzando un blocco di dim %d\n", size,newBlockLvl,sizeStart);
    //cerco un blocco libero partendo dal primo indice
    int freeIdx = -1;
    int i;
    for (i=firstIdx(newBlockLvl); i<firstIdx(newBlockLvl+1);i++){
        if(!BitmapBit(&alloc->bitmap,i)){ //se non è occupato
            freeIdx = i;
            break;
        }
    }
    //NULL se non trovo il blocco
    if (freeIdx <0){
        printf("\n***ERRORE***: memoria terminata, impossibile allocare il nuovo blocco\n");
        return NULL;
    }
    SetParents(&alloc->bitmap,1, freeIdx);
    SetChildren(&alloc->bitmap, 1,freeIdx);
    char *returning = alloc->buffer +startIdx(freeIdx)* sizeStart;
    ((int*)returning)[0] = freeIdx;
    printf("\n puntatore %p con indice %d\n", returning+sizeof(int),freeIdx);
    printf("\nbitmap dopo l'allocazione:");
    bitmapPrint(&alloc->bitmap);
    return (void*)(returning+sizeof(int));
}
