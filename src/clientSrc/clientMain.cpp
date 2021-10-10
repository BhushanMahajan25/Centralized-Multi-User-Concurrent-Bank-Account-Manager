#include "../headers/common.hpp"
#include "../headers/Transaction.hpp"

#define SERVER_ADDR "10.0.0.237"
#define SERVER_PORT 8080

void testFunction(int, vector<Transaction>&);

void testFunction(int clientFd, vector<Transaction>& transVec){
	cout<<"CLIENT:: sending msg"<<endl<<endl;
	for(Transaction txn : transVec){
		string message = to_string(txn.getAccountNumber()) + txn.getTransactionType() + to_string(txn.getAmount());
		cout<<"message:: "<<message<<endl;
		send(clientFd, (void*) &message, message.length(), 0); 

		char buffer[1024] = {0};
		int valread = 0;
		valread = read(clientFd, (void*) &buffer, 1024);
		if(valread > 0){
			cout<<"valread"<<valread<<" "<<"buffer from server:: "<<buffer<<endl;
		} else{
			cout<<"valread <=0....closing client now"<<endl;
			close(clientFd);
		}
		sleep(3);
	}
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

	//file handling variables
    vector<Transaction> transVec;
    string transFileName = argv[3];
    ifstream transFile;

	if(openTransactionFile(transFile, transFileName) == 1){
        cout<<"transaction file opend!!"<<endl;
        readTransactions(transFile, transVec);
        //updateRecords(recordsFile, userVec, transVec); 
    }
    else{
        cout<<"exting the program. First load the transactions file!"<<endl;
        exit(EXIT_FAILURE);
    }

	// Converts localhost into 0.0.0.0
	// struct hostent *he = gethostbyname(SERVER_ADDR);
	// unsigned long server_addr_nbo = *(unsigned long *)(he->h_addr_list[0]);

	// Create socket (IPv4, stream-based, protocol likely set to TCP)
	if((clientFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		cout<<"client failed to create socket"<<endl;
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
		cout<<"client failed to connect to "<<serverAddress<<" : "<<serverPort<<endl;
		close(clientFd);
		exit(EXIT_FAILURE);
	} else {
		cout<<"client to connect to "<<serverAddress<<" : "<<serverPort<<endl;
		testFunction(clientFd, transVec);
	}

	// Close the socket and return 0
	close(clientFd);
	return 0; 
}