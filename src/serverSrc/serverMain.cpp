#include "../headers/common.hpp"
#include "../headers/Transaction.hpp"
#include "../headers/User.hpp"
#include<pthread.h>
#include<sstream>

#define THREAD_POOL_SIZE 32

int serverFd, clientSocketFd;
vector<User> userVec;

pthread_t threadPool[THREAD_POOL_SIZE];
pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;
// vector<pthread_mutex_t> mx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;

void handleReadWriteConnection(int);
void* threadFunction(void*);
void* addInterest(void*);
string updateRecords(int, float, string);

void* addInterest(void* arg){
	float interestRate = 0.05f;
	string buff;
	while(true){
		sleep(2);
		cout<<"adding interest rate to all accounts"<<endl;
		pthread_mutex_lock(&mx);
		buff = updateRecords(0, interestRate, "i");
		cout<<buff<<endl;
		send(clientSocketFd, buff.c_str(), buff.size(),0);
		pthread_mutex_unlock(&mx);
	}
	return NULL;
}

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
		handleReadWriteConnection(*pClient);
		close(*pClient);
	}
	return NULL;
}

string updateRecords(int userIdx, float amount, string transactionType){
	string buff;
	User userObj = userVec.at(userIdx);
	if(transactionType != "i"){
		cout<<"user "<<userObj.getName()<<" with account number:: "<<userObj.getAccountNumber()<<" found!!"<<endl;
	}
	float latestBal = 0.0f;
	if(transactionType == "w"){
		if(amount == 0.0f){
			buff = "ERROR:: Transaction amount cannot be zero!";
		}else if(userObj.getAccountBalance() - amount < 0.0f){
			buff = "ERROR:: Insufficient balance!";
		}else{
			latestBal = userObj.getAccountBalance() - amount;
			userObj.setAccountBalance(latestBal);
			userVec.at(userIdx) = userObj;
			buff = "SUCCESS:: Withdraw:: Latest balance of account "+to_string(userObj.getAccountNumber())+" :: "+to_string(latestBal);
		}
	}else if(transactionType == "d"){
		if(amount == 0.0f){
			buff = "ERROR:: transaction amount cannot be zero!";
		}else{
			latestBal = userObj.getAccountBalance() + amount;
			userObj.setAccountBalance(latestBal);
			userVec.at(userIdx) = userObj;
			buff = "SUCCESS:: Deposit:: Latest balance of account "+to_string(userObj.getAccountNumber())+" :: "+to_string(latestBal);
		}
	}else if(transactionType == "i"){
		for(auto userObj = userVec.begin(); userObj != userVec.end(); userObj++){
			float temp = (*userObj).getAccountBalance()*amount;
			cout<<"temp:: "<<temp<<endl;
			(*userObj).setAccountBalance(temp);
			cout<<"updated information:: "<<endl;
			cout<<"account number:: "<<(*userObj).getAccountNumber()<<endl;
			cout<<"user name:: "<<(*userObj).getName()<<endl;
			cout<<"account balance:: "<<(*userObj).getAccountBalance()<<endl;
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
		buff = "ERROR:: Invalid transaction type operation!!";
	}
	return buff;
}

void handleReadWriteConnection(int pClientSocketFd){
	int clientSocketFd = pClientSocketFd;
	while(true){
		int valread = 0;
		int accountNumber;
		string transactionType;
		float amount = 0.0f;
		cout<<"inside while server"<<endl;
		/*
		one way to receive strings over socket: https://stackoverflow.com/questions/31900493/sending-a-string-over-a-network-using-sockets-in-c
		*/
		char buffer[1024] = {0};
		valread = read(clientSocketFd, (void*) &buffer, 1024);
		if(valread > 0){
			cout<<"valread:: "<<valread<<" "<<"buffer from client:: "<<buffer<<endl;
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
				pthread_mutex_lock(&mx);
				buff = updateRecords(userIdx, amount, transactionType); 
				cout<<buff<<endl;
				send(clientSocketFd, buff.c_str(), buff.size(),0);
				pthread_mutex_unlock(&mx);
			}
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
        exit(EXIT_FAILURE);
    }

	//create threads in thread_pool and reuse them for future client connections
	for(int i = 0; i < THREAD_POOL_SIZE; i++){
		pthread_create(&threadPool[i], NULL, threadFunction, NULL);
	}

	// create socket (IPv4, stream-based, protocol set to TCP)
	if((serverFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		cout<<"server failed to create the listening socket"<<endl;
		exit(EXIT_FAILURE);
	}

	// set socket to use the address passed in terminal args
	// here hardcoded macro is used 10.0.0.237 
	// can be bound separately (helps to avoid conflicts) 
	if(setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &setReuseAddr, sizeof(setReuseAddr)) != 0){
		cout<<"server failed to set SO_REUSEADDR socket option (not fatal)"<<endl;
	}

	// configure server socket address structure (init to zero, IPv4,
	// network byte order for port and address)
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(serverIP); //https://stackoverflow.com/questions/15673846/how-to-give-to-a-client-specific-ip-address-in-c
	server.sin_port = htons(serverPort); //use port specified in terminal args

	// bind the socket
	if(::bind(serverFd, (struct sockaddr*) &server, sizeof(server)) < 0){ //c++11
		cout<<"server failed to bind"<<endl;
		exit(EXIT_FAILURE);
	}

	// listen on the socket for up to some maximum pending connections
	if(listen(serverFd, maxPendingConnections) < 0){
		cout<<"server failed to listen"<<endl;
		exit(EXIT_FAILURE);
	} else{
		cout<<"server listening for a connection on port:: "<<serverPort<<endl;
	}

	// get the size client's address struct
	clientAddrLen = sizeof(client);

	pthread_t ir;
	pthread_create(&ir, NULL, addInterest, NULL);

	while(true){
		// accept a new client
		if((clientSocketFd = accept(serverFd, (struct sockaddr*) &client, &clientAddrLen)) < 0){
			cout<<"server accept failed"<<endl;
		} else{
			cout<<"server accepted a client!"<<endl;

			//using deque STL to store the connection so that a thread from thread pool can find it.
			int* pClient = new int;
			*pClient = clientSocketFd;
			pthread_mutex_lock(&mx);
			enqueue(pClient);
			pthread_cond_signal(&cnd);
			pthread_mutex_unlock(&mx);
		}
	}
	// Close the socket
	close(serverFd);
	return 0;
}