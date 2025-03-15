#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

/*Program to test full shell
run using:
    ./t | ./shell
Expected output:
    Help 
        -help
    path that ends with /demo
        -cd and pwd
    path that does not end with /demo
    1  4 19 in.txt
        -full path programs
    1  4 19 in.txt
        -non full path programs
    1  4 19
        -redirect in
    ls output
        -redirect out
    1 4 19
        -redirect both
    a list of .txt files
        -one pipe
    a list of .txt files containing the letter n
        -indefinite pipes
    DONE 
    DONE
        -There should be a 10 second wait here
        -background processes and wait
    Shell should exit here
        -exit
*/

int main(int argc, char **argv)
{
    char *messages[19] = {"help\n", "cd demo\n", "pwd\n", "cd ..\n", "pwd\n",
                          "/usr/bin/wc in.txt\n", "wc in.txt\n", "wc < in.txt\n", "ls > out.txt\n", "cat out.txt\n", 
                          "wc < in.txt > new.txt\n", "cat new.txt\n",
                          "ls | grep .txt\n",
                          "ls | grep .txt | grep n\n",
                          "./s &\n", "./s &\n", "wait\n", "exit\n", NULL};

    int index = 0;
    while (messages[index] != NULL)
    {
        write(STDOUT_FILENO, messages[index], strlen(messages[index]));
        ++index;
        sleep(1);
    }
}
