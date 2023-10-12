#include "write_noncanonical.h"

volatile int STOP = FALSE;

int write_noncanonical(LinkLayer connectionParameters)
{   
    // Program usage: Uses either COM1 or COM2
    const char *serialPortName = connectionParameters.serialPort;
    // Open serial port device for reading and writing, and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(serialPortName, O_RDWR | O_NOCTTY);

    if (fd < 0)
    {
        perror(serialPortName);
        exit(-1);
    }

    struct termios oldtio;
    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; // Inter-character timer unused
    newtio.c_cc[VMIN] = 5;  // Blocking read until 5 chars received

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");
    (void)signal(SIGALRM, alarmHandler);

    

    while (alarmCount < 4)
    {
        if (alarmEnabled == FALSE)
        {
            alarm(4); // Set alarm to be triggered in 3s
            alarmEnabled = TRUE;
        }
        // Create string to send
    unsigned char buf[BUF_SIZE] = {0};
    
    buf[0]= FLAG;
    buf[1]= ADDR;
    buf[2]= SET;
    buf[3]= buf[1]^buf[2];
    buf[4]= FLAG;
    // In non-canonical mode, '\n' does not end the writing.
    // Test this condition by placing a '\n' in the middle of the buffer.
    // The whole buffer must be sent even with the '\n'.


    int bytes = write(fd, buf, BUF_SIZE);
    printf("%d bytes written\n", bytes);

    // Wait until all bytes have been written to the serial port
    sleep(1);
    
    // Loop for input
    unsigned char buf2[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
    int bytes = read(fd, buf2, BUF_SIZE);
               
    if(buf2[3]==(buf2[1]^buf2[2]) ){
        buf2[bytes] = '\0'; // Set end of string to '\0', so we can printf
        printf("Message received!\n");
        alarm(0);
        for(int i=0; i<BUF_SIZE;i++){       
            printf(":%02X:%d\n", buf2[i], bytes);
        }
        }
        sleep(1);
        if (buf2[0] == FLAG)
            alarmCount=5;
    }
    // Restore the old port settings
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }
    close(fd);

    return 1;
}