#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMANDS 200
#define MAX_ARGS 100

// Parsea un string en args[], sin malloc, respetando comillas
void parse_args(char *input, char **args) {
    int i = 0;
    while (*input) {
        while (*input == ' ' || *input == '\t') input++;
        if (*input == '\0') break;

        char *start;
        if (*input == '\'' || *input == '"') {
            char quote = *input++;
            start = input;
            while (*input && *input != quote) input++;
        } else {
            start = input;
            while (*input && *input != ' ' && *input != '\t') input++;
        }

        if (*input) *input++ = '\0'; // termina string y avanza
        args[i++] = start;
    }
    args[i] = NULL;
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
            if (pid == -1) {
                perror("fork");
                exit(1);
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
                parse_args(commands[i], args);

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

        for (int i = 0; i < count; i++)
            wait(NULL);
    }

    return 0;
}
