#ifndef SHELL_H
#define SHELL_H

#define TRUE 1
#define FALSE 0

#define DIR_SIZE 1024
#define HOST_SIZE 1024
#define LINE_SIZE 1024
#define TOKENS_SIZE 256

#define EXIT 1

#define OUTPUT_REDIRECT 1
#define INPUT_REDIRECT 2
#define INOUT_REDIRECT 3
//!--------------------------------

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

//!--------------------------------

struct command_t {
    char * args[256];
    int arg_cnt;
};

static pid_t shell_pid;
static pid_t shell_pgid;

static int shell_is_interactive;

char EXIT_CMD[] = "exit";
char delimiters[] = {"\n\t "};
char pipe_delim[] = {"|"};
char current_dir[1024];
extern char ** environ;

//!--------------------------------

void print_welcome();
void init_shell(char ** envp);
void shell_prompt();
int  input_handle();
int  parse_commands(char * commands_arr[], int argc);
void handle_commands(command_t commands[], int cmd_amount);
void handle_pipes(command_t commands[], int cmd_amount);
void execute(command_t * command, int option, int input_fd, int output_fd);
void change_directory(char * dir[], int argc);
//!--------------------------------


//!--------------------------------
#endif // SHELL_H
