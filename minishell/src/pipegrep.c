#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(void) {

    int fd[2];
    pid_t pid;
    int status;

    char *ls[] = {"ls", NULL};
    char *grep[] = {"grep", ".c", NULL};

    if (pipe(fd) < 0) {
        printf("Create pipe error\n");
    }

    pid = fork();
    if (pid == 0) {
        dup2(fd[1], 1);
        close(fd[0]);

        if (execvp("ls", ls) == -1) {
            perror("error");
        }
        close(fd[1]);
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("error");
    } else {
        dup2(fd[0], 0);
        close(fd[1]);

        if (execvp("grep", grep) == -1) {
            perror("error");
        }

        do {waitpid(pid, &status, WUNTRACED);
        } while (! WIFEXITED(status) && ! WIFSIGNALED(status));
    }

    return 1;
}








