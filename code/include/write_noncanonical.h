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


#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source



#define FALSE 0
#define TRUE 1


bool write_noncanonical(LinkLayer connectionParameters);

#endif // WRITE_NONCANONICAL.H