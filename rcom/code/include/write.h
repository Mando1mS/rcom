#ifndef _WRITE_H_
#define _WRITE_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include "define_frame.h"
#include "link_layer.h"


int write_set(int fd);
int write_ua(int fd,LinkLayerRole rl);
int write_disc(int fd,LinkLayerRole rl);
unsigned char *create_packet(int fd,const unsigned char *packet,int packetSize,int *count_tx, int*j);
void send_supervision_frame(int fd,int acceptance,int *count_rx);

#endif // _WRITE_H_