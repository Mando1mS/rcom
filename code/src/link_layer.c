// Link layer protocol implementation

#include "link_layer.h"
#include "port_connection.h"
#include "write.h"
// MISC

int count_tx=0;
int nRetransmissions = 0;
int alarmEnabled = FALSE;
int timeout=0;
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
    timeout = connectionParameters.timeout;
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
int llwrite(int fd,const unsigned char *buf, int bufSize)
{
    MachineState state = START;
    unsigned char *frame=create_packet(fd,buf,bufSize,&count_tx);
    (void)signal(SIGALRM, alarmHandler);
    unsigned char byte=0x00;
    int acepted=0;
    int rejected =0;
    while(nRetransmissions>0 && state!=STOP){
        unsigned char answer=0x00;
        if(alarmEnabled == FALSE){
                write(fd,frame,bufSize+6);
                //sleep(1);
                alarm(timeout);
                alarmEnabled = TRUE;
                acepted=0;
                rejected=0;
        } 
        while(alarmEnabled==TRUE && acepted==FALSE && rejected==FALSE){
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
                    if(byte == FRAME_RR_0 || byte == FRAME_RR_1 ||
                     byte == FRAME_REJ_0 || byte == FRAME_REJ_1 ||
                      byte == FRAME_DISC)
                    {
                        state = C_RCV;
                        answer=byte;
                    } 
                    else if(byte == FLAG) state = FLAG_RCV;
                    else state = START;
                    break;
                case C_RCV:
                    if(byte == (A_RX ^ answer)) state = BCC_OK;
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
            if(answer==0x00)
            {
                continue;
            }
            else if(answer==FRAME_REJ_0 || answer==FRAME_REJ_1)
            {
                printf("Packet rejeitado\n");
                rejected=1;
            }  
            else if(answer==FRAME_RR_0 || answer==FRAME_RR_1)
            {
                acepted=1;
                count_tx=(count_tx+1)%2;
            }
            else continue;
        }
        if(acepted==TRUE)
            break;
        if (state != STOP)
        {
            free(frame);
            return -1;
        }
    }
    free(frame);
    if(acepted==TRUE)
    {
        return 1;
    }
    else 
    {
        llclose(fd);
        return 0;
    }
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
