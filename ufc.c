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

#define RECV_BUFF_SIZE 1024 // Receive buffer size
#define SEND_BUFF_SIZE 100 // Send buffer size
#define SERV_PORT_NO 8080 // Port number
#define SERV_IP_ADDR "10.10.1.100"

int main(int argc, char *argv[])
{

  int sock; // UDP socket
  struct sockaddr_in sa;
  int bytes_sent;
  char buffer[SEND_BUFF_SIZE];
  char recbuffer[RECV_BUFF_SIZE];
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
    return(EXIT_FAILURE);
  }

  memset(&sa, 0, sizeof sa);
  sa.sin_family = AF_INET; // Adressing is in IPv4
  sa.sin_port = htons(SERV_PORT_NO); // Ensures network byte order, and sets the port no
  sa.sin_addr.s_addr = htonl(INADDR_ANY); // Converts the address to octets
  fromlen = sizeof sa;

  bytes_sent = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr*)&sa, sizeof sa);
  if (bytes_sent < 0) {
    printf("Error sending filename: %s\n", strerror(errno));
    return(EXIT_FAILURE);
  } else {
    printf("Filename sent: %s\n", buffer);
  }

  /*
    Open a new file to store incoming data
  */

  FILE *fp;
  fp = fopen("received.txt","w");
  if(fp == NULL){
    printf("Error creating file.\n");
    return(EXIT_FAILURE);
  }

  /*
    Receive the file requested
    - Keep looping through until all packets are received
    - Print the received packet data to file
    - !! exit the while loop !!
  */

  int bytes_received = recvfrom(sock, recbuffer, sizeof recbuffer, 0, (struct sockaddr*)&sa, &fromlen);
  while(bytes_received != 0){

    printf("%d\n", bytes_received);
    if(bytes_received < 0) {
      printf("Error in receiving file data.\n");
      return(EXIT_FAILURE);
    }else{
      printf("Number of bytes received: %d\n", bytes_received);
      printf("Buffer: %s\n", recbuffer);

      if(fwrite(recbuffer, 1, bytes_received, fp) < 0){
        printf("Error writing to file.\n");
        return(EXIT_FAILURE);
      }
      memset(recbuffer, 0, sizeof(recbuffer));
      bytes_received = recvfrom(sock, recbuffer, sizeof recbuffer, 0, (struct sockaddr*)&sa, &fromlen);

    }

  }

  fclose(fp);
  close(sock);
  return(EXIT_SUCCESS);
}
