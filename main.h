//
// Created by clf on 2022/1/27.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <linux/limits.h>

#ifndef TINYSHELL_MAIN_H
#define TINYSHELL_MAIN_H

#ifdef _POSIX_PATH_MAX
#define PATHNAME_MAX		POSIX_PATH_MAX
#else
#define PATHNAME_MAX		1000
#endif

char *builtin_str[] = {
        "cd",
        "help",
        "exit"
};

struct tokens {
    char ** tokens;
    int size;
    int fds[2];
};

struct commands {
    struct tokens **tokens;
    int size;
};

int check_builtin(char **args);

char* get_path();

char* get_user();

void initPrint();

int lsh_builtin_num();

int lsh_exit(char **args);

int lsh_cd(char **args);

void lsh_loop();

int lsh_help(char **args);

char* lsh_read_line();

struct commands * lsh_split_line(char *lines);

int lsh_execute(char **args);

int lsh_launch(char **args);

int (*builtin_func[]) (char **) = {
        &lsh_cd,
        &lsh_help,
        &lsh_exit
};

#endif //TINYSHELL_MAIN_H
