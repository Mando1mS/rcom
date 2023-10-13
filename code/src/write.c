#include "write.h"

int write_set(int fd){
    printf("write_set\n");
    unsigned char buf[5] = {FLAG, A_TX, FRAME_SET, (A_TX^FRAME_SET), FLAG};
    int bytes = write(fd, buf, 5);
    sleep(1);
    return bytes;
}

int write_ua(int fd)
{
    printf("write_set\n");
    unsigned char buf[5] = {FLAG, A_RX, FRAME_UA, (A_RX^FRAME_UA), FLAG};
    int bytes = write(fd, buf, 5);
    sleep(1);
    return bytes;
}
