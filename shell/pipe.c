#include <stdlib.h>
#include <unistd.h>

int pid;
int pipe1[2];
int pipe2[2];

void main() {
	if (pipe(pipe1) == -1) {
		perror("bad pipe1");
		exit(1);
	}

	if ((pid = fork()) == -1) {
		perror("bad fork 1");
		exit(1);
	} else if (pid == 0) {
		exec1();
	}

	if (pipe(pipe2) == -1) {
		perror("bad pipe 2");
		exit(1);
	}

	if ((pid = fork()) == -1) {
		perror("bad fork 2");
		exit(1);
	} else if (pid == 0) {
		exec2();
	}

	close(pipe1[0]);
	close(pipe1[1]);

	if ((pid = fork()) == -1) {
		perror("bad fork 3");
		exit(1);
	} else if (pid == 0) {
		exec3();
	}
}

void exec1() {
	dup2(pipe1[1], STDOUT_FILENO);
	close(pipe1[0]);
	close(pipe1[1]);
	execlp("ps", "ps", "aux", NULL);
	perror("bad exec ps");
	exit(1);
}

void exec2() {
	dup2(pipe1[0], STDIN_FILENO);
	dup2(pipe2[1], STDOUT_FILENO);
	
	close(pipe1[1]);
	close(pipe1[0]);
	close(pipe2[0]);
	close(pipe2[1]);

	execlp("grep", "grep", "root", NULL);
	
	perror("bad exec grep root");
	_exit(1);
}

void exec3() {
	dup2(pipe2[0], STDIN_FILENO);
	
	close(pipe2[0]);
	close(pipe2[1]);

	execlp("grep", "grep", "sbin", NULL);
	perror("bad exec grep sbin");
	_exit(1);
}
