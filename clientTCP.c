#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define MSG_SIZE 80
#define MAX_CLIENTS 95
#define MYPORT 9876

void exitClient(int fd, fd_set *readfds, char fd_array[], int *num_clients){
    int i;

    close(fd);
    FD_CLR(fd, readfds); //clear the leaving client from the set
    for (i = 0; i < (*num_clients) - 1; i++)
        if (fd_array[i] == fd)
            break;
    for (; i < (*num_clients) - 1; i++)
        (fd_array[i]) = (fd_array[i + 1]);
    (*num_clients)--;
}



int main(int argc, char** argv){

  int i=0;

  int port;
  int num_clients = 0;
  int server_sockfd, client_sockfd;
  struct sockaddr_in server_address;
  int addresslen = sizeof(struct sockaddr_in);
  int fd;
  char fd_array[MAX_CLIENTS];
  fd_set readfds, testfds, clientfds;
  char msg[MSG_SIZE + 1];
  char kb_msg[MSG_SIZE + 10];

  int result;
  char hostname[MSG_SIZE];
  struct hostent *hostinfo;
  struct sockaddr_in address;
  char alias[MSG_SIZE];
  int clientid;

  int sockfd = socket(AF_INET,SOCK_STREAM,0);
  if(sockfd<0){
    printf("There was an error creating the socket\n");
    return 1;
  }

  struct sockaddr_in serveraddr;
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_port=htons(9876);
  serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");

  int e = connect(sockfd,(struct sockaddr*)&serveraddr,
  sizeof(struct sockaddr_in));
  if(e<0){
    printf("There was an error with connecting\n");
    return 1;
  }


  fflush(stdout);

  FD_ZERO(&clientfds);
  FD_SET(sockfd,&clientfds);
  FD_SET(0,&clientfds);//stdin


  while (1) {
    testfds=clientfds;
    select(FD_SETSIZE,&testfds,NULL,NULL,NULL);

    for(fd=0;fd<FD_SETSIZE;fd++){
       if(FD_ISSET(fd,&testfds)){
          if(fd==sockfd){

             result = read(sockfd, msg, MSG_SIZE);
             msg[result] = '\0';
             printf("%s", msg +1);

             if (msg[0] == 'X') {
                 close(sockfd);
                 exit(0);
             }
          }
          else if(fd == 0){

             fgets(kb_msg, MSG_SIZE+1, stdin);

             if (strcmp(kb_msg, "quit\n")==0) {
                 sprintf(msg, "Client is shutting down.\n");
                 write(sockfd, msg, strlen(msg));
                 close(sockfd);
                 exit(0);
             }
             else {
                 sprintf(msg, "M%s", kb_msg);
                 write(sockfd, msg, strlen(msg));
             }
          }
       }
     }
   }

  return 0;
}
