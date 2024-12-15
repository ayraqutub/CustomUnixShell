#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include "clock.h"
#include "builtin.h"


void execPwd() {
	char s[100];
    const char *pwd = getcwd(s,100);
    printf("%s\n", pwd);
}

void execCd(const char *path){
	if(path == NULL){
	printf("Expected argument to \"cd\"\n");
	}
	else if (chdir(path)==-1){
	printf("No such file or directory\n");
	}
}
