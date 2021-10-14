/** @file serverMain.cpp
 * 	@brief This file contains multi-threaded server socket communicaion with clients.
 * The server reads and stores the user records from Records.txt
 * Performs transaction operations as per transaction data received by client.
 * Seperate function to add interst rate amount to all users accounts.
 * Handles all the operations with mutiple threads and effectivly uses mutex lock/unlock mechanism
 */

#include "../headers/common.hpp"
#include "../headers/Transaction.hpp"
#include "../headers/User.hpp"
#include<pthread.h>
#include<sstream>

#define THREAD_POOL_SIZE 32

int serverFd, clientSocketFd;
vector<User> userVec;

pthread_t threadPool[THREAD_POOL_SIZE];
vector<pthread_mutex_t> mx(THREAD_POOL_SIZE); // vector of mutex
pthread_cond_t cnd = PTHREAD_COND_INITIALIZER; // condition variable

void handleReadWriteConnection(int*);
void* threadFunction(void*);
void* addInterest(void*);
string updateRecords(int, float, string);

/** @brief Adds interest amount to all users account after particular interval of time.
*/
void* addInterest(void* arg){
	float interestRate = 1.05f;
	string buff;
	int sleepTime = 5;
	while(true){
		sleep(sleepTime);
		buff = updateRecords(0, interestRate, "i");
		cout<<buff<<endl;
		//send(clientSocketFd, buff.c_str(), buff.size(),0);
	}
	return NULL;
}

/** @brief Assigns a thread from thread-pool to recently connected client
 * If a client is not connected then the thread waits until it gets signal of client connection.
 * This saves computations as otherwise the thread would have checked, if client connection is established, recurrsively.
*/
//https://www.youtube.com/watch?v=P6Z5K8zmEmc&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM&index=7
void* threadFunction(void* arg){
	while(true){
		int* pClient;
		pthread_mutex_lock(&mx.at(0));
		if((pClient = dequeue()) == NULL){
			pthread_cond_wait(&cnd, &mx.at(0));
			//try again
			pClient = dequeue();
		}
		pthread_mutex_unlock(&mx.at(0));
		handleReadWriteConnection(pClient);
		close(*pClient);
	}
	return NULL;
}

/** @brief Performs transaction operation based on transaction type
 * @param userIdx integer index of user
 * @param amount fload amount to be either deposited or withdrawn
 * @param transactionType string type of transaction to be performed
 * @return buff string of operation acknowledgement to send to client
*/
string updateRecords(int userIdx, float amount, string transactionType){
	string buff;
	User userObj = userVec.at(userIdx);
	// if(transactionType != "i"){
	// 	cout<<"user "<<userObj.getName()<<" with account number:: "<<userObj.getAccountNumber()<<" found!!"<<endl;
	// }
	float latestBal = 0.0f;
	if(transactionType == "w"){
		if(amount == 0.0f){
			buff = "ERROR:: Transaction amount cannot be zero!";
		}else if(userObj.getAccountBalance() - amount < 0.0f){
			buff = "ERROR:: Insufficient balance in account:: "+ to_string(userObj.getAccountNumber()) + " !";
		}else{
			latestBal = userObj.getAccountBalance() - amount;
			userObj.setAccountBalance(latestBal);
			userVec.at(userIdx) = userObj;
			buff = "SUCCESS:: Withdraw:: Latest balance of account "+to_string(userObj.getAccountNumber())+" :: "+to_string(latestBal);
		}
	}else if(transactionType == "d"){
		if(amount == 0.0f){
			buff = "ERROR:: Transaction amount cannot be zero!";
		}else{
			latestBal = userObj.getAccountBalance() + amount;
			userObj.setAccountBalance(latestBal);
			userVec.at(userIdx) = userObj;
			buff = "SUCCESS:: Deposit:: Latest balance of account "+to_string(userObj.getAccountNumber())+" :: "+to_string(latestBal);
		}
	}else if(transactionType == "i"){
		int counter = 0;
		for(auto userObj = userVec.begin(); userObj != userVec.end(); userObj++){
			pthread_mutex_lock(&mx.at(counter));
			float temp = (*userObj).getAccountBalance()*amount;
			(*userObj).setAccountBalance(temp);
			pthread_mutex_unlock(&mx.at(counter));
			counter++;
		}
		// for(int i = 0; i < userVec.size(); i++){
		// 	User userObj = userVec.at(i);
		// 	float temp = userObj.getAccountBalance()*amount;
		// 	cout<<"temp:: "<<temp<<endl;
		// 	userObj.setAccountBalance(temp);
		// 	userVec.at(i) = userObj;
		// }
		buff = "SUCCESS:: Interest Deposit:: Added interest amount " + to_string(amount) + "to each user's account";
	}else{
		buff = "ERROR:: Invalid transaction type operation on account "+ to_string(userObj.getAccountNumber()) +" !!";
	}
	return buff;
}

