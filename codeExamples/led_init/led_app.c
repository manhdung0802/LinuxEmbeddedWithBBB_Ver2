#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define LED_FILE "/dev/led_kernel"

int main(int argc, char* argv[]){
    int fd;
    fd = open(LED_FILE, O_RDWR);
    if(fd<0){
        printf("Open file failed\n");
        return -1;
    }

    // argv[1] là đối số thứ 1 sau tên chương trình
    // argv[0] là ./led_app
    write(fd, argv[1], 1);

    close(fd);

    return 0;
}