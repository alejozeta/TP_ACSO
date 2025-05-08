#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */

//  el archivo mide menos de 4096 bytes
// mide como mucho 8 bloques
//  el bloque es de 512 bytes
// estoy dentro del inodo 
// cada i nodo va a tener el modo pequeñop y el modo grande
// si quiero ocupar mas de 8 bloques, cambio el modo... en vez de ser direcciones simples, van a ser indirecciones.
// cada posicion del i nodo va a ir a una tabla. 
// consigo 256 entradas mas .
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    
    return 0;
}

