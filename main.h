//
// Created by 15991 on 2022/1/27.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#ifndef TINYSHELL_MAIN_H
#define TINYSHELL_MAIN_H

char *builtin_str[] = {
        "cd",
        "help",
        "exit"
};

struct tokens {
    char ** tokens;
    int size;
};

int lsh_builtin_num();

int lsh_exit(char **args);

int lsh_cd(char **args);

void lsh_loop();

int lsh_help(char **args);

char* lsh_read_line();

struct tokens * lsh_split_line(char *lines);

int lsh_execute(char **args);

int lsh_launch(char **args);

int (*builtin_func[]) (char **) = {
        &lsh_cd,
        &lsh_help,
        &lsh_exit
};

#endif //TINYSHELL_MAIN_H
