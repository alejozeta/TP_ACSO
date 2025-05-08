
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
#include <string.h>
#include <stdio.h>
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "unixfilesystem.h"

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (!fs || !pathname || pathname[0] != '/') {
        return -1;  // path inválido
    }

    int current_inumber = 1; // siempre empezamos desde raíz (inode 1)

    // Nos movemos sobre el path
    const char *p = pathname;

    // Saltamos el primer '/'
    while (*p == '/') p++;

    char name[15];  // 14 caracteres + '\0'
    while (*p) {
        // Copiar un componente (hasta próximo '/' o fin)
        int i = 0;
        while (*p != '/' && *p != '\0' && i < 14) {
            name[i++] = *p++;
        }
        name[i] = '\0';  // cerramos el string

        if (i == 0) {
            // dos '/' seguidos o terminaba en '/'
            return -1;
        }

        // Buscar el nombre en el directorio actual
        struct direntv6 dirent;
        if (directory_findname(fs, name, current_inumber, &dirent) < 0) {
            return -1;  // no encontrado
        }

        current_inumber = dirent.d_inumber;  // saltamos al siguiente inode

        // Saltamos los siguientes '/'
        while (*p == '/') p++;
    }

    return current_inumber;
}

