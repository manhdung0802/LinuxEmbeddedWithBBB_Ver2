#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define OPEN_FILE "/home/as/Desktop/linuxEmbeddedBBB/codeExamples/file_handling/text.txt"
#define TX_BUFFER "DungNM1"

int main(void){
    int fd;
    ssize_t size = 0;
    fd = open(OPEN_FILE, O_RDWR);
    if(fd < 0) {
        printf("Open file failed\n");
        return -1;
    }

    size = write(fd, TX_BUFFER, sizeof(TX_BUFFER));
    // size trả về số byte mà ghi được vào file
    if(size < 0){
        printf("Write failed\n");
        close(fd);
        return -1;
    }

    printf("Write success\n");
    close(fd);

    return 0;
}