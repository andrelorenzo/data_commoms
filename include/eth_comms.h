#ifndef ETH_COMMS_H_
#define ETH_COMMS_H_

#include <arpa/inet.h>
#include "stdlib.h"
#include <unistd.h>
#include "string.h"


#define MAX_SERVER_CLIENTS 256

typedef enum{
    COMMS_SERVER = 0U,
    COMMS_CLIENT
}arq_type_e;

typedef enum{
    COMMS_UDP = 0U,
    COMMS_TCP
}proto_type_e;

typedef enum{
    COMMS_V4 = 0U,
    COMMS_V6
}ip_type_e;

typedef enum{
    COMMS_ERROR_BAD_PROTO = 0U,
    COMMS_ERROR_SOCKET,
    COMMS_ERROR_BIND,
    COMMS_ERROR_CONNECT,
    COMMS_ERROR_SEND,
    COMMS_INVALID_CONN_ARQ,
    COMMS_ERROR_LISTEN,
    COMMS_INVALID_CONN_PROTO,
    COMMS_ERROR_CLIENT_OVERFLOW,
    COMMS_ERROR_CLOSE,
    COMMS_ERROR_UNKNOWN_CLIENT
}comms_error_e;

typedef struct {
    int socketh;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr[MAX_SERVER_CLIENTS];
    proto_type_e proto;
    arq_type_e arq;
    ip_type_e ip_type;

    int clienth_socket_list[MAX_SERVER_CLIENTS];
    uint16_t clienth_socket_count;

    comms_error_e error;
}eth_handler_t;

typedef struct{
    const char * localip;
    uint16_t localport;
    const char * remoteip;
    uint16_t remoteport;
}peer_t;

bool CommsInit(eth_handler_t * commh, peer_t * peer);
bool CommsDeinit(eth_handler_t * commh); // close the socket
int CommsSend(eth_handler_t * commh, uint8_t * msg, size_t len);

// TCP Server
bool CommsTCPDisconnect(eth_handler_t * commh, const char * client_addr); // disconnect a especific client
bool CommsTCPListen(eth_handler_t * commh);  // SERVER: maybe i can remove it

// TCP Client
bool CommsTCPConnect(eth_handler_t * commh); // CLIENT: specify ips, ports


#define ETH_COMMS_IMP 
#ifdef ETH_COMMS_IMP

#if defined(_WIN32)         // WINDOWS

#elif defined(__unix__)     // LINUX

#else                       // STM32

#endif




// def : send to 5000 ,recv on 6000
bool CommsInit(eth_handler_t * commh, peer_t * peer){

    int type = 0;
    switch(commh->proto){
        case COMMS_UDP:
            type = SOCK_DGRAM;
        break;
        case COMMS_TCP:
            type = SOCK_STREAM;
        break;
        default: 
            commh->error = COMMS_ERROR_BAD_PROTO;
        return false;
    }

    int domain = AF_INET;
    if(commh->ip_type == COMMS_V6){
        domain = AF_INET6;
    }

    commh->socketh = socket(domain, type, 0);
    if(commh->socketh == -1){
        commh->error = COMMS_ERROR_SOCKET;
        return false;
    }
    commh->serveraddr.sin_family = domain;
    commh->clientaddr[0].sin_family = domain;

    if(peer == NULL)return true;

    if(commh->arq == COMMS_SERVER){
        if(peer->remoteip != NULL){
            commh->clientaddr[0].sin_port = htons(peer->remoteport);
            commh->clientaddr[0].sin_addr.s_addr = inet_addr(peer->remoteip);
        }else{
            commh->clientaddr[0].sin_port = htons(5000);
        }

        if(peer->localip != NULL){
            commh->serveraddr.sin_port = htons(peer->localport);
            commh->serveraddr.sin_addr.s_addr = inet_addr(peer->localip);
        }else{
            commh->serveraddr.sin_port = htons(6000);
            commh->serveraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
        }

        if(bind(commh->socketh, (struct sockaddr*)&commh->serveraddr, sizeof(commh->serveraddr)) != 0){
            commh->error = COMMS_ERROR_BIND;
            return false;
        }

    }else{
        if(peer->remoteip != NULL){
            commh->serveraddr.sin_port = htons(peer->remoteport);
            commh->serveraddr.sin_addr.s_addr = inet_addr(peer->remoteip);
        }else{
            commh->serveraddr.sin_port = htons(5000);
        }

        if(peer->localip != NULL){
            commh->clientaddr[0].sin_port = htons(peer->localport);
            commh->clientaddr[0].sin_addr.s_addr = inet_addr(peer->localip);
        }else{
            commh->clientaddr[0].sin_port = htons(6000);
            commh->clientaddr[0].sin_addr.s_addr = inet_addr("0.0.0.0");
        }
    }
    return true;
}


