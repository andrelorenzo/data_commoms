#include "stdio.h"
#define ETHCOMMS_IMP
#include "eth_comms.h"
commh_t commh;

void print_rec(uint8_t *msg, size_t len, const char *ip, uint16_t port, uint16_t cid){
    UNUSED(cid);

    printf("Received %zu bytes from %s:%u (%i)\n", len, ip, port, cid)  ;
    printf("\t Message: 0x");
    for(size_t i = 0; i < len; i++){
        printf("%c", msg[i]);
    }
    printf("\n");
}

int main(int argc, char ** argv) {
    int type = 0;
    if(argc != 2){
        printf("Usage: %s [type]\n",argv[0]);
        printf(" > udp\n");
        printf(" > tcpclient\n");
        printf(" > tcpserver\n");
        return -1;
    }
    char msg[256];
    int n = snprintf(msg, sizeof msg, "HOLA DESDE EL ORDENADOR");
    CommsLogSetVerbosity(COMMS_DEBUG);

    if(strcmp(argv[1], "udp") == 0){
        CommsUDPInit(&commh, .local_port = 5001, .recvcb = print_rec);
    }else if(strcmp(argv[1], "tcpclient") == 0){
        type = 1;
        CommsTCPClientInit(&commh, .local_port = 5001, .recvcb = print_rec);
    }else if(strcmp(argv[1], "tcpserver") == 0){
        type = 2;
        CommsTCPServerInit(&commh, .local_port = 5002, .recvcb = print_rec);
    }else{
        printf("Usage: %s [type]\n",argv[0]);
        printf(" > udp\n");
        printf(" > tcpclient\n");
        printf(" > tcpserver\n");
        return -1;
    }

    if(type == 1){
        while(!CommsConnect(&commh, "127.0.0.1", 5002)){}
    }else if (type == 0){
        CommsConnect(&commh, "127.0.0.1", 5002);
    }

    while(true){
        if(type == 2 && commh.client_count > 1){
            if(!CommsSend(&commh, (uint8_t*)msg, (size_t)n, .cid = COMMS_SEND_TO_ALL_CLIENTS)) {
                CommsExit(&commh);
            }
        }else if(type != 2){
            if(!CommsSend(&commh, (uint8_t*)msg, (size_t)n,0)) {
                CommsExit(&commh);
            }
        }

        int ret = getc(stdin);
        if(ret == 101){
            break; // "e"
        }else if (ret == 99){
            CommsPrintClients(&commh, stdout);
        }
    }
    

    CommsClose(&commh);
    return 0;
}