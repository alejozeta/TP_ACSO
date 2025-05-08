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
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp,
    int blockNum) {  
    // Check if the inode is valid and blockNum is non-negative
    if (inp == NULL || blockNum < 0) {
        return -1; // Invalid parameters
    }

    // Check if the blockNum is within the direct blocks
    if (blockNum < 10) {
        return inp->i_addr[blockNum]; // Return the direct block address
    }

    // Calculate the indirect block index
    blockNum -= 10;

    // Check if the blockNum is within the range of single indirect blocks
    if (blockNum < 128) {
        char buf[BLOCK_SIZE];
        if (diskimg_readsector(fs->dfd, inp->i_addr[10], buf) != BLOCK_SIZE) {
            return -1; // Error reading the indirect block
        }
        return ((int *)buf)[blockNum]; // Return the block address
    }

    // Calculate the double indirect block index
    blockNum -= 128;

    if (blockNum < 128 * 128) {
        char buf1[BLOCK_SIZE], buf2[BLOCK_SIZE];
        int indirectBlockIndex = blockNum / 128;
        int indirectBlockOffset = blockNum % 128;

        if (diskimg_readsector(fs->dfd, inp->i_addr[11], buf1) != BLOCK_SIZE) {
            return -1; // Error reading the double indirect block
        }

        int indirectBlockAddr = ((int *)buf1)[indirectBlockIndex];
        if (diskimg_readsector(fs->dfd, indirectBlockAddr, buf2) != BLOCK_SIZE) {
            return -1; // Error reading the indirect block
        }

        return ((int *)buf2)[indirectBlockOffset]; // Return the block address
    }

    return -1; // Block number out of range
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
