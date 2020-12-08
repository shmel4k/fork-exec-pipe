#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/wait.h>

#define MAXWORD 20
#define MAXWORDLEN 80

char cldProgram[] = "./match_word";
char cldProgArg[] = "dictionary.txt";

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Wrong number of arguments..\n");
        return 1;
    }

    int fd;
    if (-1 == (fd = open(argv[1], O_RDONLY))) {
        perror("open");
        return 1;
    }
    char inBuf[MAXWORD * MAXWORDLEN + 10];
    bzero(inBuf, sizeof(inBuf));
    ssize_t nByte;
    if (-1 == (nByte = read(fd, inBuf, sizeof(inBuf)))) {
        perror("read");
        return 1;
    }
    if (-1 == close(fd)) {
        perror("close");
        return 1;
    }

    char words[MAXWORD][MAXWORDLEN];
    bzero(*words, MAXWORD * MAXWORDLEN);

    int j = 0;
    for (int i = 0, k = 0; i < nByte; ++i) {
        if (inBuf[i] != '\n') {
            words[j][k] = inBuf[i];
            ++k;
        }
        else {
            ++j;
            k = 0;
        }
    }

    const int num = j;

    int pFdW[num][2], pFdR[num][2];
    bzero(pFdW[0], sizeof(pFdW[0]) * num);
    bzero(pFdR[0], sizeof(pFdR[0]) * num);

    for(int j = 0; j < num; j++) {
        if ( (-1 == pipe(pFdW[j])) ||  (-1 == pipe(pFdR[j])) ) {
            perror("pipe");
            return 1;
        }
    }

    pid_t pid[num];
    char buf[MAXWORDLEN];
    char answers[num][MAXWORDLEN];
    bzero(*answers, num * MAXWORDLEN);
    for(int j = 0; j < num; j++) {
        if ( -1 == (pid[j] = fork()) ) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (0 != pid[j]) { // parent
            bzero(buf, MAXWORDLEN);
            sprintf(buf, "%d", j);
            write(pFdW[j][1], buf, sizeof(buf));
            read(pFdR[j][0], answers[j], MAXWORDLEN);
        }
        else { // child
            bzero(buf, MAXWORDLEN);
            read(pFdW[j][0], buf, sizeof(buf));
            int k = atoi(buf);

            char fdStr[10];
            bzero(fdStr, strlen(fdStr));
            sprintf(fdStr, "%d", pFdR[j][1]);

            char *newargv[MAXWORDLEN] = {cldProgram, cldProgArg, words[k], fdStr, NULL};
            execv(cldProgram, newargv);

            /* execve() returns only on error */
            close(STDERR_FILENO);
            dup2(pFdR[j][1], STDERR_FILENO);
            close(pFdR[j][1]);

            perror("execv");
            exit(EXIT_FAILURE);
        }
    }

    for(int j = 0; j < num; j++) {
        waitpid(pid[j], NULL, 0);
        printf("%s ", answers[j]);
    }
    printf("\n");

    return 0;
}