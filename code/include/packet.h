#ifndef _PACKET_H_
#define _PACKET_H_


#include "define_frame.h"
#include <stdio.h>
#include <stdlib.h>

unsigned char *control_packet(int controlo,char *filename,long int filesize,unsigned int *packet_size);
unsigned char *data_packet_maker(long int packet_number,unsigned char*data_to_send, long int data_size, unsigned int *packet_size);
#endif