#ifndef _DEFINE_FRAME_H_
#define _DEFINE_FRAME_H_

//MISC
#define FALSE 0
#define TRUE 1
//COMMOMN
#define FLAG 0x7E
#define A_TX 0x03
#define A_RX 0x01
//TRAMA INF
#define FRAME_INF_0 0x00
#define FRAME_INF_1 0x40
//TRAMA SUP E U
#define FRAME_SET 0x03
#define FRAME_UA 0x07
#define FRAME_RR_0 0x05
#define FRAME_RR_1 0x85
#define FRAME_REJ_0 0x01
#define FRAME_REJ_1 0x81
#define FRAME_DISC 0x0B
//STUFFING
#define FRAME_ESC 0x7D
#define FRAME_ESC_FLAG 0x5E
#define FRAME_ESC_ESC 0X5D
// Packet size
#define MAX_PACKET_SIZE 1000
typedef enum
{
    START,
    FLAG_RCV,
    OTHER_RCV,
    A_RCV,
    C_RCV,
    BCC,
    BCC_OK,
    STOP,
    READING,
    ESC_HANDLER
} MachineState;


#endif // _DEFINE_H_