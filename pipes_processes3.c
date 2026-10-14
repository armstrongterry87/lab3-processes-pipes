// C program: cat scores | grep <arg> | sort using three processes and two pipes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <search term>\n", argv[0]);
        return 1;
    }

    int pipe1[2];   // cat  -> grep
    int pipe2[2];   // grep -> sort

    char *cat_args[]  = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("fork");
            exit(1);
        }
        if (pid2 == 0) {
            // P3 (child's child): sort
            dup2(pipe2[0], 0);   // stdin from pipe2
            close(pipe1[0]); close(pipe1[1]);
            close(pipe2[0]); close(pipe2[1]);
            execvp("sort", sort_args);
            perror("execvp sort");
            exit(1);
        } else {
            // P2 (child): grep
            dup2(pipe1[0], 0);   // stdin from pipe1
            dup2(pipe2[1], 1);   // stdout to pipe2
            close(pipe1[0]); close(pipe1[1]);
            close(pipe2[0]); close(pipe2[1]);
            execvp("grep", grep_args);
            perror("execvp grep");
            exit(1);
        }
    } else {
        // P1 (parent): cat scores
        dup2(pipe1[1], 1);       // stdout to pipe1
        close(pipe1[0]); close(pipe1[1]);
        close(pipe2[0]); close(pipe2[1]);
        execvp("cat", cat_args);
        perror("execvp cat");
        exit(1);
    }
    return 0;
}
