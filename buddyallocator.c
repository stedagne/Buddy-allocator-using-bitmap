#include <stdio.h>
#include <assert.h> //gestione errori
#include <math.h> // serve per floor e log2 nelle funzioni
#include "buddyallocator.h" // definisco qui le funzioni scritte nel file .c

// funzioni per la gestione dell'albero binario

//funzione che ritorna il livello del nodo considerato,cast obbligatorio a int perchè il ris è float;
int levelIdx(size_t idx){
    return (int)floor(log2(idx+1));
}
// funzione che ritorna l'indice del nodo padre
int parentIdx(int idx){
    return (idx-1)/2;
}
// indice del fratello del nodo considerato
int buddyIdx(int idx){
    if (idx==0) return 0;
    else if (idx &0x1) return idx+1;
    return idx-1;
}
//funzione che ritorna l'indice del primo nodo sul livello i; equivale a fare 2^i;
int firstIdx(int lvl){
    return (1<< lvl) -1;
}
// funzione che ritorna la distanza dal primo elemento sul livello dato
int offsetIdx(int idx){
    return (idx-(firstIdx(levelIdx(idx))));
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
        if(sizeStart>=size) 
        break;
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
        printf("***ERRORE***: memoria terminata, impossibile allocare il nuovo blocco\n");
        return NULL;
    }
    SetParents(&alloc->bitmap,1, freeIdx);
    SetChildren(&alloc->bitmap, 1,freeIdx);
    char *returning = alloc->buffer + offsetIdx(freeIdx)* sizeStart;
    ((int*)returning)[0] = freeIdx;
    printf("puntatore %p con indice %d\n", returning+sizeof(int),freeIdx);
    printf("bitmap dopo l'allocazione:");
    bitmapPrint(&alloc->bitmap);
    return (void *)(returning+sizeof(int));
}
//free, rilascia la memoria allocata
void BuddyFree(BuddyAllocator *alloc, void *memory){
    printf("\nLibero blocco %p\n", memory);
    if(!memory){ //il free di NULL ovviamente è inutile
        printf("\n***ERRORE***:impossibile fare la free di NULL");
        return;
    }
    int *pointer = (int *)memory;
    int idxToFree = pointer[-1];//sto prendendo l'indice dal puntatore
    printf("Indice da liberare : %d\n", idxToFree);
    int sizeLvl = alloc->min_bucket_size*(1<<(alloc->num_levels - levelIdx(idxToFree)));
    char *pointerToFree = alloc->buffer+offsetIdx(idxToFree) *sizeLvl;
    assert("Puntatore non allineato" && (int *)pointerToFree == &pointer[-1]);
    assert("Double free" && BitmapBit(&alloc->bitmap, idxToFree));
    SetChildren(&alloc->bitmap, 0 ,idxToFree);
    BitmapMerge(&alloc->bitmap,idxToFree);
    printf("Free terminata, ecco la bitmap corrente:");
    bitmapPrint(&alloc->bitmap);
}
//merge, unisce bitmap e blocco libero in memoria
void BitmapMerge(BitMap* bitmap, int idx){
    assert("***ERRORE***: impossibile fare merge su un bit libero" && !BitmapBit(bitmap,idx));
    if(idx==0) return;
    int buddy_idx = buddyIdx(idx);
    if(!BitmapBit(bitmap,buddy_idx)){
        printf("Merging del blocco %d e del suo buddy %d sul livello %d\n", idx,buddy_idx,levelIdx(idx));
        int parent_idx = parentIdx(idx);
        BitmapSet(bitmap,0,parent_idx);
        BitmapMerge(bitmap,parent_idx);

    }
}
//inizializza il buddy allocator e controlla se c'è abbastanza spazio nel buffer
int BuddyAllocator_init(
    BuddyAllocator* alloc,
    int num_levels,
    char* alloc_buffer,
    int alloc_buffer_size,
    char* bitmap_buffer,
    int bitmap_buffer_size,
    int min_bucket_size){

        if (min_bucket_size<8){
            printf("\n***ERRORE***:Min_bucket_size troppo piccolo");
            return 0;
        }
        assert("***ERRORE***:Num livelli troppo grande" && num_levels<MAX_LEVEL);
        //genero numero di bit per la bitmap ed ogni bit è buddy di min_bucket_size
        int num_bits=(1<<(num_levels+1)) -1; //num allocazioni massime
        assert("***ERRORE***: Memoria per la bitmap non sufficiente" && bitmap_buffer_size>=BitmapGetBytes(num_bits));
        //se si usa una potenza di 2 allora posso utilizzare tutta la memoria a disposizione
        if(levelIdx(alloc_buffer_size)!= log2(alloc_buffer_size)){
            printf("**WARNING**:posso utilizzare solo %d bytes di %d forniti.\n",min_bucket_size<<num_levels, alloc_buffer_size);
            printf("\n*INPUT*:premi 1 per continuare e qualsiasi altro tasto per terminare");
            int input;
            scanf("%d", &input);
            if(input!=1)return 0;
            alloc_buffer_size = min_bucket_size<<num_levels; //size max gestibile
            }
            alloc->num_levels=num_levels;
            alloc->buffer =alloc_buffer;
            alloc->buffer_size =alloc_buffer_size;
            alloc->min_bucket_size=min_bucket_size;
            printf("INIT\n");
            printf("\tmemoria gestita: %d bytes\n", alloc_buffer_size); // (1<<num_levels)*min_bucket_size  (2^5)*8 = 256 bytes
            printf("\tlivelli: %d\n", num_levels);
            printf("\tmin_bucket size: %d\n", min_bucket_size);
            printf("\tbits_bitmap: %d\n", num_bits);
            printf("\tmemoria bitmap: %d bytes usati di %d bytes forniti \n", BitmapGetBytes(num_bits), bitmap_buffer_size);
            BitmapInit(&alloc->bitmap,bitmap_buffer,num_bits);
            printf("bitmap allocata: ");
            bitmapPrint(&alloc->bitmap);
            return 1;
};
