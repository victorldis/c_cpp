#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        perror("invalid number of arguments");
        exit(1);
    }
    int num;
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == -1) {
        close(fd[0]);
        close(fd[1]);
        _exit(1);
    }
    if (!pid) {
        close( fd[0]);
        dup2( fd[1], 1);     
        close( fd[1]);
        execlp(argv[1], argv[1], NULL);
        _exit(1);        
    }
    close(fd[1]);
    pid = fork();
    if (pid == -1) {
        close(fd[0]);
        _exit(1);
    }
    if (!pid) {
        close( fd[1]);
        dup2( fd[0], 0);     
        close( fd[0]);
        execlp(argv[2], argv[2], NULL);
        _exit(1);        
    }

    close( fd[0]);
    wait(NULL);
    wait(NULL);
    printf("Done\n");
    
    return 0;
}

