#include "write.h"

int write_set(int fd){
    printf("write_set\n");
    unsigned char buf[5] = {FLAG, A_TX, FRAME_SET, (A_TX^FRAME_SET), FLAG};
    int bytes = write(fd, buf, 5);
    return bytes;
}

int write_ua(int fd,LinkLayerRole rl)
{
    printf("write_ua\n");
    if(rl==LlRx)
    {
        unsigned char buf[5] = {FLAG, A_RX, FRAME_UA, (A_RX^FRAME_UA), FLAG};
        int bytes = write(fd, buf, 5);
        return bytes;
    }
    else{
        unsigned char buf[5] = {FLAG, A_TX, FRAME_UA, (A_TX^FRAME_UA), FLAG};
        int bytes = write(fd, buf, 5);
        return bytes;
    }
}
int write_disc(int fd,LinkLayerRole rl)
{
    printf("write_disc\n");
    if(rl==LlTx)
    {
        unsigned char buf[5] = {FLAG, A_TX, FRAME_DISC, (A_TX^FRAME_DISC), FLAG};
        int bytes = write(fd, buf, 5);
        return bytes;
    }
    else{
        unsigned char buf[5] = {FLAG, A_RX, FRAME_DISC, (A_RX^FRAME_DISC), FLAG};
        int bytes = write(fd, buf, 5);
        return bytes;
    }
    
}
unsigned char *create_packet(int fd,const unsigned char *packet,int packetSize,int *count_tx, int*j)
{
    int framesize=6+packetSize;
    unsigned char* frame= malloc(sizeof(unsigned char)* (framesize));

    frame[0]=FLAG;
    frame[1]=A_TX;
    if(*count_tx==0){
        frame[2]=FRAME_INF_1;
    }else
    {
        frame[2]=FRAME_INF_0;
    }
    frame[3]=frame[2]^frame[1];

    memcpy(frame+4,packet,packetSize);

    //Falta byte stuffing aqui
    *j=4;
    for(int i=0;i<packetSize;i++)
    {
        if(packet[i]==FLAG)
        {
            frame=realloc(frame,++framesize);
            frame[(*j)++]= FRAME_ESC;
            frame[(*j)++]= FRAME_ESC_FLAG;
        }
        else if(packet[i]==FRAME_ESC)
        {
            frame=realloc(frame,++framesize);
            frame[(*j)++]= FRAME_ESC;
            frame[(*j)++]= FRAME_ESC_ESC;
        }
        else{
            frame[(*j)++]=packet[i];
        }
    }

    unsigned char bcc2=0x00;
    for(int i=0;i<packetSize;i++)
    {
        bcc2 ^= packet[i];
    }     
    frame[(*j)++]=bcc2;
    frame[(*j)++]=FLAG;
    return frame;
}
void send_supervision_frame(int fd,int acceptance, int *count_rx){
    printf("Writing supervision frame\n");
    unsigned char A=0x00;
    if(acceptance==TRUE)
    {
        if(*count_rx==0)
        {
            A=FRAME_RR_1;
        }
        else
        {
            A=FRAME_RR_0;
        }
    }
    else
    {
        if(*count_rx==0)
        {
            A=FRAME_REJ_0;
        }
        else
        {
            A=FRAME_REJ_1;
        }
    }
    unsigned char buf[5] = {FLAG, A_RX, A, (A_RX^A), FLAG};
    write(fd, buf, 5);
    printf("Supervision frame written\n");
}
