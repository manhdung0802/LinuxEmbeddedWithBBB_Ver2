#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_FILE "/dev/device_file_device"
#define TX_BUFFER "1"

int main(void){
    int fd = 0;
    ssize_t size = 0;
    fd = open(DEVICE_FILE, O_RDWR);
    if(fd < 0) {
        printf("Open file failed\n");
        return -1;
    }

    size = write(fd, TX_BUFFER, strlen(TX_BUFFER));
    if(size < 0){
        printf("Write failed\n");
        close(fd);
        return -1;
    }

    return 0;
}