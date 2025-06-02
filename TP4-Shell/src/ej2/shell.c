#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMANDS 220
#define MAX_ARGS 64

// Parsea un string en args[], sin malloc, respetando comillas
int parse_args(char *input, char **args) {
    int i = 0;
    while (*input) {
        while (*input == ' ' || *input == '\t') input++;
        if (*input == '\0') break;

        char *start;
        if (*input == '\'' || *input == '"') {
            char quote = *input++;
            start = input;
            while (*input && *input != quote) input++;
            if (*input != quote) {
                fprintf(stderr, "Error: comilla sin cerrar\n");
                return -1;
            }
            *input++ = '\0';
        } else {
            start = input;
            while (*input && *input != ' ' && *input != '\t') input++;
            if (*input) *input++ = '\0';
        }
        args[i++] = start;
        if (i >= MAX_ARGS + 1) {
            fprintf(stderr, "Error: demasiados argumentos\n");
            return -1;
        }
    }
    args[i] = NULL;
    return 0;
}

// Verifica errores de sintaxis (pipes mal ubicados o múltiples consecutivos)
int check_syntax(const char *input) {
    int len = strlen(input);
    int only_pipes = 1;
    int last_was_pipe = 0;
    int count = 0;

    for (int i = 0; i < len; i++) {
        if (input[i] == '|') {
            if (last_was_pipe) return -1; // Pipes dobles o más
            last_was_pipe = 1;
            count++;
        } else if (input[i] != ' ' && input[i] != '\t') {
            only_pipes = 0;
            last_was_pipe = 0;
        }
    }

    if (only_pipes || input[0] == '|' || input[len - 1] == '|') return -1;
    return 0;
}

int main() {
    char command[1024];

    while (1) {
        if (isatty(STDIN_FILENO)) {
            printf("Shell> ");
            fflush(stdout);
        }

        if (!fgets(command, sizeof(command), stdin)) break;
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) break;
        if (check_syntax(command) != 0) {
            fprintf(stderr, "Error: comando vacío\n");
            continue;
        }

        char *commands[MAX_COMMANDS];
        int count = 0;
        char *token = strtok(command, "|");
        while (token != NULL) {
            while (*token == ' ') token++;
            if (*token == '\0') {
                fprintf(stderr, "Error: comando vacío\n");
                count = -1;
                break;
            }
            commands[count++] = token;
            token = strtok(NULL, "|");
        }
        if (count <= 0) continue;

        int prev_fd = -1;
        int pipe_fd[2];

        for (int i = 0; i < count; i++) {
            if (i < count - 1 && pipe(pipe_fd) == -1) {
                perror("pipe");
                exit(1);
            }

            pid_t pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Error: fork falló al crear proceso %d\n", i);
                continue;
            } else if (pid == 0) {
                if (prev_fd != -1) {
                    dup2(prev_fd, STDIN_FILENO);
                    close(prev_fd);
                }
                if (i < count - 1) {
                    close(pipe_fd[0]);
                    dup2(pipe_fd[1], STDOUT_FILENO);
                    close(pipe_fd[1]);
                }

                char *args[MAX_ARGS];
                if (parse_args(commands[i], args) != 0)
                    exit(1);
                if (args[0] == NULL) {
                    fprintf(stderr, "Error: comando vacío\n");
                    exit(1);
                }

                execvp(args[0], args);
                perror("execvp");
                exit(1);
            }

            if (prev_fd != -1) close(prev_fd);
            if (i < count - 1) {
                close(pipe_fd[1]);
                prev_fd = pipe_fd[0];
            }
        }

        for (int i = 0; i < count; i++){
            wait(NULL);
        }
            
    }

    return 0;
}
