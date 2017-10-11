#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>

/* UDP Server */

#define RECV_BUFF_SIZE 100 // Receive buffer size
#define SEND_BUFF_SIZE 1024 // Send buffer size
#define PORT_NO 8080 // Port number


int main(int argc, char *argv[])
{

  /*
    Setup of intial variables
  */

  int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP Socket
  struct sockaddr_in sa; // Socket address
  char rbuffer[RECV_BUFF_SIZE]; // Receiving buffer
  char sbuffer[SEND_BUFF_SIZE]; // Sending buffer
  memset(sbuffer, 0, sizeof(sbuffer));
  memset(rbuffer, 0, sizeof(rbuffer));
  ssize_t recsize;
  socklen_t fromlen;
  char dir_name[128]; // File directory name
  char filename[100]; // Filename
  int file_found_flag = -1;
  int bytes_sent;

  /*
    Initialization of UDP socket
  */

  memset(&sa, 0, sizeof sa);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  sa.sin_port = htons(PORT_NO);
  fromlen = sizeof(sa);

  if (-1 == bind(sock, (struct sockaddr *)&sa, sizeof sa)) {
    perror("error bind failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  /*
    Start file transfer
  */

  for (;;) {

    recsize = recvfrom(sock, (void*)rbuffer, sizeof rbuffer, 0, (struct sockaddr*)&sa, &fromlen);
    if (recsize < 0) {
      fprintf(stderr, "%s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    /*
      Open the requested directory
    */

    DIR *d;
    struct dirent *dir;
    if(argc == 2) d = opendir(argv[1]);
    else d = opendir(".");

    memset(filename, 0, sizeof(filename));
    printf("Requested filename: %.*s\n", (int)recsize, rbuffer);
    sprintf(filename, "%s", rbuffer);
    printf("%s\n", filename);

    if (d) {
      printf("Sucessfully opened directory: %s\n", argv[1]);
      while ((dir = readdir(d)) != NULL) {

        file_found_flag = strcmp(dir->d_name, filename);
        if(file_found_flag == 0){
          printf("We found your requested file yay!\n");
          memset(filename, 0, sizeof(filename));
          sprintf(filename, "%s/%s", argv[1], rbuffer);
          break;
        }
      }
    }else{
      printf("%s\n", "Cannot open directory :(\n\0");
    }

    /*
      Retrieve the requested file
    */

    if(file_found_flag == 0){

      FILE *input;
      input = fopen(filename, "r");
      if(input == NULL){
        printf("Cannot open requested file.\n");
        int err = sendto(sock, sbuffer, strlen(sbuffer), 0, (struct sockaddr*)&sa, sizeof sa);
        if (err < 0) perror("Problem sending error msg.\n");
        return(EXIT_FAILURE);
      }

      /*
        Send the requested file
      */
      
      if (bytes_sent < 0) printf("Error in sending file to client!!!!!!!.\n");
      while(!feof(input)){

        memset(sbuffer, 0, sizeof(sbuffer));
        int nread = fread(sbuffer, 1, SEND_BUFF_SIZE, input);
        printf("%s", sbuffer);
        bytes_sent = sendto(sock, sbuffer, strlen(sbuffer), 0, (struct sockaddr*)&sa, sizeof sa);
        if (bytes_sent < 0){
          printf("Error in sending file to client.\n");
          return(EXIT_FAILURE);
        }

      }
      fclose(input);

    }else{

      strcpy(sbuffer, "File not found :(\n\0");
      bytes_sent = sendto(sock, sbuffer, strlen(sbuffer), 0, (struct sockaddr*)&sa, sizeof sa);
      if(bytes_sent < 0) printf("Error in sending error msg back to client.\n");
      return(EXIT_FAILURE);
    }
    closedir(d);
  }

  close(sock);
  return(EXIT_SUCCESS);
}
