#include "../headers/common.hpp"
#include "../headers/Transaction.hpp"


Transaction::Transaction(){}

const int Transaction::getTimestamp(){
    return this->_timest;
}
void Transaction::setTimestamp(const int timestamp){
    this->_timest = timestamp;
}

const int Transaction::getAccountNumber(){
    return this->_accountNumber;
}
void Transaction::setAccountNumber(const int accountNumber){
    this->_accountNumber = accountNumber;
}

const char Transaction::getTransactionType(){
    return this->_transType;
}
void Transaction::setTransactionType(const char transType){
    this->_transType = transType;
}

const int Transaction::getAmount(){
    return this->_amount;
}
void Transaction::setAmount(const int amount){
    this->_amount = amount;
}

Transaction::~Transaction(){}
