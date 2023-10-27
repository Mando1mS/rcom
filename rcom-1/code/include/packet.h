#ifndef _PACKET_H_
#define _PACKET_H_


#include "define_frame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char *control_packet(int controlo,const char *filename,long int filesize, int *packet_size);
unsigned char *data_packet_maker(const unsigned char*data_to_send, long int data_size,  int *packet_size);
unsigned char* read_control_packet(unsigned char * received_packet,int received_packet_size, int *file_size);
unsigned char* read_data_packet(unsigned char* received_packet, int received_packet_size);

#endif