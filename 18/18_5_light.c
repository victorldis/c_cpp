#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if (argc == 1) {
        return 0;
    }

    pid_t pid;
    int i, j;

    int fd[4];
    int * fds_prev = fd;
    int * fds = &fd[2];
    
    for (i = 1; i < argc; ++i) {
        j = i - 1;
        if (i != argc-1) {
            pipe(fds);
        }
        pid = fork();
        if (pid) { // parent
            if ( i != 1 ) {
                close(fds_prev[0]);
            } 
            if ( i != argc-1 ) {
                close(fds[1]);
            }
        }
        else { 
            if (i != 1) {
                dup2(fds_prev[0], 0);
                close(fds_prev[0]);
            }
            if (i != argc-1) {
                close(fds[0]);
                dup2(fds[1], 1);
                close(fds[1]);
            }
            execlp(argv[i], argv[i], NULL);
            _exit(1);
        }
        int *tmp = fds;
        fds = fds_prev;
        fds_prev = tmp;
    }
    for (int k = i - 1; k >= 0; --k) {
        wait(NULL);
    }
    return 0;
}
