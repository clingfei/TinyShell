#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64


char* lsh_read_line() {
    /*
    int bufsize = LSH_RL_BUFSIZE;
    char* buffer = malloc(sizeof(char) * bufsize);
    if (buffer == NULL) {
        fprintf(stderr, "malloc error\n");
        exit(EXIT_FAILURE);
    }
    char c;
    int position = 0;
    while (1) {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\n';
            return buffer;
        }
        buffer[position++] = c;

        if (position >= LSH_RL_BUFSIZE) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (buffer == NULL) {
                fprintf(stderr, "realloc error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
     */
    char *line = NULL;
    ssize_t bufsize =0;
    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);     //EOF
        }
        else {
            perror("readline");         //error
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

char** lsh_split_line(char *lines) {
    int bufsize = LSH_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token = malloc(LSH_RL_BUFSIZE * sizeof(char));
    int tokensize = LSH_RL_BUFSIZE;

    if (tokens == NULL) {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }
    int pos = 0, cur = 0;
    while (lines[position] != '\n') {
        if (lines[position] == ' ') {
            if (cur > 0) {
                strncpy(tokens[pos], token, cur);
                pos++;
                cur = 0;
            }
            position++;
        }
        else if (lines[position] == '"') {
            while (lines[position] != '\n' && lines[position] != '"') {
                token[cur++] = lines[position++];
                if (cur >= tokensize) {
                    tokensize += LSH_RL_BUFSIZE;
                    token = realloc(token, tokensize);
                }
            }
            if (lines[position] == '\n') {
                fprintf(stderr, "parse error.Please check your input\n");
                return NULL;
            }
            if (lines[position] == '"') {
                strncpy(tokens[pos], token, cur);
                pos++;
                cur = 0;
            }
        }
        else {
            token[cur++] = lines[position++];
            if (cur >= tokensize) {
                tokensize += LSH_RL_BUFSIZE;
                token = realloc(token, tokensize);
            }
        }
    }
    return tokens;
}

int lsh_execute(char **args) {

}

void lsh_loop() {
    char *lines;
    char **args;
    int status;

    do {
        printf("> $");
        lines = lsh_read_line();
        if (lines == NULL) continue;
        args = lsh_split_line(lines);
        if (args == NULL) continue;
        status = lsh_execute(args);

        free(lines);
        free(args);
    } while(status);
}

int main(int argc, char **argv) {
    //load config files

    // lsh_loop()
    lsh_loop();
    return EXIT_SUCCESS;
}
