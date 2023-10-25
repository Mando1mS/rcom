#include "packet.h"
unsigned char *control_packet(int controlo,char *filename,long int filesize,unsigned int *packet_size)
{
    int filename_size=strlen(filename);
    int filesize_size = sizeof(filesize); 
    *packet_size=1+2+filename_size+2+filesize_size;
    unsigned char *packet=malloc(sizeof(unsigned char)*(*packet_size));
    // 2=Start - 3=End
    packet[0]=controlo;
    //type- Tamanho do ficheiro.
    packet[1]=0;
    packet[2]=filesize_size;
    packet[3]=filesize;
    packet[4]=1;
    packet[5]=filename_size;
    for(int i=0;i<filename_size;i++)
    {
        packet[6+i]=filename[i];
    }
    return packet;
}

unsigned char *data_packet_maker(long int packet_number,unsigned char*data_to_send, long int data_size, unsigned int *packet_size){

}