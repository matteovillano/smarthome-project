#include <stdio.h>    
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#include <arpa/inet.h>


#define PORT 8080 
#define SA struct sockaddr


int websockinit();
int sockaccept(int sockfd);
