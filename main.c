#include <stdio.h>
#include <stdlib.h>

#define LSH_RL_BUFSIZE 1024
#define EXIT_
char* lsh_read_line() {
    int bufsize = LSH_RL_BUFSIZE;
    char* buffer = malloc(sizeof(char) * bufsize);
    if (buffer == NULL) {
        fprintf(stderr, "malloc error\n");
        return EXIT_FAILURE;
    }
}

char** lsh_split_line(char *lines) {

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
        args = lsh_split_line(lines);
        status = lsh_execute(args);

        free(lines);
        free(args);
    } while(status)
}

int main(int argc, char **argv) {
    //load config files

    // lsh_loop()
    lsh_loop();
    return EXIT_SUCCESS;
}