/** @brief handles send/receive operations of socket.
 * This function receives transaction data from client 
 * and performs corresponding transaction function with the help of lock/unclock mechanism
 * @param pClientSocketFd integer pointer pointing to client socketfd
*/
void handleReadWriteConnection(int *pClientSocketFd){
	while(true){
		int valread = 0;
		int accountNumber;
		string transactionType;
		float amount = 0.0f;
		char buffer[1024] = {0};
		valread = recv(*pClientSocketFd, (void*) &buffer, 1024, 0);
		if(valread > 0){
			cout<<"CLIENT:: Buffer from client:: "<<buffer<<endl;
			istringstream ss(buffer);
			string word;
			ss>>word;
			accountNumber = stoi(word);
			ss>>word;
			transactionType = word;
			ss>>word;
			amount = stof(word);

			string buff;
			int userIdx = isUser(accountNumber, userVec);
			if(userIdx == -1){
				buff = "ERROR:: User with account number:: " + to_string(accountNumber) + " not found!!";
			}
			else{
				pthread_mutex_lock(&mx.at(userIdx));
				buff = updateRecords(userIdx, amount, transactionType); 
				cout<<buff<<endl;
				send(*pClientSocketFd, buff.c_str(), buff.size(),0);
				pthread_mutex_unlock(&mx.at(userIdx));
			}
		} else{
			// cout<<"valread <=0....closing server and client now"<<endl;
			close(*pClientSocketFd);
			free(pClientSocketFd);
			return;
		}
	}
}

int main(int argc, char **argv) {
	// exit the program if invalid arguments are passed
	if(argc != 4 ){
        cout<<"ERROR: Invalid Arguments! Please run the program with args as ./<executable server file> <IP address> <server port> <Records.txt file path>"<<endl;
        exit(EXIT_FAILURE);
    }

	/* file handling variables*/
	string recoFileName = argv[3];	//"input-files/Records.txt";
    ifstream recordsFile;
    ifstream transFile;

	/*socket variables*/
	int serverPort = stoi(argv[2]);
	int setReuseAddr = 1; // ON == 1  
	int maxPendingConnections = 1;
	char* serverIP = argv[1];

	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t clientAddrLen; 

	//open record file and read the records
    if(openFile(recordsFile, recoFileName) == 1){
        cout<<"SUCCESS:: Records file opened!!"<<endl;
        readRecords(recordsFile, userVec);
    }
    else{
		cout<<"ERROR:: File is either not opened or file is empty!!"<<endl;
        cout<<"ERROR:: Exiting the program. First load the records file!"<<endl;
        exit(EXIT_FAILURE);
    }

	//initialize the vector of mutexes with 0
	for(int i = 0; i < THREAD_POOL_SIZE; i++){
		pthread_mutex_init(&mx.at(i), 0);
	}

	//create threads in thread_pool and reuse them for future client connections
	for(int i = 0; i < THREAD_POOL_SIZE; i++){
		pthread_create(&threadPool[i], NULL, threadFunction, NULL);
	}

	// create socket (IPv4, stream-based, protocol set to TCP)
	if((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		cout<<"ERROR:: Server failed to create the listening socket"<<endl;
		exit(EXIT_FAILURE);
	}

	// set socket option to reuse the address passed in terminal args
	if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &setReuseAddr, sizeof(setReuseAddr)) != 0){
		cout<<"ERROR:: Server failed to set SO_REUSEADDR socket option"<<endl;
	}
	// set socket option to reuse the address passed in terminal args
	// wrote seperate if block fo SO_REUSEPORT because SO_REUSEADDR | SO_REUSEPORT in single if block was producing errors
	if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEPORT, &setReuseAddr, sizeof(setReuseAddr)) != 0){
		cout<<"ERROR:: Server failed to set SO_REUSEPORT socket option"<<endl;
	}

	// configure server socket address structure (init to zero, IPv4,
	// network byte order for port and address)
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(serverIP);
	server.sin_port = htons(serverPort); //use port specified in terminal args

	// bind the socket
	if(::bind(serverFd, (struct sockaddr*) &server, sizeof(server)) < 0){ //c++11
		cout<<"ERROR:: Server failed to bind"<<endl;
		exit(EXIT_FAILURE);
	}

	// listen on the socket for up to some maximum pending connections
	if(listen(serverFd, maxPendingConnections) < 0){
		cout<<"ERROR:: Server failed to listen"<<endl;
		exit(EXIT_FAILURE);
	} else{
		cout<<"SUCCESS:: Server listening for a connection on port:: "<<serverPort<<endl;
	}

	// get the size client's address struct
	clientAddrLen = sizeof(client);

	pthread_t ir;
	pthread_create(&ir, NULL, addInterest, NULL);
	int clientCounter = 0;
	while(true){
		// accept a new client
		if((clientSocketFd = accept(serverFd, (struct sockaddr*) &client, &clientAddrLen)) < 0){
			cout<<"ERROR:: Server accept failed"<<endl;
		} else{
			cout<<"SUCCESS:: Server accepted a client!"<<endl;

			//using deque STL to store the connection so that a thread from thread pool can find it.
			int* pClient = new int;
			*pClient = clientSocketFd;
			pthread_mutex_lock(&mx.at(clientCounter));
			enqueue(pClient);
			pthread_cond_signal(&cnd);
			pthread_mutex_unlock(&mx.at(clientCounter));
			clientCounter++;
		}
	}
	// Close the socket
	close(serverFd);
	return 0;
}