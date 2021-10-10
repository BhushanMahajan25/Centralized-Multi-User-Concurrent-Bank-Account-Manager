#ifndef COMMON_HPP
#define COMMON_HPP

/*client and serve headers*/
#include<iostream>
#include<sys/socket.h>
//#include <netdb.h>
//#include<netinet/in.h>
#include<arpa/inet.h>
//#include <stdlib.h>
#include <string>
#include <unistd.h>

/*fileHandling.cpp headers*/
#include<fstream>
#include<vector>

//clientSocketQueue.cpp headers
#include<queue>

#include "User.hpp"
#include "Transaction.hpp"

using namespace std;

int openRecordsFile(ifstream&, string);
void readRecords(ifstream&, vector<User>&);

int openTransactionFile(ifstream&, string);
void readTransactions(ifstream&, vector<Transaction>&);

void enqueue(int* pClient);
int* dequeue();

#endif