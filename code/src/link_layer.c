// Link layer protocol implementation

#include "link_layer.h"
#include "port_connection.h"
#include "write.h"
// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

int nRetransmissions = 0;
int alarmEnabled = FALSE;

void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    nRetransmissions--;
    printf("Alarm triggered\n");
}


////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{
    int fd = port_connection(connectionParameters.serialPort, connectionParameters.baudRate);

    nRetransmissions = connectionParameters.nRetransmissions;
    int timeout = connectionParameters.timeout;
    MachineState state = START;
    unsigned char byte = 0x00;

    switch (connectionParameters.role)
    {
    case LlTx:
        (void)signal(SIGALRM, alarmHandler);
        while(nRetransmissions>0 && state!=STOP){
            if(alarmEnabled == FALSE){
                write_set(fd);
                alarm(timeout);
                alarmEnabled = TRUE;
                
            } 

            int byteread = read(fd, &byte, 1);
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
        }
        if (state != STOP) return -1;
        break;
    case LlRx:
        while(state!=STOP){
            int byteread = read(fd, &byte, 1);
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
        }
        //So manda uma vez o UA.
        write_ua(fd); 
        if (state != STOP) return -1;
        break;
    default:
        return 0;   
    }
    return fd;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(const unsigned char *buf, int bufSize)
{
    // TODO

    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    // TODO

    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{

    return 1;
}
