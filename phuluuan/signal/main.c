#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]){
	int i = 0;
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGINT);
	sigprocmask(SIG_BLOCK, &sigset, NULL);
	while(1){
		printf("%d\n", i);
		i++;
		sleep(1);
	}
	return 0;
}