#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define OPEN_FILE "/dev/kernel_dung"
#define DATA_WR "DungNM1"

int main(void){
    int fd = 0;
    ssize_t size = 0;
    fd = open(OPEN_FILE, O_RDWR);
    if(fd < 0) {
        printf("Open file failed\n");
        return -1;
    }

    size = write(fd, DATA_WR, strlen(DATA_WR));
    if(size < 0){
        printf("Write failed\n");
        close(fd);
        return -1;
    }

    return 0;
}