// Link layer protocol implementation

#include "link_layer.h"
#include "port_connection.h"
#include "write.h"
// MISC

int count_tx=0;
int count_rx=0;
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
    alarmEnabled = FALSE;
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

            read(fd, &byte, 1);
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
            read(fd, &byte, 1);
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
        write_ua(fd,LlRx); 
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
    alarmEnabled = FALSE;
    MachineState state = START;
    int j=0;
    printf("frame created\n");

    unsigned char *frame=create_packet(fd,buf,bufSize,&count_tx,&j);



    (void)signal(SIGALRM, alarmHandler);
    unsigned char byte=0x00;
    int acepted=0;
    int rejected =0;
    while(nRetransmissions>0 && state!=STOP){
        unsigned char answer=0x00;
        if(alarmEnabled == FALSE){
                printf("A mandar o frame\n");
                write(fd,frame,j);
                printf("Frame enviado\n");
                alarm(timeout);
                alarmEnabled = TRUE;
                acepted=0;
                rejected=0;
        } 
        while(alarmEnabled==TRUE && state!=STOP){
            read(fd, &byte, 1);
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
        if(rejected==TRUE)
        {
            nRetransmissions--;
            printf("A reenviar o frame\n");
            alarmEnabled = FALSE;
            state=START;
            continue;
        }
        if(acepted==TRUE)
            break;   
    }
    if (state != STOP)
        {
            free(frame);
            return -1;
        }
    free(frame);
    if(acepted==TRUE)
    {   
        printf("Frame aceite\n");
        printf("Packet sent successfully\n");
        return 1;
    }
    else 
    {
        llclose(fd,LlTx);
        return 0;
    }
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(int fd, unsigned char *packet)
{
    alarmEnabled = FALSE;
    MachineState state = START;
    unsigned char byte = 0x00;
    unsigned char answer=0x00;
    int i=0;
    while(state!=STOP){
        read(fd, &byte, 1);
        switch (state) {
            case START:
                if(byte == FLAG) state = FLAG_RCV;
                break;
            case FLAG_RCV:
                if(byte == A_TX) state = A_RCV;
                else if(byte != FLAG) state = START;
                break;
            case A_RCV:
                if(byte == FRAME_INF_0 || byte == FRAME_INF_1 )
                {
                    state = C_RCV;
                    answer=byte;
                }
                else if(byte == FLAG) state = FLAG_RCV;
                else state = START;
                break;
            case C_RCV:
                if(byte == (A_TX ^ answer)) state = READING;
                else if(byte == FLAG) state = FLAG_RCV;
                else state = START;
                break;
            case READING:
                if(byte == FRAME_ESC) {
                    state = ESC_HANDLER;
                }else if(byte == FLAG){
                    state = STOP;
                }else{
                    packet[i++]=byte;
                }
                break;
            case ESC_HANDLER:
                if(byte == FRAME_ESC_FLAG){
                    packet[i++] = FLAG;
                    state = READING;
                }else if (byte == FRAME_ESC_ESC) {
                    packet[i++] = FRAME_ESC;
                    state = READING;
                }
                else{
                    //If after ESC byte is not ESC_FLAG or ESC_ESC, then it is an error
                    //because bytestuffing is not being used correctly
                    state = START;
                }
                break;
            default:
                break;
        }       
        //The full package was transmitted and saved in packet array
        // Now we need to check if the BCC2 is correct
        if(state == STOP){


            //Received BCC2
            unsigned char bcc2 = packet[--i];
            printf("BCC2: %x\n",bcc2);
            //Remove BCC2 from packet so that i can calculate current BCC2
            packet[i] = '\0';
            unsigned char calculated_bcc2 = packet[0];
            printf("state %d\n",state);
            //Print packet
            printf("Packet: ");
            for(int j=0;j<i;j++)
            {
                printf("%x ",packet[j]);
            }
            printf("\n");
            for(int j = 1; j < i; j++)
                calculated_bcc2 ^= packet[j];
            printf("Calculated BCC2: %x\n",calculated_bcc2);
            if(bcc2 == calculated_bcc2){
                send_supervision_frame(fd,1,&count_rx);
                count_rx = (count_rx + 1)%2;
                printf("Frame lido\n");
                return i;
            }else{
                printf("Error: Package rejected\n");
                send_supervision_frame(fd,0,&count_rx);
                return -1;
            }
        }
    }
    printf("Frame lido\n");
    
    return -1;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int fd,const char *role)
{   
    alarmEnabled = FALSE;
    unsigned char byte=0x00;
    MachineState state = START;
    LinkLayerRole rl=strcmp(role,"rx") ? LlTx : LlRx;
    if(rl==LlTx)
    {
        (void)signal(SIGALRM, alarmHandler);
        while(nRetransmissions>0 && state!=STOP){
            if(alarmEnabled == FALSE){
                printf("A mandar o DISC\n");
                write_disc(fd,rl);
                alarm(timeout);
                alarmEnabled = TRUE;
            }
            if(read_disc(fd,state)==1)
            {
                printf("Leu o DISC corretamente.\n");
                //enviar o ultimo ua.
                write_ua(fd,rl);
                printf("Ultimo UA enviado, fechando a conexão.\n");
                port_restore(fd);
                close(fd);
                return 1;
            }
        }
    }
    else if(rl==LlRx)
    {
        printf("A espera de receber o DISC\n");
        while(state!=STOP){
            read(fd, &byte, 1);
            switch (state) {
                case START:
                    if(byte == FLAG) state = FLAG_RCV;
                    break;
                case FLAG_RCV:
                    if(byte == A_TX) state = A_RCV;
                    else if(byte != FLAG) state = START;
                    break;
                case A_RCV:
                    if(byte == FRAME_DISC) state = C_RCV;
                    else if(byte == FLAG) state = FLAG_RCV;
                    else state = START;
                    break;
                case C_RCV:
                    if(byte == (A_TX ^ FRAME_DISC)) state = BCC_OK;
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
        //So manda uma vez o DISC.
        printf("A escrever o UA\n");
        write_disc(fd,rl); 
        state= START;
        printf("A espera do UA\n");
        while(state!=STOP){
            read(fd, &byte, 1);
            switch (state) {
                case START:
                    if(byte == FLAG) state = FLAG_RCV;
                    break;
                case FLAG_RCV:
                    if(byte == A_TX) state = A_RCV;
                    else if(byte != FLAG) state = START;
                    break;
                case A_RCV:
                    if(byte == FRAME_UA) state = C_RCV;
                    else if(byte == FLAG) state = FLAG_RCV;
                    else state = START;
                    break;
                case C_RCV:
                    if(byte == (A_TX ^ FRAME_UA)) state = BCC_OK;
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
        printf("UA lido, a fechar conexão\n");
        port_restore(fd);
        close(fd);
        return 1;
    }
    return 0;
}
