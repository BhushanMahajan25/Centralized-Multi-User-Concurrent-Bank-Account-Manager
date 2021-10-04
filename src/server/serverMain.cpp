#include "../headers/common.hpp"
#include "../headers/fileHandling.hpp"

#define SERVER_PORT 8080  // macro for server port. get port from terminal args and delete this
#define SERVER_IP "10.0.0.237" // macro for serverip. get ip from terminal args and delete this

void testFunction(int clientSocketFd){
  int counter = 1;
  while(true){
    int valread = 0;
    /*
      one way to receive strings over socket: https://stackoverflow.com/questions/31900493/sending-a-string-over-a-network-using-sockets-in-c
    */
    char buffer[1024] = {0};
    valread = read(clientSocketFd, (void*) &buffer, 1024);
    if(valread > 0){
      cout<<"valread:: "<<valread<<" "<<"buffer from client:: "<<buffer<<endl;
      string hello = "ACK:: got msg:: " + to_string(counter);
      send(clientSocketFd, (void*) &hello, hello.length(), 0);
      counter++;
    } else{
      cout<<"valread <=0....closing server and client now"<<endl;
      close(clientSocketFd);
      break;
    }
  }
  close(clientSocketFd);
}

int main(int argc, char **argv) {

  /* file handling variables*/
  string fileName = "../input-files/Records.txt";
  vector<User> userVec;
  fstream recordsFile;

  /*socket variables*/
  int serverFd = 0;
  int clientSocketFd = 0;
  
  int setReuseAddr = 1; // ON == 1  
  int maxPendingConnections = 1;

  struct sockaddr_in server;
  struct sockaddr_in client;
  socklen_t clientAddrLen; 

  if(openRecordsFile(recordsFile, fileName) == 1){
    readRecords(recordsFile, userVec);
  }
  else{
    cout<<"exting the program. First load the file!";
    exit(0);
  }

  // create socket (IPv4, stream-based, protocol set to TCP)
  if((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    cout<<"server failed to create the listening socket"<<endl;
    exit(1);
  }

  // set socket to use the address passed in terminal args
  // here hardcoded macro is used 10.0.0.237 
  // can be bound separately (helps to avoid conflicts) 
  if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &setReuseAddr, sizeof(setReuseAddr)) != 0){
    cout<<"server failed to set SO_REUSEADDR socket option (not fatal)"<<endl;
  }

  // configure server socket address structure (init to zero, IPv4,
  // network byte order for port and address)
  // bzero(&server, sizeof(server));
  server.sin_family = AF_INET;
  //server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_addr.s_addr = inet_addr(SERVER_IP); //https://stackoverflow.com/questions/15673846/how-to-give-to-a-client-specific-ip-address-in-c
  server.sin_port = htons(SERVER_PORT); //use port specified in terminal args

  // bind the socket
  if(::bind(serverFd, (struct sockaddr*) &server, sizeof(server)) < 0){ //c++11
    cout<<"server failed to bind"<<endl;
    exit(1);
  }

  // listen on the socket for up to some maximum pending connections
  if(listen(serverFd, maxPendingConnections) < 0){
    cout<<"server failed to listen"<<endl;
    exit(1);
  } else{
    cout<<"server listening for a connection on port:: "<<SERVER_PORT<<endl;
  }

  // get the size client's address struct
  clientAddrLen = sizeof(client);

  // accept a new client
  if((clientSocketFd = accept(serverFd, (struct sockaddr*) &client, &clientAddrLen)) < 0){
    cout<<"server accept failed"<<endl;
  } else{
    cout<<"server accepted a client!"<<endl;
    testFunction(clientSocketFd);
  }

  // Close the socket and return 
  close(serverFd);
  return 0;
}