#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};

  //create a socket file descriptor
  if((server_fd = socket(AF_INET, SOCK_STREAM, 0))==0){
    perror("socket failed!");
    exit(EXIT_FAILURE);
  }

  //set socket options
  // if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
  //   perror("setsockopt");
  //   exit(EXIT_FAILURE);
  // } -> commented as was giving error in macos

  // Allow address reuse
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
      perror("setsockopt SO_REUSEADDR");
      exit(EXIT_FAILURE);
  }

  // Allow port reuse (optional, and only works properly on Linux and newer macOS with caution)
  #ifdef SO_REUSEPORT
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
      perror("setsockopt SO_REUSEPORT");
      exit(EXIT_FAILURE);
  }
  #endif


  //setup the server address
  //set the address family to AF_INET (IPv4)
  address.sin_family = AF_INET;

  //accepting connection on any available interface/ communication mean
  address.sin_addr.s_addr = INADDR_ANY;

  //set the port number in network byte order / PORT 8080
  address.sin_port = htons(PORT);

  if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
   perror("bind fails!");
   exit(EXIT_FAILURE);
  }

  //start listening for incoming connections
  if(listen(server_fd, 3) < 0){
    perror("listening fails!");
    exit(EXIT_FAILURE);
  }

  //print a message indication that the server is listening on the port
  printf("Server listening on port %d\n", PORT);

  //accept incoming connections
  if((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0){
    perror("Accept!");
    exit(EXIT_FAILURE);
  }

  //accepting the connection
  printf("Connection accepted\n");

  //read data from the client and print it
  //declare a signed size type variable for the number of bytes read

  ssize_t valread;
  while((valread = read(new_socket, buffer, BUFFER_SIZE)) > 0){
    printf("Client: %s", buffer);
    memset(buffer, 0, sizeof(buffer));
  }

  //close the socket
  close(server_fd);
  return 0;
}
