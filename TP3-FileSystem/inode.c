#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"
#include "string.h"
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
    if (inumber < 1) {
        return -1;  // inodos empiezan desde 1
    }

    // Cálculo de bloque y posición dentro del bloque
    int inodes_per_sector = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int block_num = INODE_START_SECTOR + (inumber - 1) / inodes_per_sector;
    int index_in_block = (inumber - 1) % inodes_per_sector;

    // Leer el sector que contiene el inodo
    struct inode inodes[inodes_per_sector];
    if (diskimg_readsector(fs->dfd, block_num, inodes) != DISKIMG_SECTOR_SIZE) {
        return -1;
    }

    // Copiar el inodo deseado al buffer de salida
    *inp = inodes[index_in_block];
    return 0;
}



/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    if (inp == NULL || !(inp->i_mode & IALLOC) || blockNum < 0) {
        return -1;
    }

    int ptrs_per_block = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);

    if (!(inp->i_mode & ILARG)) {
        if (blockNum >= 8) return -1;
        int block = inp->i_addr[blockNum];
        return (block != 0) ? block : -1;
    }

    // Archivos grandes : acceso indirecto simple o doble
    int simple_limit = 7 * ptrs_per_block;

    if (blockNum < simple_limit) {
        // Indirección simple: punteros en i_addr[0..6]
        int indir_block_index  = blockNum / ptrs_per_block;
        int indir_block_offset = blockNum % ptrs_per_block;

        if (indir_block_index >= 7) return -1;

        int indir_block_num = inp->i_addr[indir_block_index];
        if (indir_block_num == 0) return -1;

        uint16_t ptrs[ptrs_per_block];
        if (diskimg_readsector(fs->dfd, indir_block_num, ptrs) != DISKIMG_SECTOR_SIZE) {
            return -1;
        }

        int data_block_num = ptrs[indir_block_offset];
        return (data_block_num != 0) ? data_block_num : -1;
    }

    // Indirección doble: i_addr[7]
    blockNum -= simple_limit;

    int double_indir_block = inp->i_addr[7];
    if (double_indir_block == 0) return -1;

    uint16_t first_level[ptrs_per_block];
    if (diskimg_readsector(fs->dfd, double_indir_block, first_level) != DISKIMG_SECTOR_SIZE) {
        return -1;
    }

    int first_index  = blockNum / ptrs_per_block;
    int second_index = blockNum % ptrs_per_block;

    if (first_index >= ptrs_per_block) return -1;

    int second_indir_block = first_level[first_index];
    if (second_indir_block == 0) return -1;

    uint16_t second_level[ptrs_per_block];
    if (diskimg_readsector(fs->dfd, second_indir_block, second_level) != DISKIMG_SECTOR_SIZE) {
        return -1;
    }

    int data_block_num = second_level[second_index];
    return (data_block_num != 0) ? data_block_num : -1;
}


int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
