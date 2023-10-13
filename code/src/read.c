#include "read.h"
int read_ua(int fd, MachineState state){
    unsigned char byte;
    int byteread=0;
    while (state != STOP) {

        byteread=read(fd, &byte, 1);
        //printf("byte read: %d\n",byteread);
        if (byteread > -1) {
            //printf("byte:%x\n",byte);
            switch (state) {
                case START:
                    if(byte == FLAG) state = FLAG_RCV;
                    break;
                case FLAG_RCV:
                    if(byte == A_RX) state = A_RCV;
                    else if(byte != FLAG) state = START;
                    break;
                case A_RCV:
                    if(byte == FRAME_UA) state = C_RCV;
                    else if(byte == FLAG) state = FLAG_RCV;
                    else state = START;
                    break;
                case C_RCV:
                    if(byte == (A_RX ^ FRAME_UA)) state = BCC_OK;
                    else if(byte == FLAG) state = FLAG_RCV;
                    else state = START;
                    break;
                case BCC_OK:
                    if(byte == FLAG) state = STOP;
                    else state = START;
                    break;
                default:
                    break;
            }
        }else{
            exit(0);
        }
    }
    return 1;
}


int read_set(int fd, MachineState state){
    unsigned char byte;
    while (state != STOP) {
        if (read(fd, &byte, 1) > 0) {
            switch (state) {
                case START:
                    if(byte == FLAG) state = FLAG_RCV;
                    break;
                case FLAG_RCV:
                    if(byte == A_TX) state = A_RCV;
                    else if(byte != FLAG) state = START;
                    break;
                case A_RCV:
                    if(byte == FRAME_SET) state = C_RCV;
                    else if(byte == FLAG) state = FLAG_RCV;
                    else state = START;
                    break;
                case C_RCV:
                    if(byte == (A_TX ^ FRAME_SET)) state = BCC_OK;
                    else if(byte == FLAG) state = FLAG_RCV;
                    else state = START;
                    break;
                case BCC_OK:
                    if(byte == FLAG) state = STOP;
                    else state = START;
                    break;
                default: 
                    break;
            }
        }else{
            break;
        }
    }
    return 1;

}

