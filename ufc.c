#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

/* UDP Client */

#define RECV_BUFF_SIZE 100 // Receive buffer size
#define SEND_BUFF_SIZE 1024 // Send buffer size
#define SERV_PORT_NO 8080 // Port number
#define SERV_IP_ADDR "10.10.1.100"

int main(int argc, char *argv[])
{

  int sock; // UDP socket
  struct sockaddr_in sa;
  int bytes_sent;
  char buffer[200];
  char recbuffer[1024];
  ssize_t recfile;
  socklen_t fromlen;

  /*
    Checks that a filename is given
  */

  if (argc == 2) strcpy(buffer, argv[1]);
  else{
    printf("No filename given. Unable to continue. \n");
    return(EXIT_FAILURE);
  }

  /*
    Initialization of UDP socket
  */

  sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (-1 == sock) {
      printf("Error Creating Socket");
      exit(EXIT_FAILURE);
    }

  memset(&sa, 0, sizeof sa);
  sa.sin_family = AF_INET; // Adressing is in IPv4
  sa.sin_addr.s_addr = htonl(INADDR_ANY); // Converts the address to octets
  sa.sin_port = htons(SERV_PORT_NO); // Ensures network byte order, and sets the port no
  bytes_sent = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&sa, sizeof sa);
  if (bytes_sent < 0) {
    printf("Error sending filename: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }else{
    printf("Filename sent: %s\n", buffer);
  }

  /*
    Receive the file requested
  */

  FILE *fp;
  fp = fopen("received.txt","w");
  if(fp == NULL){
    printf("Error creating file.\n");
    return(EXIT_FAILURE);
  }

  /*
    Keep looping through until all packets are received
    Print the received packet data to file
  */

  while((bytes_received = recvfrom(sock, (void*)recbuffer, sizeof buffer, 0, (struct sockaddr*)&sa, &fromlen))) > 0){
    printf("Number of bytes received: %d\n". bytes_received);
    printf("Buffer: %s\n", recbuffer);

    if(fwrite(recbuffer, 1, bytes_received, fp) < 0){
      printf("Error writing to file.\n");
      return(EXIT_FAILURE);
    }
    memset(recvBuff, '0', sizeof(recvBuff));
  }

  if(bytes_received < 0) {
    printf("Error in receiving file data.\n");
    return(EXIT_FAILURE);
  }

  fclose(fp);
  close(sock);
  return(EXIT_SUCCESS);
}
