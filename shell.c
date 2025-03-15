#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h> //for open()

#include "helpers.h" //for parse fct

#define MAX_BG_PROCESSES 100

//current built-in commands
void myshell_help();
void myshell_pwd();
void myshell_cd(char** args);
void myshell_exit();
void myshell_wait();

int execute_external_cmd(char** args, int is_background);
int find_executable(char* command, char* full_path);
int execute_multiple_pipes(char*** commands, int num_cmds);
char*** split_commands_by_pipe(char** args, int* num_cmds);

// store background pids
pid_t bg_processes[MAX_BG_PROCESSES];
int bg_process_count = 0;

int main() {
    char* line = NULL;
    size_t len = 0;
    char** args;
    char delim[] = " \t\r\n";

    while (1) {
        if (getline(&line, &len, stdin) == -1) {
            if (feof(stdin)) {
                printf("\nExiting shell.\n");
                break;
            } else {
                perror("getline");
            }
        }

        args = parse(line, delim);

        if (args == NULL || args[0] == NULL) {
            free(args);
            continue;
            //Empty, ask user for input again
        }


        //Handle built-ins first
        if (strcmp(args[0], "help") == 0) {
            myshell_help();
        } else if (strcmp(args[0], "pwd") == 0) {
            myshell_pwd();
        } else if (strcmp(args[0], "cd") == 0) {
            myshell_cd(args);
        } else if (strcmp(args[0], "exit") == 0) {
            myshell_exit();
        } else if (strcmp(args[0], "wait") == 0) {
            myshell_wait();
        } else {
            //Check if should run in background
            int last_arg = 0;
            int is_background = 0;

            while (args[last_arg] != NULL) {
                last_arg++;
            }
            last_arg--;

            if (strcmp(args[last_arg], "&") == 0) {
                is_background = 1;
                args[last_arg] = NULL;
            }

            // Check for multiple pipes
            int num_cmds = 0;
            char*** commands = split_commands_by_pipe(args, &num_cmds);

            if (num_cmds > 1) {
                pid_t pid = fork();
                if (pid == 0) {
                    execute_multiple_pipes(commands, num_cmds);
                    exit(EXIT_SUCCESS);
                } else if (pid > 0) {
                    if (is_background) {
                        printf("Piped command running in background with PID: %d\n", pid);
                        if (bg_process_count < MAX_BG_PROCESSES) {
                            bg_processes[bg_process_count++] = pid;
                        } else {
                            fprintf(stderr, "Max background processes reached\n");
                        }
                        fflush(stdout);
                    } else {
                        waitpid(pid, NULL, 0);
                    }
                } else {
                    perror("fork");
                }
            } else {
                execute_external_cmd(args, is_background);
            }

            //Free memory
            for (int i = 0; i < num_cmds; i++) {
                free(commands[i]);
            }
            free(commands);
        }

        free(args); //free the memory allocated by parse
    }

    free(line);
    return 0;
}

//Week 4 Deliverable, also handles single pipes
int execute_multiple_pipes(char*** commands, int num_cmds) {
    int pipe_arr[num_cmds -1][2];

    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipe_arr[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            if (i > 0) {
                dup2(pipe_arr[i - 1][0], STDIN_FILENO);
            }
            if (i < num_cmds - 1) {
                dup2(pipe_arr[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipe_arr[j][0]);
                close(pipe_arr[j][1]);
            }

            char full_path[PATH_MAX];
            if (find_executable(commands[i][0], full_path) == -1) {
                fprintf(stderr, "%s: command not found\n", commands[i][0]);
                exit(EXIT_FAILURE);
            }
            execv(full_path, commands[i]);
            perror("execv");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipe_arr[i][0]);
        close(pipe_arr[i][1]);
    }

    for (int i = 0; i < num_cmds; i++) {
        int status;
        wait(&status);
    }

    return 0;
}

