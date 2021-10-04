#include "../headers/common.hpp"

#define SERVER_ADDR "10.0.0.237"
#define SERVER_PORT 8080

void testFunction(int clientFd){
  cout<<"CLIENT:: sending msg"<<endl<<endl;
  int counter = 1;
  while(counter <= 5){
    string hello = "Message from Client::" + to_string(counter);
    send(clientFd, (void*) &hello, hello.length(), 0);
    counter++; 

    char buffer[1024] = {0};
    int valread = 0;
    valread = read(clientFd, (void*) &buffer, 1024);
    if(valread > 0){
      cout<<"valread"<<valread<<" "<<"buffer from server:: "<<buffer<<endl;
    } else{
      cout<<"valread <=0....closing client now"<<endl;
      close(clientFd);
    }
    sleep(5);
  }
}

int main(int argc, char **argv) {

  int clientFd = 0;
  struct sockaddr_in serverSocketAddr;

  // Converts localhost into 0.0.0.0
	// struct hostent *he = gethostbyname(SERVER_ADDR);
	// unsigned long server_addr_nbo = *(unsigned long *)(he->h_addr_list[0]);

  // Create socket (IPv4, stream-based, protocol likely set to TCP)
  if((clientFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    cout<<"client failed to create socket"<<endl;
    exit(1);
  }


  // Configure server socket address structure (init to zero, IPv4,
  // network byte order for port and address) 
  bzero(&serverSocketAddr, sizeof(serverSocketAddr));
  serverSocketAddr.sin_family = AF_INET;
  serverSocketAddr.sin_port = htons(SERVER_PORT);
  serverSocketAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

  // Connect socket to server
  if(connect(clientFd, (struct sockaddr *)&serverSocketAddr, sizeof(serverSocketAddr)) < 0){
    cout<<"client failed to connect to "<<SERVER_ADDR<<" : "<<SERVER_PORT<<endl;
    close(clientFd);
    exit(1);
  } else {
  	cout<<"client to connect to "<<SERVER_ADDR<<" : "<<SERVER_PORT<<endl;


    testFunction(clientFd);
  }

  // Close the socket and return
  close(clientFd);
  return 0; 
}