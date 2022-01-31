#include "main.h"

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64

inline int lsh_exit(char **args) {
    return 0;
}

inline int lsh_builtin_num() {
    return sizeof(builtin_str) / sizeof(char *);
}

char* lsh_read_line() {
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

struct commands *lsh_split_line(char *lines) {
    int bufsize = LSH_TOK_BUFSIZE;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token = malloc(LSH_RL_BUFSIZE * sizeof(char));
    int tokensize = LSH_RL_BUFSIZE;
    struct commands *cmd = malloc(sizeof(struct commands));
    cmd->tokens = malloc(sizeof(struct tokens));
    cmd->size = 0;

    if (tokens == NULL) {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    int pos = 0, cur = 0;

    while (*lines != '\n') {
        if (*lines == ' ') {
            if (cur > 0) {
                *(tokens + pos) = malloc(cur * sizeof(char));
                strncpy(*(tokens + pos), token, cur);
                pos++;
                cur = 0;
            }
            lines++;
        }
        else if (*lines == '\"') {
            lines++;
            while (*lines != '\n' && *lines != '\"') {
                *(token + cur) = *lines++;
                cur++;
                if (cur >= tokensize) {
                    tokensize += LSH_RL_BUFSIZE;
                    token = realloc(token, tokensize);
                }
            }
            if (*lines == '\n') {
                fprintf(stderr, "parse error.Please check your input\n");
                return NULL;
            }
            if (*lines == '\"') {
                *(tokens + pos) = malloc(cur * sizeof(char));
                strncpy(*(tokens + pos), token, cur);
                pos++;
                cur = 0;
                lines++;
            }
        }
        else if (*lines == '|') {
            struct tokens *tmp = malloc(sizeof(struct tokens));
            tmp->tokens = tokens;
            tmp->size = pos + 1;

            //cmd->tokens = malloc(sizeof(struct tokens *));
            cmd->tokens[cmd->size++] = tmp;
            //*(cmd->tokens + cmd->size) = tmp;
            //cmd->size++;
            cmd = realloc(cmd, sizeof(struct commands));

            pos = 0;
            cur = 0;
            tokens = malloc(bufsize * sizeof(char *));
            lines++;
        }
        else {
            *(token + cur) = *lines++;
            cur++;
            if (cur >= tokensize) {
                tokensize += LSH_RL_BUFSIZE;
                token = realloc(token, tokensize);
            }
        }
    }
    if (cur > 0) {
        *(tokens + pos) = malloc(sizeof(char) * cur);
        strncpy(*(tokens + pos), token, cur);
    }

    free(token);

    struct tokens tmp;
    tmp.tokens = tokens;
    tmp.size = pos + 1;

    //cmd->tokens = malloc(sizeof (struct tokens *));
    cmd->tokens[cmd->size] = &tmp;
    //*(cmd->tokens + cmd->size) = &tmp;
    cmd->size++;
    //printf("%s\n", cmd->tokens[cmd->size - 1]->tokens[0]);
    return cmd;
}

/*
 * if args is builtin, return index
 * else return -1
 */
int check_builtin(char **args) {
    int i = 0;

    for (i = 0; i < lsh_builtin_num(); ++i) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int lsh_execute(char **args) {

    if (args[0] == NULL) {
        return 1;
    }

    int i = check_builtin(args);

    if (i != -1) {
        return (*builtin_func[i])(args);
    }

    return lsh_launch(args);
}

int lsh_cd(char **args) {
    if (args == NULL) {
        fprintf(stderr, "expected argument to \"cd\"\n");
    }
    else {
        if (strcmp(args[1], ".") == 0) return 1;
        else if (strcmp(args[1], "~") == 0) {
            char *user = get_user();
            char path[PATHNAME_MAX] = "/home/";
            strcat(path, user);
            if (chdir(path) != 0) {
                fprintf(stderr, "%s\n", strerror(errno));
            }
        }
        else if (strcmp(args[1], "..") == 0) {
            char *path = get_path();
            if (strcmp(path, "/") != 0) {
                char *ptr = path + strlen(path) - 1;
                while (ptr != path && *ptr != '/') {
                    ptr--;
                }
                char *target = malloc(sizeof(char) * (ptr - path + 1));
                strncpy(target, path, ptr - path + 1);
                if (chdir(target) != 0) {
                    fprintf(stderr, "%s\n", strerror(errno));
                }
            }
            free(path);
        }
        else if (chdir(args[1]) != 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            //fprintf(stderr, "cd error\n");
        }
    }
    return 1;
}

int lsh_help(char **args) {
    int i;
    printf("Type program names and arguments\n");
    printf("The following are built in:\n");

    for (i = 0; i < lsh_builtin_num(); ++i) {
        printf("%s\n", builtin_str[i]);
    }

    printf("Use the man command for other program information, like \"man shell\".\n");
    return 1;
}

int lsh_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        //printf("%s\n", args[0]);
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            //perror("execvp error\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid < 0) {
        perror("fork error\n");
    }
    else {
        wpid = waitpid(pid, &status, WUNTRACED);
    }
    return 1;
}

inline char* get_user() {
    struct passwd *pwd = getpwuid(getuid());

    if (pwd == NULL) {
        fprintf(stderr, "getpwuid error: %s", strerror(errno));
    }
    return pwd->pw_name;
}

inline char* get_path() {
    char *path = malloc(PATHNAME_MAX * sizeof(char));

    if (NULL == getcwd(path, PATHNAME_MAX)) {
        fprintf(stderr, "getcwd error: %s", strerror(errno));
        exit(1);
    }
    return path;
}

inline void initPrint() {
    char *path = get_path();
    char *user = get_user();
    char cmp_path[PATHNAME_MAX] = "/home/";
    strcat(cmp_path, user);
    if (strcmp(cmp_path, path) == 0)
        printf("[%s:~]> $ ", user);
    else
        printf("[%s:%s]> $ ", user, path);
    free(path);
}

void GC(struct tokens *args) {
    for (int i = 0; i < args->size; ++i) {
        free(args->tokens[i]);
    }
    free(args->tokens);
    free(args);
}

void lsh_loop() {
    char *lines;
    struct commands *args;
    int status;

    do {
        initPrint();
        lines = lsh_read_line();
        if (lines == NULL) continue;
        //printf("read lines: %s\n", lines);

        args = lsh_split_line(lines);

        if (args->size == 1) {              //no pipe
            if (args->tokens[args->size-1]->tokens == NULL || args->tokens[args->size - 1]->size == 0) continue;

            args->tokens[args->size - 1]->fds[STDIN_FILENO] = STDIN_FILENO;
            args->tokens[args->size - 1]->fds[STDOUT_FILENO] = STDOUT_FILENO;
            status = lsh_execute(args->tokens[args->size - 1]->tokens);
        }
        else {                              // at least one pipe
            int i;
            for (i = 0; i < args->size; ++i) {
                if (check_builtin(args->tokens[i]->tokens) != -1) {
                    fprintf(stderr, "there is no builtin in pipe.\n");
                }
            }

            int pipe_cnt = args->size - 1;
            int (*pipes)[2] = calloc(pipe_cnt * sizeof(int[2]), 1);

            if (pipes == NULL) {
                fprintf(stderr, "pipe malloc error.\n");
            }

            args->tokens[0]->fds[STDIN_FILENO] = STDIN_FILENO;
            for (i = 1; i < args->size; ++i) {
                pipe(pipes[i-1]);
                args->tokens[i-1]->fds[STDOUT_FILENO] = pipes[i-1][1];
                args->tokens[i]->fds[STDIN_FILENO] = pipes[i-1][0];
            }
            args->tokens[pipe_cnt]->fds[STDOUT_FILENO] = STDOUT_FILENO;

            for (i = 0; i < args->size; ++i) {
                if (args->tokens[i]->fds[0] != -1 && args->tokens[i]->fds[0] != STDIN_FILENO) {
                    dup2(args->tokens[i]->fds[0], STDIN_FILENO);
                }
                if (args->tokens[i]->fds[1] != -1 && args->tokens[i]->fds[1] != STDOUT_FILENO) {
                    dup2(args->tokens[i]->fds[1], STDOUT_FILENO);
                }

                lsh_execute(args->tokens[i]->tokens);
            }

            free(pipes);
        }

        /*
        for (int i = 0; i < args->size; ++i) {
            printf("%s\n", args->tokens[i]);
        }
         */

        free(args);
    } while(status);
}

int main(int argc, char **argv) {
    //load config files

    // lsh_loop()
    lsh_loop();
    return EXIT_SUCCESS;
}