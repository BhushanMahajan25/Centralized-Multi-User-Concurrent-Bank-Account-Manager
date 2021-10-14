/** @file This file contains code of socket communication with server
 * 	@brief Client reads Transactions.txt, sends the data to server, and receives messages from the server
*/

#include<chrono>
#include "../headers/common.hpp"
#include "../headers/Transaction.hpp"

using namespace std::chrono;

// logic
float sendTransactionsToServer(int&, vector<Transaction>&);

void writeTimestamp(float &, int);

void writeTimestamp(float &meanExecTime, int totalSize){
	//commented as the below code is used for testing scalability
	ofstream file1;
	file1.open("logs/file.txt",ios::app);
	if(!file1.is_open()){
		cout << endl << "log file could not be opened" <<endl;
		exit(0);
	}
	file1  << "average time of execution of all transactions " + to_string(meanExecTime/totalSize) << "\n";
	file1.close();
}

/** @brief This function sends transaction data to connected server.
 * The function also receives the from connected server
 * The client socket closes after successful communication.
 * @param clientFd integer reference of client socketfd 
 * @param transVec reference of vector<Transaction>
*/

//https://www.geeksforgeeks.org/measure-execution-time-function-cpp/
float sendTransactionsToServer(int& clientFd, vector<Transaction>& transVec){
	int startClock, stopClock;
	float avgExecTime = 0.0f;
	for(Transaction txn : transVec){
		string message = to_string(txn.getAccountNumber()) + " " + txn.getTransactionType() + " " + to_string(txn.getAmount());
		auto start = high_resolution_clock::now();
		send(clientFd, message.c_str(), message.size(), 0); 
		char buffer[4096] = {0};
		int valread = 0;
		valread = recv(clientFd, (void*) &buffer, sizeof(buffer),0);
		auto stop = high_resolution_clock::now();
		if(valread > 0){
			cout<<"SERVER:: Buffer from server:: "<<buffer<<endl;
		} else{
			cout<<"ERROR: No data received from the server!!!"<<endl;
		}
		auto duration = duration_cast<microseconds>(stop - start);
		avgExecTime += duration.count();
		cout << "Time taken by the function:: "<< duration.count() << " microseconds" << endl;
		sleep(2);
	}
	close(clientFd);
	return avgExecTime;
}

int main(int argc, char **argv) {

	// exit the program if invalid arguments are passed
	if(argc != 4 ){
		cout<<"ERROR: Invalid Arguments! Please run the program with args as ./<exe file> <IP address> <server port> <input file path>"<<endl;
		exit(EXIT_FAILURE);
	}

	int clientFd;
	int serverPort = stoi(argv[2]);
	char* serverAddress = argv[1];
	struct sockaddr_in serverSocketAddr;
	float meanExecTime = 0.0f;

	//file handling variables
    vector<Transaction> transVec;
    string transFileName = argv[3];
    ifstream transFile;

	if(openFile(transFile, transFileName) == 1){
        cout<<"SUCCESS:: Transaction file opened!!"<<endl;
        readTransactions(transFile, transVec);
    }
    else{
        cout<<"ERROR:: Exting the program. First load the transactions file!"<<endl;
        exit(EXIT_FAILURE);
    }

	// Create socket (IPv4, stream-based, protocol set to TCP)
	if((clientFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		cout<<"ERROR:: Client failed to create socket"<<endl;
		exit(EXIT_FAILURE);
	}
	
	// Configure server socket address structure (init to zero, IPv4,
	// network byte order for port and address) 
	bzero(&serverSocketAddr, sizeof(serverSocketAddr));
	serverSocketAddr.sin_family = AF_INET;
	serverSocketAddr.sin_port = htons(serverPort);
	serverSocketAddr.sin_addr.s_addr = inet_addr(serverAddress);

	// Connect socket to server
	if(connect(clientFd, (struct sockaddr *)&serverSocketAddr, sizeof(serverSocketAddr)) < 0){
		cout<<"ERROR:: Client failed to connect to "<<serverAddress<<" : "<<serverPort<<endl;
		close(clientFd);
		exit(EXIT_FAILURE);
	} else {
		cout<<"SUCCESS:: Client to connect to "<<serverAddress<<" : "<<serverPort<<endl;
		meanExecTime = sendTransactionsToServer(clientFd, transVec);
	}

	// writeTimestamp(meanExecTime, transVec.size());

	// Close the socket and return 0
	close(clientFd);
	return 0; 
}