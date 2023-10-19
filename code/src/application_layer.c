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
    if(pass_open==TRUE)
    {
        if(connectionParameters.role==LlTx)
            {
                printf("Executando o llwrite()\n");
                unsigned char *source=NULL;
                int buffersize=0;
                FILE *fp=fopen(filename,"r");
                if(fp!=NULL)
                    {   
                        if(fseek(fp,0L,SEEK_END)==FALSE)
                            {
                                buffersize=ftell(fp);
                                if(buffersize==-1)
                                    ferror(fp);
                                source=malloc(sizeof(unsigned char)*(buffersize+1));
                                if(fseek(fp,0L,SEEK_SET)!=0)
                                    ferror(fp);
                                size_t newLength = fread(source,sizeof(unsigned char),buffersize,fp);
                                if(ferror(fp)!=0)
                                {
                                    fputs("Error handling the buffer!",stderr);
                                }
                            else{ source[newLength+1]='\0';}
                            }
                    fclose(fp);
                    }
                int i=0;
                while(source[i]!='\0')
                {
                    printf("Byte: %d \n",source[i]);
                    i++;
                 }
            llwrite(source,buffersize);
            free(source);
        }
        if(connectionParameters.role==LlRx)
        {
            printf("Executando o llread()\n");
        }
    }
    else{
        printf("Error found while opening the port connection\n");
    }
    
    
    
    
    //llwrite(Buffer,buffersize);
    
}
