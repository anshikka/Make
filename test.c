#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int makeargv(const char *s, const char *delimiters, char ***argvp) {
    int error;
    int i;
    int numtokens;
    const char *snew;
    char *t;

    if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
        errno = EINVAL;
        return -1;
    }
    *argvp = NULL;
    snew = s + strspn(s, delimiters);
    if ((t = malloc(strlen(snew) + 1)) == NULL)
        return -1;
    strcpy(t, snew);
    numtokens = 0;
    if (strtok(t, delimiters) != NULL)
        for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++);

    if ((*argvp = malloc((numtokens + 1) * sizeof(char *))) == NULL) {
        error = errno;
        free(t);
        errno = error;
        return -1;
    }

    if (numtokens == 0)
        free(t);
    else {
        strcpy(t, snew);
        **argvp = strtok(t, delimiters);
        for (i = 1; i < numtokens; i++)
            *((*argvp) + i) = strtok(NULL, delimiters);
    }
    *((*argvp) + numtokens) = NULL;
    return numtokens;
}

int main() {
//    char * line = "---abcabc-efefef-ggg---";
//    char * dels = "-";
//    char * line = "t1: d1 d2 d3";
//    char * dels = ": ";
    char * line = "\tr1, r2";
    char * dels = "\t ,";

    char *** tokens = malloc(sizeof(char***));
    int numtokens = makeargv(line, dels, tokens);
    printf("Num tokens : %d\n", numtokens );
    printf("First token : %s\n", tokens[0][0]);
    printf("Second token : %s\n", tokens[0][1]);

    return 0;
}