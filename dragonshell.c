#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/times.h>
#include <fcntl.h>
#include <unistd.h>
#include "builtin.h"
#include "clock.h"
#include "pipe.h"


/**
 * @brief Tokenize a C string 
 * 
 * @param str - The C string to tokenize 
 * @param delim - The C string containing delimiter character(s) 
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
void tokenize(char* str, const char* delim, char ** argv){
  char* token;
  token = strtok(str, delim);
  for(size_t i = 0; token != NULL; ++i){
    argv[i] = token;
  token = strtok(NULL, delim);
  }
}

pid_t background_pid = -1;

void sig_handler(int sig) {
  //pid_t pid;
  //while ((pid = waitpid(-1, NULL, WNOHANG))>0){
    if (background_pid > 0){
      kill(background_pid, sig);
      background_pid = -1;
    } 
  //}
    printf("\n");
}

void execExit(){
  if (background_pid != -1){
    kill(background_pid, SIGCONT);
    kill(background_pid, SIGTERM);
  }  
  while (waitpid(-1, NULL, WNOHANG) > 0);
  background_pid = -1;
	double user_time, system_time;
	end_clock(&user_time, &system_time);

	printf("User time: %.2f seconds\n", user_time);
	printf("System time: %.2f seconds\n", system_time);

	//_exit(0);
}


int main(int argc, char *argv[]) {
    char input[100];
    const char* delim = " ";
    char* tokenized[7];
    // for (int i = 0; i < 7; i++) {
    //   tokenized[i] = (char*)malloc(20 * sizeof(char));
    // }

    printf("Welcome to Dragon Shell!\n");
    start_clock();
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = 0; 
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = &sig_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);

    while (1) {
        printf("dragonshell > ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
          if (errno == EINTR){
            continue;
          }
            break; 
        }
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        for (int i=0; i < 7; i++){
          tokenized[i] = NULL;
        }
        int tokens;

        tokenize(input, delim, tokenized);
        if (tokenized[0] == NULL){
          continue;
        }

        int inputRedirect = -1, outputRedirect = -1, pipe = -1;
        for (int i = 0; tokenized[i] != NULL; ++i) {
            if (strcmp(tokenized[i], "<") == 0) {
                inputRedirect = i;  
            } if (strcmp(tokenized[i], ">") == 0) {
                outputRedirect = i; 
            } if (strcmp(tokenized[i], "|") == 0) {
                pipe = i; 
            }
            tokens = i;
        }
        int background = 0;
        if (strcmp(tokenized[tokens], "&")==0){
            background = 1;
            tokenized[tokens] = NULL;
        }

        if (strcmp(tokenized[0], "pwd") == 0) {
            execPwd();
        }
        else if (strcmp(tokenized[0], "cd") == 0){
          execCd(tokenized[1]);
        }
        else if (strcmp(tokenized[0], "exit") == 0){
          execExit();
          return 0;
        }else if(pipe != -1){
          execPipe(tokenized);
        }else{

          pid_t pid = fork();
          if (pid == 0) { //child
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            int input_file, output_file;
            if (inputRedirect != -1){
              input_file = open(tokenized[inputRedirect + 1], O_RDONLY);
              dup2(input_file, STDIN_FILENO);
              tokenized[inputRedirect] = NULL;
              close(input_file);
            }
            if (outputRedirect != -1){
              output_file = open(tokenized[outputRedirect + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644 );    
              dup2(output_file, STDOUT_FILENO);   
              tokenized[outputRedirect] = NULL; 
              close(output_file);
            }
            if(execv(tokenized[0], tokenized)==-1){
              printf("dragonshell: Command not found\n");
            }
            if (inputRedirect != -1) close(input_file);
            if (outputRedirect != -1) close(output_file);
          }
          if (background == 1){
              if (background_pid == -1){
                background_pid = pid;
                printf("PID %d is sent to background\n", background_pid);
              } else{
                kill(background_pid, SIGTERM);
              }
          }
          else { 
            int status;
            waitpid(pid, &status, WUNTRACED);
          }
        }
    }
    
    // for (int i = 0; i < 7; i++) {
    //   free(tokenized[i]);
    // }
    
    return 0;
}
