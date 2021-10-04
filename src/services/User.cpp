
#include<iostream>
#include "../headers/User.hpp"

using namespace std;

User::User(){};

string const User::getName(){
    return this->_name;
};

void User::setName(const string name){
    this->_name = name;
}

int const User::getAccountNumber(){
    return this->_accountNumber;
}
void User::setAccountNumber(const int accountNumber){
    this->_accountNumber = accountNumber;
}

int const User::getAccountBalance(){
    return this->_accountBalance;
}
void User::setAccountBalance(const int accountBalance){
   this->_accountBalance = accountBalance;
}

User::~User(){};
