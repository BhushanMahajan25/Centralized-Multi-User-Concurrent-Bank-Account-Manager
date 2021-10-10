#include "../headers/common.hpp"
#include "../headers/Transaction.hpp"
#include "../headers/User.hpp"
#include "../headers/clientSocketQueue.hpp"
#include<pthread.h>


#define SERVER_PORT 8080  // macro for server port. get port from terminal args and delete this
#define SERVER_IP "10.0.0.237" // macro for serverip. get ip from terminal args and delete this
#define THREAD_POOL_SIZE 20

int serverFd, clientSocketFd, threadPoolSize;

pthread_t threadPool[THREAD_POOL_SIZE];
pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;

void handleReadWriteConnection(void*);
void* threadFunction(void*);


//https://www.youtube.com/watch?v=P6Z5K8zmEmc&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM&index=7
void* threadFunction(void* arg){
	while(true){
		int* pClient;
		pthread_mutex_lock(&mx);
		if((pClient = dequeue()) == NULL){
			pthread_cond_wait(&cnd, &mx);
			//try again
			pClient = dequeue();
		}
		pthread_mutex_unlock(&mx);
		
		//if(pClient != NULL){
			handleReadWriteConnection(pClient);
		//}
	}
	return NULL;
}

void handleReadWriteConnection(void* pClientSocketFd){
	int clientSocketFd = *((int*)pClientSocketFd);
	free(pClientSocketFd);
	int counter = 1;
	while(true){
		int valread = 0;
		cout<<"inside while server"<<endl;
		/*
		one way to receive strings over socket: https://stackoverflow.com/questions/31900493/sending-a-string-over-a-network-using-sockets-in-c
		*/
		char buffer[1024] = {0};
		valread = read(clientSocketFd, (void*) &buffer, 1024);
		if(valread > 0){
			cout<<"valread:: "<<valread<<" "<<"buffer from client:: "<<buffer<<endl;
			string ack = "ACK:: got msg::" + to_string(counter) + "::"+buffer;
			send(clientSocketFd, (void*) &ack, ack.length(), 0);
			counter++;
		} else{
			cout<<"valread <=0....closing server and client now"<<endl;
			close(clientSocketFd);
			return;
		}
	}
}

int main(int argc, char **argv) {

	// exit the program if invalid arguments are passed
	if(argc != 4 ){
        cout<<"ERROR: Invalid Arguments! Please run the program with args as ./<exe file> <IP address> <server port> <input file path>"<<endl;
        exit(EXIT_FAILURE);
    }

	/* file handling variables*/
	string fileName = argv[3];	//"../input-files/Records.txt";
	vector<User> userVec;
    vector<Transaction> transVec;
    string recoFileName = "input-files/Records.txt";

    ifstream recordsFile;
    ifstream transFile;

	/*socket variables*/
	//int serverFd, clientSocketFd, threadPoolSize;
	int serverPort = stoi(argv[2]);
	int setReuseAddr = 1; // ON == 1  
	int maxPendingConnections = 1;
	char* serverIP = argv[1];

	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t clientAddrLen; 

	//IMP: in Records.txt there must not be any empty line!!
    if(openRecordsFile(recordsFile, recoFileName) == 1){
        cout<<"records file opend!!"<<endl;
        readRecords(recordsFile, userVec);
		recordsFile.close();
    }
    else{
        cout<<"exiting the program. First load the records file!"<<endl;
        exit(1);
    }

	//create threads in thread_pool and reuse them for future client connections
	for(int i = 0; i < THREAD_POOL_SIZE; i++){
		pthread_create(&threadPool[i], NULL, threadFunction, NULL);
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
	server.sin_addr.s_addr = inet_addr(serverIP); //https://stackoverflow.com/questions/15673846/how-to-give-to-a-client-specific-ip-address-in-c
	server.sin_port = htons(serverPort); //use port specified in terminal args

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
		cout<<"server listening for a connection on port:: "<<serverPort<<endl;
	}

	// get the size client's address struct
	clientAddrLen = sizeof(client);

	while(true){
		// accept a new client
		if((clientSocketFd = accept(serverFd, (struct sockaddr*) &client, &clientAddrLen)) < 0){
			cout<<"server accept failed"<<endl;
		} else{
			cout<<"server accepted a client!"<<endl;
			/*pthread_t t;
			pthread_create(&t, NULL, handleReadWriteConnection,pClient);*/

			//using deque STL to store the connection so that a thread from thread pool can find it.
			int* pClient = new int;
			*pClient = clientSocketFd;
			pthread_mutex_lock(&mx);
			enqueue(pClient);
			pthread_cond_signal(&cnd);
			pthread_mutex_unlock(&mx);
		}
	}
	// Close the socket and return 
	close(clientSocketFd);
	close(serverFd);
	return 0;
}