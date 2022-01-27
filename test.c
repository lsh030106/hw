#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TOK_BUFSIZE 64
#define TOK_DELUM " \t\r\n\a"
#define TOK_PIPE "|"

char ***split_line(char *, int*);

int main(void) {
    char line[] = "ls | grep a | grep b";
    int seq = 0;
    char ***arg;

    arg = split_line(line, &seq);
    
    printf("%d", seq);
   
    for (int i = 0; i < seq; i++) {
        printf("%s\n", arg[i][0]);
    }


    return 0;
}

char ***split_line(char *line, int *seq) {

    int bufsize = TOK_BUFSIZE, position = 0;
    char ***token_list = (char ***)malloc(sizeof(char **) * bufsize);

    for (int i = 0; i < bufsize; i++) {
        token_list[i] = (char **)malloc(sizeof(char *) * bufsize);
    }
    //char *tokens[bufsize];
    char *token;

    if (! token_list) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELUM);
    while (token != NULL) {

        if (strcmp(token, TOK_PIPE) == 0) {
            *seq += 1;
            token_list[*seq][position] = NULL;
            //memcpy(*token_list, tokens, position + 1);
            //token_list[*seq++] = tokens;
            //memset(tokens, 0, bufsize);
            position = 0;
        } else {

            token_list[*seq][position] = token;
            position++;
        }
        token = strtok(NULL, TOK_DELUM);
    }

    token_list[*seq][position] = NULL;
    //memcpy(token_list[*seq], tokens, position);
    token_list[++*seq] = NULL; 
    return token_list;
}