int CommsSend(eth_handler_t * commh, uint8_t * msg, size_t len){
    if(commh->arq == COMMS_CLIENT && commh->proto == COMMS_UDP){
        if(connect(commh->socketh, (struct sockaddr *)&commh->serveraddr, sizeof(commh->serveraddr)) != 0){
            commh->error = COMMS_ERROR_CONNECT;
        }
    }

// TODO: test if for server we need to do the sendto, because the lack of connect, etc...
    ssize_t nbytes = send(commh->socketh,(void*)msg, (size_t)len,0);
    if(nbytes < 0){
        commh->error = COMMS_ERROR_SEND;
    }

    return nbytes;
}


bool CommsTCPConnect(eth_handler_t * commh){
    if(commh->arq == COMMS_SERVER){
        commh->error = COMMS_INVALID_CONN_ARQ;
        return false;
    }


    if(connect(commh->socketh, (struct sockaddr *)&commh->serveraddr, sizeof(commh->serveraddr)) != 0){
        commh->error = COMMS_ERROR_CONNECT;
        return false;
    }
    return true;
}

bool CommsTCPListen(eth_handler_t * commh){
    if(commh->proto == COMMS_UDP){
        commh->error = COMMS_INVALID_CONN_PROTO;
        return false;
    }

    if(listen(commh->socketh, 20) != 0){
        commh->error = COMMS_ERROR_LISTEN;
        return false;
    }

    socklen_t len = sizeof(commh->clientaddr[commh->clienth_socket_count++]);
    int csocket = accept(commh->socketh, (struct sockaddr *)&commh->clientaddr[commh->clienth_socket_count], &len);
    if(csocket < 0){
        commh->clienth_socket_count--;
        commh->error = COMMS_ERROR_SOCKET;
        return false;
    }
    if(commh->clienth_socket_count == MAX_SERVER_CLIENTS){
        commh->clienth_socket_count--;
        commh->error = COMMS_ERROR_CLIENT_OVERFLOW;
        return false;
    }
    commh->clienth_socket_list[commh->clienth_socket_count] = csocket;
    return true;
}


bool CommsTCPDisconnect(eth_handler_t * commh, const char * client_addr){

    in_addr_t addr =  inet_addr(client_addr);
    
    for(uint16_t i = 0; i < commh->clienth_socket_count; ++i){
        if(addr == commh->clientaddr[i].sin_addr.s_addr){
            if(close(commh->clienth_socket_list[i]) == -1){
                commh->error = COMMS_ERROR_CLOSE;
                return false;
            }else{
                return true;
            }
        }
    }
    commh->error = COMMS_ERROR_UNKNOWN_CLIENT;
    return false;
}

bool CommsDeinit(eth_handler_t * commh){
    if(close(commh->socketh) == -1){
        commh->error = COMMS_ERROR_CLOSE;
        return false;
    }
    memset(commh,0,sizeof commh);
    return true;
}



#endif

#endif // ETH_COMMS_H_

