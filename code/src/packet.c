#include <string.h>
#include "packet.h"
unsigned char *control_packet(int controlo,const char *filename,long int filesize, int *packet_size)
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

    // Copy the data into the packet
    for(int i = 0; i < data_size; i++)
        packet[3 + i] = data_to_send[i];

    return packet;
}