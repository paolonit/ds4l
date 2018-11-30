#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
int main(void) { 
	pid_t pid;
	pid_t pid2;
	pid = fork();
	pid2 = fork();
	if(pid < 0 || pid2 < 0) {
		fprintf(stderr, "Fork Failed");
		exit(-1);
	}
	if(pid  == 0) {
		execlp("./event_gen", "event_gen", NULL);
	}
	if(pid2 == 0) {
		execlp("./device_read", "device_read", NULL);
	}
	exit(0);
	return(0);
}
