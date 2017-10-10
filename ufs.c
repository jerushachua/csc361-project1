#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> /* for close() for socket */
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>

/* UDP Server */


int main(int argc, char *argv[])
{

  char dir_name[128];
  int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in sa;
  char buffer[1024];
  ssize_t recsize;
  socklen_t fromlen;
  char filename[1024];
  int file_found_flag = -1;
  int bytes_sent;
  char file_contents[1024];

  memset(&sa, 0, sizeof sa);
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  sa.sin_port = htons(8080);
  fromlen = sizeof(sa);

  if (-1 == bind(sock, (struct sockaddr *)&sa, sizeof sa)) {
    perror("error bind failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  for (;;) {
    memset(&buffer[0], 0, sizeof(buffer));
    recsize = recvfrom(sock, (void*)buffer, sizeof buffer, 0, (struct sockaddr*)&sa, &fromlen);
    if (recsize < 0) {
      fprintf(stderr, "%s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    DIR *d;
    struct dirent *dir;
    if(argc == 2) d = opendir(argv[1]);
    else d = opendir(".");

    sprintf(filename, "%s", buffer);

    if (d) {
      printf("Sucessfully opened directory: %s\n", argv[1]);
      while ((dir = readdir(d)) != NULL) {

        file_found_flag = strcmp(dir->d_name, filename);
        if(file_found_flag == 0){
          printf("We found your requested file yay!\n");
          break;
        }
      }
      if(file_found_flag != 0) printf("Not found :(\n");
    }else{
      printf("%s\n", "Cannot open directory :(\n\0");
    }
    closedir(d);
  }

    /* TODO: if file is found, read the contents and send it back to client
     *       file_contents is only size 1024
     *
     *
     *
     *       client: cannot exit loop once packet is sent. close socket on server side?
     */
  if(file_found_flag == 0){

    FILE *input;
    input = fopen(filename, "r");
    if(input == NULL) fprintf(stderr, "%s\n", strerror(errno));

    while(fgets(file_contents, 1024, input)){

      strcpy(file_contents, buffer);
      printf("%s\n", file_contents);
      bytes_sent = sendto(sock, file_contents, strlen(file_contents), 0, (struct sockaddr*)&sa, sizeof sa);
      if (bytes_sent < 0) printf("Error in sending msg back to client.\n");
    }
    fclose(input);
  }else{
    strcpy(file_contents, "Not found :(\n\0");
    bytes_sent = sendto(sock, file_contents, strlen(file_contents), 0, (struct sockaddr*)&sa, sizeof sa);
    if(bytes_sent < 0) printf("Error in sending error msg back to client.\n");
    close(sock);

  }
}
