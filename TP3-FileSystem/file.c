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
    struct inode in;

    if (inode_iget(fs, inumber, &in) < 0) {
        return -1;
    }

    int diskBlock = inode_indexlookup(fs, &in, blockNum);
    if (diskBlock < 0) {
        return -1;
    }

    if (diskimg_readsector(fs->dfd, diskBlock, buf) != DISKIMG_SECTOR_SIZE) {
        return -1;
    }

    int filesize = inode_getsize(&in);
    int block_offset = blockNum * DISKIMG_SECTOR_SIZE;

    if (block_offset >= filesize) {
        return 0;
    }

    int valid_bytes = filesize - block_offset;
    if (valid_bytes > DISKIMG_SECTOR_SIZE) {
        valid_bytes = DISKIMG_SECTOR_SIZE;
    }

    return valid_bytes;
}

