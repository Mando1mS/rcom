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


int write_set(int fd);
int write_ua(int fd);
unsigned char *create_packet(int fd,unsigned char *packet,int packetSize,int *count_tx);
#endif // _WRITE_H_