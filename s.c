#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

/*Program to test background processes. Sleeps for 10 then prints DONE.*/

int main(int argc, char **argv){
    sleep(10);
    puts("DONE");
}
