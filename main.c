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

struct tokens *lsh_split_line(char *lines) {
    int bufsize = LSH_TOK_BUFSIZE;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token = malloc(LSH_RL_BUFSIZE * sizeof(char));
    int tokensize = LSH_RL_BUFSIZE;

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
        else if (*lines == '"') {
            while (*lines != '\n' && *lines != '"') {
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
            if (*lines == '"') {
                *(tokens + pos) = malloc(cur * sizeof(char));
                strncpy(*(tokens + pos), token, cur);
                pos++;
                cur = 0;
            }
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
    *(tokens + pos) = malloc(sizeof (char) * cur);
    strncpy(*(tokens + pos), token, cur);

    free(token);

    struct tokens* res;
    res->tokens = tokens;
    res->size = pos + 1;
    return res;
}

int lsh_execute(char **args) {
    int i = 0;

    if (args[0] == NULL) {
        return 1;
    }

    for (i = 0; i < lsh_builtin_num(); ++i) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            //printf("args: %s, target: %s\n", args[0], builtin_str[i]);
            return (*builtin_func[i])(args);
        }
    }
    printf("%s\n", args[0]);
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
            if (strcmp(path, "/") == 0)
                return 1;
            else {
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
        if (execvp(args[0], args) == -1) {
            perror("execvp error\n");
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
    printf("[%s:%s]> $ ", get_user(), get_path());
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
    struct tokens *args;
    int status;

    do {
        initPrint();
        lines = lsh_read_line();
        if (lines == NULL) continue;
        //printf("read lines: %s\n", lines);

        args = lsh_split_line(lines);

        if (args->tokens == NULL || args->size == 0) continue;

        /*
        for (int i = 0; i < args->size; ++i) {
            printf("%s\n", args->tokens[i]);
        }
         */

        status = lsh_execute(args->tokens);

        free(args);
    } while(status);
}

int main(int argc, char **argv) {
    //load config files

    // lsh_loop()
    lsh_loop();
    return EXIT_SUCCESS;
}