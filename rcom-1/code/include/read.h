#ifndef _READ_H_
#define _READ_H_

#include "define_frame.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

int read_set(int fd, MachineState state);

int read_ua(int fd, MachineState state);

int read_disc(int fd,MachineState state);


#endif // _READ_H_