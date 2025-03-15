#include <stdio.h>
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
}
