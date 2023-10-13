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

    switch (connectionParameters.role)
    {
    case LlTx:
        (void)signal(SIGALRM, alarmHandler);
        while(nRetransmissions>0 && state!=STOP){
            if(alarmEnabled==FALSE){
                write_set(fd);
                alarm(timeout);
                alarmEnabled = TRUE;
                
            }   
            //printf("Waiting for UA\n");
            if(read_ua(fd, state)){
                    printf("UA received\n");
                    alarmEnabled = FALSE;
                    state = STOP;
                    //alarm(0);
                }
        }
        if (state != STOP) return -1;
        break;
    case LlRx:
        /* code */
        break;
    default:
        break;
    }
    return 1;
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
    // TODO

    return 1;
}