//Execute external program, Week 2 & 3 deliverable
int execute_external_cmd(char** args, int is_background) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        //In Child

        //Week 3: Redirection start
        int i = 0;
        int input_fd = -1, output_fd = -1;
        while (args[i] != NULL) {
            if (strcmp(args[i], ">") == 0) {
                //Is output redirection
                if (args[i + 1] == NULL) {
                    fprintf(stderr, "No file provided for output redirection\n");
                    exit(EXIT_FAILURE);
                }
                output_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (output_fd == -1) {
                    perror("open for output redirection");
                    exit(EXIT_FAILURE);
                }
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
                args[i] = NULL;
            } else if (strcmp(args[i], "<") == 0) {
                //Is Input redirection
                if (args[i + 1] == NULL) {
                    fprintf(stderr, "No file provided for input redirection\n");
                    exit(EXIT_FAILURE);
                }
                input_fd = open(args[i +1], O_RDONLY);
                if (input_fd == -1) {
                    perror("open for input redirection");
                    exit(EXIT_FAILURE);
                }
                dup2(input_fd, STDIN_FILENO);
                args[i] = NULL;
            }
            i++;
        }

        //Find the executable path
        char full_path[PATH_MAX];
        if (find_executable(args[0], full_path) == -1) {
            fprintf(stderr, "%s: command not found\n", args[0]);
            exit(EXIT_FAILURE);
        }

        if (execv(full_path, args) == -1) {
            perror("execv");
            exit(EXIT_FAILURE);
        } else {
            if (is_background) {
                if (bg_process_count < MAX_BG_PROCESSES) {
                    bg_processes[bg_process_count++] = pid;
                } else {
                    fprintf(stderr, "Max background processes reached\n");
                }
                printf("shell> ");
                fflush(stdout);
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
        }

        return 0;

    } else {
        int status;
        waitpid(pid, &status, 0); //Let child process finish

        //use methods to determine child status
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }

    //This code shouldn't execute by the child
    return 0;
}

//Used in conjunction with execv
int find_executable(char* command, char* full_path) {
    //Check if abs or rel path
    if (command[0] == '/' || strncmp(command, "./", 2) == 0) {
        strncpy(full_path, command, PATH_MAX);

        if (access(full_path, X_OK) == 0) {
            return 0;
        } else {
            perror("access");
            return -1;
        }
    }
    
    //If not abs or rel, search in PATH
    char* path_env = getenv("PATH");
    if (path_env == NULL) {
        fprintf(stderr, "PATH environment variable not found\n");
        return -1;
    }

    char* path = strdup(path_env);
    if (path == NULL) {
        perror("strdup");
        return -1;
    }

    char* dir = strtok(path, ":");
    while (dir != NULL) {
        snprintf(full_path, PATH_MAX, "%s/%s", dir, command);

        if (access(full_path, X_OK) == 0) {
            free(path);
            return 0;
        }
        //Move to next dir in PATH
        dir = strtok(NULL, ":");
    }

    //Not found
    free(path);
    return -1;
}

//Used to parse commands for pipe
char*** split_commands_by_pipe(char ** args, int* num_cmds) {
    int count = 0;
    int i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "|") == 0) {
            count++;
        }
        i++;
    }
    *num_cmds = count + 1;

    char*** commands = malloc((*num_cmds) * sizeof(char**));
    if (commands == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }


    //Splitting args int sub arrays, each is a different cmd
    int cmd_start = 0;
    int cmd_index = 0;

    for (int j = 0; j <= i; j++) {
        if (args[j] ==NULL || strcmp(args[j], "|") == 0) {
            int cmd_size = j - cmd_start;
            commands[cmd_index] = malloc((cmd_size + 1) * sizeof(char *));
            if (commands[cmd_index] == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            //Copy command to sub-array
            for (int k = 0; k < cmd_size; k++) {
                commands[cmd_index][k] = args[cmd_start + k];
            }
            commands[cmd_index][cmd_size] = NULL;

            cmd_start = j + 1;
            cmd_index++;
        }
    }
    return commands;
}




// <== Built-Ins : Week 1 / 4 ==>

//wait
void myshell_wait() {
    for (int i = 0; i < bg_process_count; i++) {
        if (bg_processes[i] != 0) {
            waitpid(bg_processes[i], NULL, 0);
            bg_processes[i] = 0;
        }
    }
    bg_process_count = 0;
}

//help
void myshell_help() {
    printf("Shell Commands:\n\n");
    printf("help - Displays all commands and info\n");
    printf("pwd - Prints current working directory\n");
    printf("cd [dir] - Changes directory to specified directory\n");
    printf("exit - Exits this shell\n");
    printf("wait - Waits for background process to finish\n\n");
    }

//cd
void myshell_cd(char** args) {
    if (args[1] == NULL) {
        const char* home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME environment variable not set\n");
        } else {
            chdir(home);
        } 
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
}

//pwd
void myshell_pwd() {
    char cwd[PATH_MAX]; //Max size of 4096 bytes, including "/0"
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
}

//exit
void myshell_exit() {
    printf("Exiting the shell.\n");
    exit(0);
}