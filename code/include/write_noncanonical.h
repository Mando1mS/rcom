#ifndef _WRITE_NONCANONICAL_H_
#define _WRITE_NONCANONICAL_H_



#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <link_layer.h>
#include <alarm.h>


#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source



#define FALSE 0
#define TRUE 1

#define BUF_SIZE 5  
#define SET 0x03
#define FLAG 0x7E
#define ADDR 0x03

int write_noncanonical(LinkLayer dados);

#endif // WRITE_NONCANONICAL.H