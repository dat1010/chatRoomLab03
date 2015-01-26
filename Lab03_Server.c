#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
//

int main(int argc, char **argv){
  int sockfd=socket(AF_INET,SOCK_STREAM,0);
  fd_set sockets;
  FD_ZERO(&sockets);

  //create and prepare socket
  struct sockaddr_in serveraddr, clientaddr;
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_port = htons(9876);
  serveraddr.sin_addr.s_addr=INADDR_ANY;


  if(bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))< 0){
	printf("Trouble binding");
	}

	printf("Bind Complete");

	//Listen for a client
  listen(sockfd,10);

  //Accept incoming connection
  printf("Waiting for incoming connection...");
  int len=sizeof(clientaddr);

  while( (new_socket = accept(sockfd, (struct sockaddr *)&clientaddr, (socklen_t*)&len)) )
  {
		printf("Connection accepted");

		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = new_socket;

		if( pthread_create( &sniffer_thread , NULL , connection_handler , (void*) new_sock < 0)
		{
				perror("could not create thread");
				return 1;
		}
  }

  if(new_socket < 0)
  {
		perror("accept failed");
		return 1;
  }
  return 0;
  }

  void *connection_handler(void *socket_desc)
  {
	//Get the socket descriptor
	int sock = *(int*) socket_desc;
	int read_size;
	char *message , client_message[2000];

	//Receive a message
	while( (read_size = recv(sock , client_message , 2000 , 0)) > 0)
	{
		//Send the message from client
		write(sock , client_message , strlen(client_message));
	}

	if(read_size == 0)
	{
		printf("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}

	//Free socket pointer
	free(socket_desc);
  }
  FD_SET(sockfd,&sockets);

  while(1){
    fd_set tmp_set = sockets;
    select(FD_SETSIZE,&tmp_set,NULL,NULL,NULL);
    int i;
    for(i=0; i<FD_SETSIZE; i++){
      if(FD_ISSET(i,&tmp_set)){
	if(i==sockfd){
	  printf("A client connected\n");
	  int clientsocket = accept(sockfd,
				    (struct sockaddr*)&clientaddr,
				    &len);
	  FD_SET(clientsocket,&sockets);
	} else {
	  char line[5000];
	  recv(i,line,5000,0);
	  printf("Got from client: %s\n",line);
	  send(i,line,strlen(line),0);
	  close(i);
	  FD_CLR(i,&sockets);
	}
      }
    }
  }

  return 0;
}
