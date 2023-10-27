#include <string.h>
#include "packet.h"
unsigned char *control_packet(int controlo,const char *filename,long int filesize, int *packet_size)
{
    unsigned char L2 = (filesize >> 8) & 0xFF;  // High byte
    unsigned char L1 = filesize & 0xFF;    
    int filename_size=strlen(filename);
    //int filesize_size = sizeof(filesize); 
    *packet_size=1+1+1+2+1+1+filename_size;
    unsigned char *packet=malloc(sizeof(unsigned char)*(*packet_size));
    // 2=Start - 3=End
    packet[0]=controlo;
    //type- Tamanho do ficheiro.
    packet[1]=0;
    packet[2]=2;
    packet[3]=L2;
    packet[4]=L1;

    packet[5]=1;
    //type- tamanho no nome do ficheiro.
    packet[6]=filename_size;
    for(int i=0;i<filename_size;i++)
    {
        packet[7+i]=filename[i];
    }
    return packet;
}
unsigned char *data_packet_maker( const unsigned char *data_to_send, long int data_size, int *packet_size) {
    // Calculate the size of the data field, represented as "L2 L1"
    /*
     * The use of bitwise shifting and masking (>> 8 and & 0xFF) is
     * a common method to isolate high and low bytes of a 16-bit value.
     * Shifting data_size right by 8 bits isolates the high byte, and
     * the & 0xFF operation ensures that only the lowest 8 bits are
     * retained, which represents the low byte.
     * */
    unsigned char L2 = (data_size >> 8) & 0xFF;  // High byte
    unsigned char L1 = data_size & 0xFF;         // Low byte

    *packet_size = 1 + 2 + data_size;  // Control field (1 byte) + L2 L1 (2 bytes) + Data field

    // Allocate memory for the packet
    unsigned char *packet = (unsigned char *)malloc(*packet_size);

    // Create the data packet
    packet[0] = 1;  // Control field value
    packet[1] = L2; // High byte of data size
    packet[2] = L1; // Low byte of data size
    printf("Data size: %ld\n",data_size);
    // Copy the data into the packet
    for(int i = 0; i < data_size; i++)
        packet[3 + i] = data_to_send[i];

    return packet;
}
unsigned char* read_control_packet(unsigned char * received_packet,int received_packet_size, int *file_size){
    // 1st element - File size
    unsigned char file_size_bytes[received_packet[2]];
    memcpy(file_size_bytes,received_packet+3,received_packet[2]);
    /*
     * The for loop iterates through the file_size_bytes array. It uses bitwise
     * left-shift and bitwise OR operations to properly combine the individual
     * bytes into a single integer value. The shift (8 * (received_packet[2] - 1 - i))
     * is used to position each byte correctly within the 4-byte integer representation.
     * */
    for(int i=0;i<received_packet[2];i++)
    {
        *file_size |= (file_size_bytes[i] << (8 * (received_packet[2] - 1 - i)));
    }
    // 2nd element - File name
    unsigned char filename_size=received_packet[6];
    unsigned char * filename= malloc(sizeof(unsigned char)*filename_size);
    memcpy(filename,received_packet+7,filename_size);
    return filename;
}
unsigned char* read_data_packet(unsigned char* received_packet, int received_packet_size){
    unsigned char *buffer = malloc(sizeof(unsigned char*) *received_packet_size);
    if(buffer)
        memcpy(buffer,received_packet+3,received_packet_size-3);

    return buffer;
}