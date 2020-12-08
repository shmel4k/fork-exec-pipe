#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#define MAXSYMBOL 128

char * sequence(char *dst, char *src) {
    int buf[MAXSYMBOL];
    memset(buf, 0, sizeof(buf));

    char *p = src;
    while(p != NULL && *p) {
        ++buf[*p++];
    }

    p = dst;
    for(int i = 0; i < MAXSYMBOL; ++i) {
        for(int j = 0; j < buf[i]; ++j) {
            *p++ = i;
        }
    }

    return dst;
}

int main(int argc, char* argv[])
{
    FILE *fp;
    char words[50][20];
    char check_word[20];
    int num_words = 0, i = 0;

    if(argc != 4)
    {
        printf("Wrong number of arguments..\n");
        return 1;
    }

    bzero(check_word, sizeof(check_word));
    strcpy(check_word, argv[2]);
    char chk_word_seq[MAXSYMBOL];
    bzero(chk_word_seq, MAXSYMBOL);
    sequence(chk_word_seq, check_word);

    fp = fopen(argv[1], "r");

    while(!feof(fp))
    {
        fscanf(fp, "%s", words[i]);
        i++;
        num_words++;
    }

    char buf[40];
    bzero(buf, sizeof(buf));
    for(i=0; i<num_words; i++)
    {
        if(strlen(words[i]) == strlen(check_word))
        {
            char word_seq[MAXSYMBOL];
            bzero(word_seq, MAXSYMBOL);
            sequence(word_seq, words[i]);

            if (!strncmp(word_seq, chk_word_seq, MAXSYMBOL)) {
                sprintf(buf, "%s", words[i]);
                break;
            }
        }
    }

    if (!strlen(buf)) {
        sprintf(buf, "no_match");
    }
    int fd = atoi(argv[3]);
    write(fd, buf, sizeof(buf));

    return 0;
}
