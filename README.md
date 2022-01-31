# TinyShell

This is a tiny shell implementation referring to [Tutorial - Write a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/), and I add some extention to it.

## OVERVIEW

There are three main steps in total for shell, which are Initialize, Interpret, Execute.
 
* Initialize: the shell should read and execute its configuration files to initialize the state. For simplicity, I don't add this process.
* Interpret: You know, shell is an interactive program, so it is important to interpret user's input properly. In my shell, user can only input one line at a time. What's more, the tutorial don't support **"tokens"**, I add this function.
* Execute: Typically, there are two kinds of command: the first one is execute some executable files in subprocess, by means of system call *fork()* and *execvp()*. The other is called builtin commands, for example, *cd* to change current directory, *exit* to quit the shell, *help* to show help information, e.g..

## DETAILS

In main function, it calls a function lsh_loop(), to read from terminal and execute commands until user input exit.

To make shell looks more pretty and more convenient to use, call initPrint to show the current user and current directory. Function *get_user()* and *get_path()* return username and pathname. As these two function are very short, so I use inline to reduce the cost.

After initPrint, use *lsh_read_line()* to read input from stdin, mainly use *getline()*. After read, need to check whether read is successfully, and return a type char pointer, aka line defined in the function.

The following function is *lsh_split_line()*, which is one of the most function in the shell. Different from tutorial, I add pipe, aka '|', which is a very useful feature and one of the most significant ways of process communication in Linux. we split lines by blank space, quotation mark and pipe symbol.
For convenience, I define two struct, tokens and commands:
```C
struct tokens {
    char ** tokens;
    int size;
    int fds[2];
};

struct commands {
    struct tokens **tokens;
    int size;
};
```
Size is the number of token in tokens, and tokens in commands. fds is file descriptor, which is used in pipe line to redirect STDIN and STDOUT to pipe.
When finish parsing tokens, aka a single command, add it to commands to return.

Subsequently, I should check the size of commands, if size == 1, there are only 1 command so don't need pipeline, and just spread args to *lsh_execute()* to execute it.
Otherwise, there should be at least two commands and the input of the latter should from the output of the former.Before redirect stdin and stdout, should check whether there is builtin command.
Because builtin command don't start a new process, therefore, pipe should not exist between them. Under this situation, output error message to hint user there is a wrong commmand.
After check, malloc pipes and redirect stdin/stdout to it. Then call *lsh_execute()* function to execute commands.

In lsh_execute, check args[0] to confirm the type of commands. We define a function pointer to conveniently check and call builtin function, 
```C
char *builtin_str[] = {
        "cd",
        "help",
        "exit"
};
```
if the command is not a builtin one, use the system call *fork()* to start a subprocess. If return value is 0, then the current function is child, otherwise is parent.
Because images of child and parent are identical, child process should call *execvp()* to substitute its image. And the parent process use *waitpid()* to wait for child.

After execute a command, use *free()* to recycle memory.

## TODO
- [ ] fix pipe bug.
- [ ] check memory leakage.
