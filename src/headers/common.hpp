/** @file common.hpp
 *  @brief This file contains common headers and functions used by multiple cpp files
*/

#ifndef COMMON_HPP
#define COMMON_HPP

/*client and serve headers*/
#include<iostream>
#include<sys/socket.h>
//#include <netdb.h>
//#include<netinet/in.h>
#include<arpa/inet.h>
//#include <stdlib.h>
#include<string>
#include<algorithm>
#include<unistd.h>

/*fileHandling.cpp headers*/
#include<fstream>
#include<vector>

//clientSocketQueue.cpp headers
#include<queue>

#include "User.hpp"
#include "Transaction.hpp"

using namespace std;

int openFile(ifstream&, string);
void readRecords(ifstream&, vector<User>&);
void readTransactions(ifstream&, vector<Transaction>&);

int isUser(int, vector<User>&);

void enqueue(int* pClient);
int* dequeue();

#endif