// Application layer protocol implementation

#include "application_layer.h"

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    //Comum para ambos.
    LinkLayer connectionParameters;
    strcpy(connectionParameters.serialPort,serialPort);
    connectionParameters.role= strcmp(role,"rx") ? LlTx : LlRx;
    connectionParameters.baudRate = baudRate;
    connectionParameters.nRetransmissions = nTries;
    connectionParameters.timeout = timeout;
    int pass_open = llopen(connectionParameters);

    if(pass_open == 0){
        perror("Error establishing port connection.");
        exit(-1);
    } else{
        printf("Port connection established.\n");
    }

        if(connectionParameters.role==LlTx)
        {
            printf("Executando o llwrite()\n");

            unsigned char *source=NULL;
            int buffersize=0;

            FILE *fp=fopen(filename,"rb");
            if(fp==NULL)
            {
                printf("Error opening file\n");
                exit(-1);
            }
            if(fseek(fp,0L,SEEK_END)==FALSE)
            {
                buffersize=ftell(fp);
                if(buffersize==-1){
                    ferror(fp);
                    exit(-1);
                }

                source=malloc(sizeof(unsigned char)*(buffersize+1));

                if(fseek(fp,0L,SEEK_SET)== TRUE) {
                    ferror(fp);
                    exit(-1);
                }

                size_t newLength = fread(source,sizeof(unsigned char),buffersize,fp);
                if(ferror(fp)!=0)
                {
                    fputs("Error handling the buffer!",stderr);
                    exit(-1);
                }
                else{ source[newLength+1]='\0';}
            }
            fclose(fp);

            //Print the buffer
            int i=0;
            while(source[i]!='\0')
            {
                printf("Byte: %d \n",source[i]);
                i++;
            }

            llwrite(source,buffersize);
            free(source);
            printf("llwrite() executado\n");
        }else if(connectionParameters.role==LlRx)
        {
            printf("Executando o llread()\n");
        }

}
