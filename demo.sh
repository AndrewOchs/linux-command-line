#!/bin/bash

mkdir demo;

echo "#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*Program to test pipe. Reads from stdin and outputs to stdout that input twice.*/

int main(int argc, char **argv){
    char buf[128];
    for(size_t i = 0; i<128; ++i){   //cleans buffer
        buf[i] = '\0';
    }
    ssize_t num;
    while((num = read(STDIN_FILENO, buf, 128)) > 1){
        buf[num] = '\0';  //for strlen
        write(STDOUT_FILENO, buf, strlen(buf));
        write(STDOUT_FILENO, buf, strlen(buf));
    }
}" > p.c;

echo "#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*Program to test redirection. Reads from stdin and outputs to stdout.*/

int main(int argc, char **argv){
    char buf[128];
    for(size_t i = 0; i<128; ++i){   //cleans buffer
        buf[i] = '\0';
    }
    ssize_t num;
    while((num = read(STDIN_FILENO, buf, 128)) > 1){
        buf[num] = '\0';  //for strlen
        write(STDOUT_FILENO, buf, strlen(buf));
    }
}" > r.c;

echo "#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

/*Program to test background processes. Sleeps for 10 then prints DONE.*/

int main(int argc, char **argv){
    sleep(10);
    puts(\"DONE\");
}" > s.c;

echo "#include <string.h>
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
    char *messages[19] = {\"help\n\", \"cd demo\n\", \"pwd\n\", \"cd ..\n\", \"pwd\n\",
                          \"/usr/bin/wc in.txt\n\", \"wc in.txt\n\", \"wc < in.txt\n\", \"ls > out.txt\n\", \"cat out.txt\n\", 
                          \"wc < in.txt > new.txt\n\", \"cat new.txt\n\",
                          \"ls | grep .txt\n\",
                          \"ls | grep .txt | grep n\n\",
                          \"./s &\n\", \"./s &\n\", \"wait\n\", \"exit\n\", NULL};

    int index = 0;
    while (messages[index] != NULL)
    {
        write(STDOUT_FILENO, messages[index], strlen(messages[index]));
        ++index;
        sleep(1);
    }
}" > shelltest.c

gcc -o s s.c;
gcc -o t shelltest.c

echo "This is input text" > in.txt;



