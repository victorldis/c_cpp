#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
         return 1;
    }
    int num, n = atoi(argv[1]);

    int fd[4];
    FILE * fdi, * fdo;

    pipe(fd);
    pipe(&fd[2]);

    int i;
    for (i=0; i<2; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            _exit(1);
        }
        if (!pid) {
            fprintf(stderr, "son 1\n");
            close(fd[i*2+1]);
            close(fd[(1-i)*2]);

            fdi = fdopen(fd[i*2],"r");
            fdo = fdopen(fd[(1-i)*2+1],"w");

            while (fscanf(fdi, "%d", &num)>0) {
                if (num==n) {
                    break;
                }
                printf( "%d %d\n", i + 1, num);
                fprintf( fdo, "%d\n", num+1);
                fflush(fdo);                
            }

            fclose(fdi);
            fclose(fdo);
            exit(0);
        }
    }
    
    fdo = fdopen(fd[1],"w");
    fprintf( fdo, "%d\n", 1);
    fclose(fdo);
  
    close(fd[0]);
    close(fd[2]);
    close(fd[3]);

    wait(NULL);
    wait(NULL);
    printf( "Done\n");
  
    return 0;    
}

