#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int error = 0;

void exelp_error(int sig) {
    error = 1;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        return 0;
    }

    pid_t *pids = (pid_t *) malloc( (argc-1) * sizeof(pid_t) );
    if (pids == NULL) {
        return 1;            
    }

    pid_t pid;
    int i, j, status;

    int fd[4];
    int * fds_prev = fd;
    int * fds = &fd[2]; 

    signal(SIGUSR1, exelp_error);

    for (i = 1; i < argc; i++) {
        j = i - 1;

        if (error) {
            break;
        }

        if (i != argc-1) {
            if (pipe(fds) == -1) {
                error = 1;
                break;
            }
        }
        pid = fork();
        if (pid == -1) {
            error = 1;
            break;
        }
        if (pid) { // parent
            pids[j] = pid;
            if ( i != 1 ) {
                close(fds_prev[0]);
            } 
            if ( i != argc-1 ) {
                close(fds[1]);
            }
        } else { // son
            free(pids);
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
            kill(SIGUSR1, getppid());
            _exit(1);
        }
        int *tmp = fds; // swap fds and fds_prev 
        fds = fds_prev;
        fds_prev = tmp;
    }

    if (error) {
        for (int k = i-1; k >= 0; --k) {
            kill(pids[k], SIGKILL);
        }
    }

    free(pids);
    for (int k = i - 1; k >= 0; --k) {
        wait(&status);
    }
    return error;
}
