#include "shell.h"

int print_prompt = 1;

int main(int argc, char * argv[], char ** envp)
{
    init_shell(envp);

    print_welcome();

    while (TRUE) {
        shell_prompt();
        input_handle();
    }
}


int input_handle()
{
    char * user_line = (char *) calloc(LINE_SIZE, sizeof(char));

    fgets(user_line, LINE_SIZE, stdin);

    if (user_line[0] == '\n') // empty line -> exit from function
        return 0;

    char * commands[TOKENS_SIZE] = {};

    char * work = 0;
    work = strtok(user_line, "|");

    int tok_cnt = 0;
    while (work) {
        commands[tok_cnt++] = work;
        work = strtok(0, "|");
    }

    parse_commands(&commands[0], tok_cnt);

    free(user_line);
    print_prompt = 1;

    return 0;
}

int parse_commands(char * command_lines[], int argc)
{
    assert(command_lines);
    assert(argc > 0);

    command_t * commands = (command_t *) calloc(256, sizeof(command_t)); // commands in a line with pipes

#ifdef DEBUG
    printf("[parse commands] - <%x %d>\n", command_lines, argc);
    printf("default: \n");

    for (int i = 0; i < argc; i++)
        printf("[%d] (%s)\n", i, command_lines[i]);
#endif

    // divide every command line into tokens
    for (int i = 0; i < argc; i++) {
        char * work = 0;
        work = strtok(command_lines[i], delimiters);

        int tok_cnt = 0;
        while (work) {
            commands[i].args[tok_cnt++] = work;
            work = strtok(NULL, delimiters);
        }

        commands[i].arg_cnt = tok_cnt;
#ifdef DEBUG
        for (int j = 0; j < commands[i].arg_cnt; j++)
            printf("[%s] ", commands[i].args[j]);
#endif
    }

    handle_commands(commands, argc);

    free(commands);

    return 0;
}

