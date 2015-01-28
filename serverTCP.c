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



int main(int argc, char **argv){
  int i=0;

 int port;
 int num_clients = 0;
 int server_sockfd, clientfd;
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


  printf("\n*** Server program starting (enter \"quit\" to stop): \n");
    fflush(stdout);

    /* Create and name a socket for the server */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9876);
    bind(server_sockfd, (struct sockaddr *)&server_address, addresslen);

    /* Create a connection queue and initialize a file descriptor set */
    listen(server_sockfd, 1);
    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);
    FD_SET(0, &readfds);


  while (1) {
        testfds = readfds;
        select(FD_SETSIZE, &testfds, NULL, NULL, NULL);

        for (fd = 0; fd < FD_SETSIZE; fd++) {
           if (FD_ISSET(fd, &testfds)) {

              if (fd == server_sockfd) {
                 clientfd = accept(server_sockfd, NULL, NULL);


                    FD_SET(clientfd, &readfds);
                    fd_array[num_clients]=clientfd;
                    /*Client ID*/
                    printf("Client %d joined\n",num_clients++);
                    fflush(stdout);

                    sprintf(msg,"%d: ",clientfd);

                    send(clientfd,msg,strlen(msg),0);


              }
              else if (fd == 0)  {  /* Process keyboard activity */
                 fgets(kb_msg, MSG_SIZE + 1, stdin);

                 if (strcmp(kb_msg, "quit\n")==0) {
                    sprintf(msg, "Server shutting down.\n");
                    for (i = 0; i < num_clients ; i++) {
                       write(fd_array[i], msg, strlen(msg));
                       close(fd_array[i]);
                    }
                    close(server_sockfd);
                    exit(0);
                 }
                 else {
                    //printf("server - send\n");
                    sprintf(msg, "M%s: ", kb_msg);
                    for (i = 0; i < num_clients ; i++)
                       write(fd_array[i], msg, strlen(msg));
                 }
              }
              else if(fd) {

                 result = read(fd, msg, MSG_SIZE);

                 if(result==-1) perror("read()");
                 else if(result>0){

                    sprintf(kb_msg,"M%2d: ",fd);
                    msg[result]='\0';

                    strcat(kb_msg,msg+1);

                    for(i=0;i<num_clients;i++){
                       if (fd_array[i] != fd)
                          write(fd_array[i],kb_msg,strlen(kb_msg));
                    }

                    printf("%s",kb_msg+1);

                    if(msg[0] == 'X'){
                       exitClient(fd,&readfds, fd_array,&num_clients);
                    }
                 }
              }
              else {
                 exitClient(fd,&readfds, fd_array,&num_clients);
              }
           }
        }
     }

  return 0;
}
