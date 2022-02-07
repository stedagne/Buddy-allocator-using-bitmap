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