void handle_commands(command_t commands[], int cmd_amount)
{
    assert(commands);
    assert(cmd_amount > 0);

    if (cmd_amount > 1) {
        handle_pipes(commands, cmd_amount);
        return;
    }

    if (strcmp(commands[0].args[0], "cd") == 0) {
        change_directory(commands[0].args, commands[0].arg_cnt);
        return;
    }

    if (strcmp(commands[0].args[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    }

    for (int i = 0; i < commands[0].arg_cnt; i++)
        {
            if (strcmp(commands[0].args[i], "<") == 0)
            {
                int input_fd = open(commands[0].args[i + 1], O_RDWR);
                if (input_fd < 0)
                {
                    fprintf(stderr, "Unable to open [%s]: %s", commands[0].args[i + 1], strerror(errno));
                    return;
                }

                //printf("i = %d arg_cnt = %d\n", i, commands[0].arg_cnt);

                // Handle output redirect that is right after <
                if (i + 4 == commands[0].arg_cnt && strcmp(commands[0].args[i + 2], ">") == 0)
                {
                    int output_fd = open(commands[0].args[i + 3], O_CREAT | O_RDWR, S_IRWXU);
                    if (output_fd < 0)
                    {
                        fprintf(stderr, "Unable to open [%s]: %s\n", commands[0].args[i + 3], strerror(errno));
                        return;
                    }

                    commands[0].args[i + 3] = NULL;
                    execute(&commands[0], INOUT_REDIRECT, input_fd, output_fd);

                    close(input_fd);
                    close(output_fd);

                    return;
                }

                commands[0].args[i] = NULL;

                execute(&commands[0], INPUT_REDIRECT, input_fd, -1);
                close(input_fd);

                return;
            }

            if (strcmp(commands[0].args[i], ">") == 0)
            {
                int output_fd = open(commands[0].args[i + 1], O_CREAT | O_RDWR, S_IRWXU);

                if (output_fd < 0)
                {
                    fprintf(stderr, "Unable to open [%s]: %s", commands[0].args[i + 1], strerror(errno));
                    return;
                }

                // Handle input redirect that is right after >
                if ((i + 4 == commands[0].arg_cnt) && (strcmp(commands[0].args[i + 2], "<") == 0) )
                {
                    int input_fd = open(commands[0].args[i + 3], O_RDWR, S_IRWXU);
                    if (input_fd < 0)
                    {
                        fprintf(stderr, "Unable to open [%s]: %s\n", commands[0].args[i + 3], strerror(errno));
                        return;
                    }

                    commands[0].args[i + 3] = NULL;
                    execute(&commands[0], INOUT_REDIRECT, input_fd, output_fd);

                    close(input_fd);
                    close(output_fd);

                    return;
                }

                commands[0].args[i] = NULL;
                execute(&commands[0], OUTPUT_REDIRECT, -1, output_fd);
                close(output_fd);

                return;
            }
        }

    execute(&commands[0], 0, -1, -1);
}

void handle_pipes(command_t commands[], int cmd_amount)
{
   assert(commands);
   assert(cmd_amount > 0);

   if (cmd_amount != 2)
       return;

   pid_t child_1 = -1, child_2 = -1;
   int filedes[2], status = 0;

   if (pipe(filedes) == -1) {
       perror("pipe");
       return;
   }

   child_1 = fork();

   if (child_1 == 0) {
       dup2(filedes[1], STDOUT_FILENO);

       close(filedes[0]);
       close(filedes[1]);

       execvp(commands[0].args[0], commands[0].args);
       perror("exec");
       exit(EXIT_FAILURE);
   } else if (child_1 < 0) {
       perror("fork error");
       exit(EXIT_FAILURE);
   }

   close(filedes[1]);

   child_2 = fork();

   if (child_2 == 0) {
       dup2(filedes[0], STDIN_FILENO);

       close(filedes[0]);
       close(filedes[1]);

       execvp(commands[1].args[0], commands[1].args);
       perror("exec");
       exit(EXIT_FAILURE);
   } else if (child_2 < 0) {
       perror("fork error");
       exit(EXIT_FAILURE);
   }

   close(filedes[0]);
   close(filedes[1]);

   while (wait(&status) != -1) {}
}

void execute(command_t * command, int option, int input_fd, int output_fd)
{
    assert(command);

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Unable to create new process: %s", strerror(errno));
        return;
    }
    else if (pid == 0) {
        switch(option) {
            case 0:     // no output redirect
                break;
            case OUTPUT_REDIRECT:     // output redirect
                dup2(output_fd, STDOUT_FILENO);
                break;
            case INPUT_REDIRECT:
                dup2(input_fd, STDIN_FILENO);
                break;
            case INOUT_REDIRECT:
                printf("inout redirect\n");
                dup2(input_fd, STDIN_FILENO);
                dup2(output_fd, STDOUT_FILENO);
                break;
            default:
                fprintf(stderr, "Wrong options in execute function!\n");
                return;
        }

        execvp(command->args[0], command->args);

        fprintf(stderr, "Unable to exec: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        waitpid(pid, NULL, 0);
    }
}

void change_directory(char * dir[], int argc)
{
    if (argc == 1)
        chdir(getenv("HOME"));
    else if (argc == 2)
        chdir(dir[1]);
    else
    {
        printf("Wrong argument format!\n");
        printf("argc = %d\n", argc);
    }
}

void init_shell(char ** envp)
{
    environ = envp;

    shell_is_interactive = isatty(STDIN_FILENO);

    if (shell_is_interactive)
    {
        if ((setenv("shell", getcwd(current_dir, DIR_SIZE), 1)) == -1) {
            fprintf(stderr, "setenv error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        shell_pid = getpid();

        //! until we are a foreground process - loop

        while (tcgetpgrp(STDIN_FILENO) != (shell_pgid = getpgrp()))
            kill(shell_pid, SIGTTIN);

        setpgid(shell_pid, shell_pid);

        shell_pgid = getpgrp();

        if (shell_pid != shell_pgid) {
            printf("Shell it nos process group leader");
            exit(EXIT_FAILURE);
        }

        //! Start controlling STDIN and STDOUT in shell

        tcsetpgrp(STDIN_FILENO, shell_pid);
    }
    else {
        fprintf(stderr, "Shell is not interactive");
        exit(EXIT_FAILURE);
    }
}

void print_welcome()
{
    printf("------------------------------------\n");
    printf("-------Welcome to simple shell!-----\n");
    printf("------------------------------------\n");
}

void shell_prompt()
{
    char host_name[HOST_SIZE];
    gethostname(host_name, sizeof(host_name));

    printf("<%s@%s: %s> $  ", getenv("LOGNAME"), host_name, getcwd(current_dir, DIR_SIZE));
}
