// Application layer protocol implementation

#include "application_layer.h"

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    //Comum para ambos.
    LinkLayer connectionParameters;
    strcpy(connectionParameters.serialPort,serialPort);
    connectionParameters.role= strcmp(role,"rx") ? LlTx : LlRx;
    connectionParameters.baudRate = baudRate;
    connectionParameters.nRetransmissions = nTries;
    connectionParameters.timeout = timeout;
    int fd = llopen(connectionParameters);

    if(fd == 0){
        perror("Error establishing port connection.");
        exit(-1);
    } else{
        printf("Port connection established.\n");
    }

        if(connectionParameters.role==LlTx)
        {
            printf("Executando o llwrite()\n");

            unsigned char *source=NULL;
            long int buffersize=0;

            FILE *fp=fopen(filename,"r");
            if(fp==NULL)
            {
                printf("Error opening file\n");
                exit(-1);
            }
            if(fseek(fp,0L,SEEK_END)==FALSE)
            {
                buffersize=ftell(fp);
                if(buffersize==-1){
                    ferror(fp);
                    exit(-1);
                }
                source=malloc(sizeof(unsigned char)*(buffersize+1));

                if(fseek(fp,0L,SEEK_SET)== TRUE) {
                    ferror(fp);
                    exit(-1);
                }

                size_t newLength = fread(source,sizeof(unsigned char),buffersize,fp);
                if(ferror(fp)!=0)
                {
                    fputs("Error handling the buffer!",stderr);
                    exit(-1);
                } //Not really sure if else is needed here. Check when testing.
                else{ source[newLength+1]='\0';}
            }

            fclose(fp);

            int packet_size;
            unsigned char *packet_start= control_packet(2,filename,buffersize,&packet_size);
            if(llwrite(fd,packet_start,packet_size)!=1)
            {
                free(packet_start);
                perror("Llwrite failed for packet start.\n");
                exit(-1);
            }
            free(packet_start);

            long int remaining_bytes=buffersize;
            long int packet_number=0;

            while(remaining_bytes>0){
                long int data_size = remaining_bytes > MAX_PACKET_SIZE ? MAX_PACKET_SIZE : remaining_bytes;

                unsigned char* data_to_send = malloc(sizeof(unsigned char)*(data_size));
                memcpy(data_to_send, source, data_size);
                unsigned char* data_packet = data_packet_maker(data_to_send, data_size, &packet_size);
                free(data_to_send);
                if(llwrite(fd,data_packet,packet_size)!=1)
                {
                    free(data_packet);
                    perror("Llwrite failed for packet data.\n");
                    exit(-1);
                }
                free(data_packet);
                remaining_bytes-=data_size;
                packet_number= (packet_number+1)%255;
                source+=data_size;
            }

            unsigned char *packet_end= control_packet(3,filename,buffersize,&packet_size);
            if(llwrite(fd,packet_end,packet_size)!=1)
            {
                free(packet_end);
                perror("Llwrite failed for packet start.\n");
                exit(-1);
            }
            free(packet_end);
            free(source);
            printf("llwrite() executado\n");
            llclose(fd,role);

        }else if(connectionParameters.role==LlRx)
        {
            printf("Executando o llread()\n");
            unsigned char *received_packet= malloc(sizeof(unsigned char)*MAX_PACKET_SIZE);
            int received_packet_size=0;
            int file_size=0;
            // Tratar o packet de controlo
            while((received_packet_size=llread(fd,received_packet))==-1);
            if(received_packet[0]!=2)
            {
                printf("Error receiving control packet.\n");
                exit(-1);
            }
            unsigned char* file_name=read_control_packet(received_packet,received_packet_size, &file_size);
            llread(fd,received_packet);
            free(received_packet);
            llclose(fd,role);
            //TODO
        }
}