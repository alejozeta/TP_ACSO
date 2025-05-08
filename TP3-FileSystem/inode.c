#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

#define INODE_SIZE 32
#define BLOCK_SIZE 512
#define INODE_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
#define INODE_START_SECTOR 2


/**
 * TODO
 */

// sector size is 512 bytes
// block size is 512 bytes
// inode size is 32
// blocksize/ inode size = 16
// number of inodes = 512/32 = 16
// sector number = inode number / 16
// inode position = inode number % 16
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    //Implement Code Here
    // levanto inode inumber y lo guardo en inp
    // 1. Check if the inumber is valid
    if (inumber < 0) {
        return -1; // Invalid inode number
    }
    // 2. Calculate the sector number and offset

    int sectorNum = INODE_START_SECTOR + (inumber / INODE_PER_BLOCK);
    int offset = (inumber % INODE_PER_BLOCK) * INODE_SIZE;

    // levanto inodo
    char buf[BLOCK_SIZE];
    if (diskimg_readsector(fs->dfd, sectorNum, buf) != BLOCK_SIZE) {
        return -1; // Error reading the sector
    }
    // 3. Copy the inode data from the buffer to inp
    memcpy(inp, buf + offset, INODE_SIZE);
    return 0; 
}

/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if (inp == NULL || blockNum < 0) {
        return -1;
    }

    int filesize = inode_getsize(inp);
    int maxBlocks = (filesize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
    if (blockNum >= maxBlocks) {
        return -1;
    }

    // Caso 1: archivo chico (sin ILARG): usar i_addr[blockNum]
    if ((inp->i_mode & ILARG) == 0) {
        if (blockNum >= 8) return -1;
        return inp->i_addr[blockNum];
    }

    // Caso 2: archivo grande (ILARG activado)
    if (blockNum < 7 * 256) {
        int which = blockNum / 256;
        int offset = blockNum % 256;

        uint16_t indir = inp->i_addr[which];
        if (!indir) return -1;

        uint16_t table[256];
        if (diskimg_readsector(fs->dfd, indir, table) != DISKIMG_SECTOR_SIZE)
            return -1;

        return table[offset];
    }

    // Caso 3: doble indirecto
    blockNum -= 7 * 256;

    uint16_t dbl = inp->i_addr[7];
    if (!dbl) return -1;

    uint16_t lvl1[256];
    if (diskimg_readsector(fs->dfd, dbl, lvl1) != DISKIMG_SECTOR_SIZE)
        return -1;

    int i1 = blockNum / 256;
    int i2 = blockNum % 256;

    if (!lvl1[i1]) return -1;

    uint16_t lvl2[256];
    if (diskimg_readsector(fs->dfd, lvl1[i1], lvl2) != DISKIMG_SECTOR_SIZE)
        return -1;

    return lvl2[i2];
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
