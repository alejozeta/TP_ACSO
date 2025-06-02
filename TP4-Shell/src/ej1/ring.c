#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	//Valid cantidad de argumentos
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <n> <c> <s>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);     // cantidad de procesos
    int value = atoi(argv[2]); // valor inicial
    int start = atoi(argv[3]); // proceso que inicia

	//Creo los pipes
    int pipes[n][2];
    for (int i = 0; i < n; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }
    }

    int pipe_back[2];
    if (pipe(pipe_back) == -1) {
        perror("pipe");
        exit(1);
    }

	// creo los procesos hijos
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            int read_fd = pipes[i][0];
            int write_fd = ((i + 1) % n == start) ? pipe_back[1] : pipes[(i + 1) % n][1];

            for (int j = 0; j < n; j++) {
                if (j != i) close(pipes[j][0]);
                if (j != (i + 1) % n) close(pipes[j][1]);
            }
            close(pipe_back[0]);

            int buffer;
            if (read(read_fd, &buffer, sizeof(int)) > 0) {
                buffer += 1;
                write(write_fd, &buffer, sizeof(int));
            }

            close(read_fd);
            close(write_fd);
            exit(0);
        }
    }
    for (int i = 0; i < n; i++) {
        close(pipes[i][0]);
        if (i != start)
            close(pipes[i][1]);
    }
    close(pipe_back[1]);

    write(pipes[start][1], &value, sizeof(int));
    close(pipes[start][1]);

    int result;
    read(pipe_back[0], &result, sizeof(int));
    close(pipe_back[0]);

    printf("Resultado final: %d\n", result);

    for (int i = 0; i < n; i++)
        wait(NULL);

    return 0;
}
