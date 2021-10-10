#include "../headers/common.hpp"
#include "../headers/Transaction.hpp"
#include "../headers/User.hpp"
//#include<algorithm>
//#include<functional>


bool is_file_empty(ifstream& pFile){
    //https://stackoverflow.com/questions/2390912/checking-for-an-empty-file-in-c
    return pFile.peek() == ifstream::traits_type::eof();
}

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
    //recordsFile.close();

    cout<<endl<<"printing user objects vector:: "<<endl;
    for(User i : userVec){
        cout<<i.getAccountNumber()<<"\t"<<i.getName()<<"\t"<<i.getAccountBalance()<<endl;
    }
}

void readTransactions(ifstream &transFile, vector<Transaction> &transVec){
    string currLine;
    do{ 
        Transaction *transObj = new Transaction();
        transFile>>currLine;
        //cout<<currLine.data()<<endl;
        if(!currLine.empty())
            transObj->setTimestamp(stoi(currLine));
        transFile>>currLine;
        //cout<<currLine<<endl;
        if(!currLine.empty())
            transObj->setAccountNumber(stoi(currLine));
        transFile>>currLine;
        //cout<<currLine<<endl;
        if(!currLine.empty())
            transObj->setTransactionType(currLine.at(0));
        transFile>>currLine;
        //cout<<currLine<<endl;
        if(!currLine.empty())
            transObj->setAmount(stoi(currLine));
        if(!currLine.empty())
            transVec.push_back(*transObj);
        getline(transFile,currLine);
    }while(!transFile.eof());
    transFile.close();

    cout<<endl<<"printing transaction objects vector:: "<<endl;
    for(Transaction i : transVec){
        cout<<i.getAccountNumber()<<"\t"<<i.getTransactionType()<<"\t"<<i.getAmount()<<endl;
    }
}

int openRecordsFile(ifstream& recordsFile, string recoFileName){
    recordsFile.open(recoFileName, ios::in);
    if(recordsFile.is_open() && !is_file_empty(recordsFile)){
        cout<<"Printitng records!!"<<endl;
        return 1;
    }
    cout<<"File is either not opened or file is empty!!"<<endl;
    return -1;
}

int openTransactionFile(ifstream& transFile, string transFileName){
    transFile.open(transFileName, ios::in);
    if(transFile.is_open() && !is_file_empty(transFile)){
        cout<<"Printitng transactions!!"<<endl;
        return 1;
    }
    cout<<"File is either not opened or file is empty!!"<<endl;
    return -1;
}


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

void updateRecords(ifstream &recordsFile, vector<User> &userVec, vector<Transaction> &transVec){
    for(Transaction t : transVec){
        int userIdx = isUser(t.getAccountNumber(), userVec);
        if(userIdx == -1){
            cout<<endl<<endl<<"user with account number:: "<<t.getAccountNumber()<<" not found!!"<<endl;
            //skip the current transaction and move onto the next transaction
        }
        else{
            User userObj = userVec.at(userIdx);
            cout<<"user "<<userObj.getName()<<" with account number:: "<<t.getAccountNumber()<<" found!!"<<endl;
            int currentBalance = userObj.getAccountBalance();
            if(tolower(t.getTransactionType()) == 'w' && t.getAmount() <= userObj.getAccountBalance() && userObj.getAccountBalance() != 0){
                int latestBal = userObj.getAccountBalance() - t.getAmount();
                userObj.setAccountBalance(latestBal);
            }
            else if(tolower(t.getTransactionType()) == 'd' && t.getAmount() > 0){
                int latestBal = userObj.getAccountBalance() + t.getAmount();
                userObj.setAccountBalance(latestBal);
            }
        }
    }
}

//int main(){
    
    // vector<User> userVec;
    // vector<Transaction> transVec;
    // string recoFileName = "input-files/Records.txt";
    // string transFileName = "input-files/Transactions.txt";

    // ifstream recordsFile;
    // ifstream transFile;
    // ofstream newRecordsFile;
    
    // //IMP: in Records.txt there must not be any empty line!!
    // if(openRecordsFile(recordsFile, recoFileName) == 1){
    //     cout<<"records file opend!!"<<endl;
    //     readRecords(recordsFile, userVec);
    // }
    // else{
    //     cout<<"exting the program. First load the records file!"<<endl;
    //     exit(1);
    // }

    // if(openTransactionFile(transFile, transFileName) == 1){
    //     cout<<"transaction file opend!!"<<endl;
    //     readTransactions(transFile, transVec);
    //     //updateRecords(recordsFile, userVec, transVec); 
    // }
    // else{
    //     cout<<"exting the program. First load the transactions file!"<<endl;
    //     exit(1);
    // }

    //updateRecords(recordsFile, userVec, transVec);

    //recordsFile.close();

    //return 0;
//}