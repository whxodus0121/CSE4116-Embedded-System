#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

#define STOPWATCH_DEVICE "/dev/stopwatch"
#define DEVICE_MAJOR_NUMBER 242

int main(int argc, char **argv){
    int fd;

    /*stopwatch open후 시작*/
    fd = open(STOPWATCH_DEVICE, O_WRONLY);
    write(fd, NULL, 0);

    close(fd);
    return 0;
}
