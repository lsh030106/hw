#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

int lsh_launch(char ***args, int seq);
char ***split_line(char *line, int *seq);
void loop(void);
char *read_line(void);
int lsh_execute(char ***args, int seq);
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_num_builtins(void);

char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
};
/*int main(void) {

    char *ls[] = {"ls", NULL};
    char *grep[] = {"grep", ".c", NULL};
    char *grep2[] = {"grep", "m", NULL};
    char *grep3[] = {"grep", "2", NULL};

    char ***args = (char ***)malloc(sizeof(char **) * 4);

    args[0] = ls;
    args[1] = grep;
    args[2] = grep2;
    args[3] = grep3;

    for (int i = 4; i >= 0; i--) {
        if (args[i] == NULL) {
            break;
        }
        printf("%s\n", args[i][0]);
        
    }

    lsh_excute(args, 3);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < sizeof(char **); j++) {
            free(args[i][j]);
        }
        free(args[i]);
    }
    free(args);

    return 1;
}*/

int main(void) {
    
    loop();

    return 0;
}

#define ANSI_COLOR_BLUE "\x1b[1;34m"
#define ANSI_COLOR_BLACK "\x1b[0m"

void loop(void) {
    char *line;
    char ***args;
    char *dir;
    int status, seq = 0;

    do {
        dir = getcwd(NULL, 0);
        if (dir ==NULL) {
            fprintf(stderr, "lsh: search diretory error\n");
        }

        printf(ANSI_COLOR_BLUE "%s ", dir);
        printf(ANSI_COLOR_BLACK "> ");
        line = read_line();
        args = split_line(line, &seq);
        //status = lsh_execute(args, seq);
        printf("%d\n", seq);
        /*for (int i = 0; i < seq; i++) {
            printf("%s", args[i][0]);
        }*/
    } while (status);
}

#define RL_BUFSIZE 1024

char *read_line(void) {

    int bufsize = RL_BUFSIZE;
    int position = 0;
    char *buffer = (char *)malloc(sizeof(char) * bufsize);
    int c;

    if (! buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }

        position++;

        if (position >= bufsize) {
            bufsize += RL_BUFSIZE;
            buffer = (char *)realloc(buffer, bufsize);
            if (! buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

int lsh_execute(char ***args, int seq) {
    int i;

    if (args[0][0] == NULL) {
        return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0][0], builtin_str[i]) == 0) {
            return (*builtin_func[i]) (args[0]);
        }
    }

    return lsh_launch(args, seq);
}

int lsh_launch(char ***args, int seq) {

    int fd[2];
    pid_t pid;
    int status;


    if (pipe(fd) < 0) {
        printf("Create pipe error\n");
    }

    pid = fork();
    if (pid == 0) {
        dup2(fd[1], 1);
        close(fd[0]);

        // 자식 생성
        // 여기서 자식이 받아야할 값 fork
        if (seq != 1) { // seq 가 마지막 명령이 아니라면
            lsh_execute(args, seq - 1);
        }

        if (execvp(args[seq - 1][0], args[seq - 1]) == -1) {
            perror("error");
        }

        close(fd[1]);
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("error");
    } else {

        dup2(fd[0], 0);
        close(fd[1]);

        if (execvp(args[seq][0], args[seq]) == -1) {
            perror("error");
        }

        do {waitpid(pid, &status, WUNTRACED);
        } while (! WIFEXITED(status) && ! WIFSIGNALED(status));
    }

    return 1;
}

#define TOK_BUFSIZE 64
#define TOK_DELUM " \t\r\n\a"
#define TOK_PIPE "|"

char ***split_line(char *line, int *seq) {

    int bufsize = TOK_BUFSIZE, position = 0;
    char ***token_list = (char ***)malloc(sizeof(char **) * bufsize);
    char **tokens;
    char *token;

    if (! token_list) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELUM);
    while (token != NULL) {
        
        if (strcmp(token, TOK_PIPE) == 0) {
            tokens[++position] = NULL;
            token_list[*seq++] = tokens;
            position = 0;
        } else {
        
            tokens[position] = token;
            position++;
        }
        token = strtok(NULL, TOK_DELUM);
    }

    tokens[position] = NULL;
    token_list[*seq] = tokens;
    
    return token_list;
}

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int lsh_cd(char **args) {
    
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args) {
    int i;
    printf("LSH\n");
    printf("Type program names and arguments\n");
    
    for (i = 0; i < lsh_num_builtins(); i++) {
        printf(" %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}