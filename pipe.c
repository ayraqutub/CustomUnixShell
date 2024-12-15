#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "pipe.h"

void execPipe(char** tokenized){
    char* cmd1[5];
    char* cmd2[5];
    split_on_pipe(tokenized, cmd1, cmd2);

    int fd[2];
    if (pipe(fd) == -1){
        printf("Check");
    }
    int pid1= fork();
    if (pid1 == 0){
        dup2(fd[1], STDOUT_FILENO); //write side of pipe
        close(fd[0]);
        close(fd[1]); //dup2 leaves the first file descriptor so now getting rid of it

        if(execv(cmd1[0], cmd1)==-1){
          printf("Command not found\n");
        }
    }

    int pid2 = fork();
    if (pid2 == 0){
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        if(execv(cmd2[0], cmd2)==-1){
          printf("Command not found\n");
        }
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void split_on_pipe(char** tokenized, char** cmd1, char** cmd2) {
    int cmd1_index = 0, cmd2_index = 0;
    int is_cmd2 = 0; 

    for (int i = 0; tokenized[i] != NULL; i++) {
        if (strcmp(tokenized[i], "|") == 0) {
            tokenized[i] = NULL;
            i++;
            is_cmd2 = 1;
        }
        if (is_cmd2) {
            cmd2[cmd2_index++] = tokenized[i]; //CHECK
        } else {
            cmd1[cmd1_index++] = tokenized[i];
        }
    }
    cmd1[cmd1_index] = NULL;
    cmd2[cmd2_index] = NULL;
}
