#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 5) {
        printf("invalid number of arguments\n");
        exit(1);
    }
    int fd[2];
    pipe(fd);
    pid_t pid = fork();

    if (pid == -1) {
        _exit(1);
    }
    if (!pid) {
        close(fd[1]);
        dup2(fd[0], 0);
        close(fd[0]);

        int f = open(argv[4], O_WRONLY | O_TRUNC | O_CREAT, 0666); ///
        dup2(f, 1);
        close(f);
        execlp(argv[3], argv[3], NULL);
        exit(1);
    }

    close(fd[0]);
    for (int i = 1; i <= 2; ++i) {
        pid = fork();
        if (pid == -1) {
            _exit(1);
        }
        if (!pid) {
            dup2(fd[1], 1);
            close(fd[1]);
            execlp(argv[i], argv[i], NULL);
            exit(1);
        }
        wait(NULL);
    }
    close(fd[1]);
    wait(NULL);
}

