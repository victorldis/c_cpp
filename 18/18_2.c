#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    long long sum = 0; // long long !
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
        close(fd[1]);
        pid_t pid1 = fork();
        if (pid1 == -1) {
            close(fd[0]);
            _exit(1);
        }
        if (!pid1) {
            while (read(fd[0], &num, sizeof(num))) {
                sum += num;
            }
	    printf("%lld\n", sum);
            close(fd[0]);
            exit(0);
        } 
        close(fd[0]);
        wait(NULL);
        exit(0);
    }
    close(fd[0]);
    while(scanf("%d", &num) >  0) {
        write(fd[1], &num, sizeof(num));
    }
    close(fd[1]);
    wait(NULL);
    return 0;
}

