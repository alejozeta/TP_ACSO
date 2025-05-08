#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *name,
  int dirinumber, struct direntv6 *dirEnt) {
    if (!fs || !name || !dirEnt) {
        return -1;
        }

    struct inode dir_inode;
    if (inode_iget(fs, dirinumber, &dir_inode) < 0) {
        return -1;
        }

    int filesize = inode_getsize(&dir_inode);
    int num_dirents = filesize / sizeof(struct direntv6);

    for (int i = 0; i < num_dirents; i++) {
        struct direntv6 d;
        int block = i * sizeof(struct direntv6) / DISKIMG_SECTOR_SIZE;
        int offset = (i * sizeof(struct direntv6)) % DISKIMG_SECTOR_SIZE;

        char buf[DISKIMG_SECTOR_SIZE];
        if (file_getblock(fs, dirinumber, block, buf) < 0) {
            return -1;
            }

        memcpy(&d, buf + offset, sizeof(struct direntv6));

        if (strncmp(d.d_name, name, 14) == 0) {
            *dirEnt = d;
            return 0;}
    }

    return -1;  // nombre no encontrado
}


