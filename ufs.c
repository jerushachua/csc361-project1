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

// UDP Server

int main(void)
{
  int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in sa;
  char buffer[1024];
  ssize_t recsize;
  socklen_t fromlen;

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
    recsize = recvfrom(sock, (void*)buffer, sizeof buffer, 0, (struct sockaddr*)&sa, &fromlen);
    if (recsize < 0) {
      fprintf(stderr, "%s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    printf("datagram: %.*s\n", (int)recsize, buffer);
    
    /* find the requested datagram in the current directory */ 
    DIR *d; 
    struct dirent *dir; 
    d = opendir("."); 
    
    /* TODO: figure out how to read from the buffer!
     *       and compare this datagram to the filenames in the while loop
     *       (dir->d_name) == buffer) is not doing the right comparision
     */ 
    printf("%.*s\n", (int)recsize, buffer);  
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
            if(dir->d_name == buffer){
                printf("The requested file is in this directory yay!");
                break; 
            }
        }
    }else{
        printf("Unable to open the directory :("); 
    }
    closedir(d); 
    /* close the directory */
  }
}
