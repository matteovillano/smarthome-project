#include "sock.h"


//////////////socket init//////////////////
int websockinit(){
	
	int sockfd/*,*/ /*connfd,*//* len*/; 
    struct sockaddr_in servaddr/*, cli*/; 
  
    // socket create 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        return -1; 
    } 
	
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP and PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr =htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding  
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        return -1; 
    }
    
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        return -1; 
    } 
    
    printf("Web server is now online...\n"); 
	return sockfd;
}

int sockaccept(int sockfd){
	int connfd;
	struct sockaddr_in cli;
	int len;
	 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        return -1; 
    } 
	
	return connfd; 
}
///////////////////////////////////////////
