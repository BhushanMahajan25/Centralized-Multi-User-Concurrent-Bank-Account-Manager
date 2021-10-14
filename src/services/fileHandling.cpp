#include "../headers/common.hpp"
#include "../headers/Transaction.hpp"
#include "../headers/User.hpp"

/** @brief Checks if file is empty or not
 *  @param pFile reference of file descriptor of type istream
 *  @return true if file is empty; false otherwise 
*/

// This function is cited from "https://stackoverflow.com/questions/2390912/checking-for-an-empty-file-in-c"
bool is_file_empty(ifstream& pFile){
    return pFile.peek() == ifstream::traits_type::eof();
}

/** @brief Reads the Records.txt and stores the data in vector<User>
 *  File is closed after successful reading and storing the data
 *  @param recordsFile reference of records file descriptor of type ifstream
 *  @param userVec reference of vector<User>
*/
void readRecords(ifstream &recordsFile, vector<User> &userVec){
    string currLine;
    do{ 
        User* userObj = new User();
        recordsFile>>currLine;
        //cout<<currLine<<endl;
        if(!currLine.empty())
            userObj->setAccountNumber(stoi(currLine));
        recordsFile>>currLine;
        //cout<<currLine<<endl;
        if(!currLine.empty())
            userObj->setName(currLine);
        recordsFile>>currLine;
        //cout<<currLine<<endl;
        if(!currLine.empty())
            userObj->setAccountBalance(stoi(currLine));
        if(!currLine.empty())
            userVec.push_back(*userObj);
        getline(recordsFile,currLine);
    }while(!recordsFile.eof());
    recordsFile.close();
}

/** @brief Reads the Transactions.txt and stores the data in vector<Transaction>
 *  File is closed after successful reading and storing the data
 *  @param transFile reference of trasactions file descriptor of type ifstream
 *  @param transVec reference of vector<Transaction>
*/
void readTransactions(ifstream &transFile, vector<Transaction> &transVec){
    string currLine;
    do{ 
        Transaction *transObj = new Transaction();
        transFile>>currLine;
        if(!currLine.empty())
            transObj->setTimestamp(stoi(currLine));
        transFile>>currLine;
        if(!currLine.empty())
            transObj->setAccountNumber(stoi(currLine));
        transFile>>currLine;
        if(!currLine.empty())
            transObj->setTransactionType(currLine.at(0));
        transFile>>currLine;
        if(!currLine.empty())
            transObj->setAmount(stoi(currLine));
        if(!currLine.empty())
            transVec.push_back(*transObj);
        getline(transFile,currLine);
    }while(!transFile.eof());
    transFile.close();
}


/** @brief Opens a file in read mode
 *  @param fileDesc reference of file descriptor
 *  @param fileName string of file name
 *  @return 1 if file is opened successfully and the file is not empty; -1 otherwise
 */
int openFile(ifstream& fileDesc, string fileName){
    fileDesc.open(fileName, ios::in);
    if(fileDesc.is_open() && !is_file_empty(fileDesc)){
        return 1;
    }
    return -1;
}

/**
 * @brief Checks if user exists in vector<User> by comparing account number
 * @param accNum integer account number
 * @param userVec reference of vector<User>
 * @return index if user exists; -1 otherwise
*/
int isUser(int accNum, vector<User> &userVec){
    int counter = 0;
    for(User &u : userVec){
        if(u.getAccountNumber() == accNum){
            return counter;
        }
        counter++;
    }
    return -1;
}