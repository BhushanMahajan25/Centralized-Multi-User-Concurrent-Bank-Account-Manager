#include "../headers/fileHandling.hpp"
#include<algorithm>
#include<functional>

using namespace std;

class Transaction{
    private:
        char _transType;
        int _accountNumber;
        int _amount;
        int _timest;
    public:
        Transaction(){}
        // timestamp?
        const int getTimestamp(){
            return this->_timest;
        }
        void setTimestamp(const int timestamp){
            _timest = timestamp;
        }

        // account number, transaction type(withdrawal/deposit), amount (space separated)
        const int getAccountNumber(){
            return this->_accountNumber;
        }
        void setAccountNumber(const int accountNumber){
            this->_accountNumber = accountNumber;
        }

        const char getTransactionType(){
            return this->_transType;
        }
        void setTransactionType(const char transType){
            this->_transType = transType;
        }

        const int getAmount(){
            return this->_amount;
        }
        void setAmount(const int amount){
            this->_amount = amount;
        }

        ~Transaction(){}
};


bool is_file_empty(std::fstream& pFile){
    //https://stackoverflow.com/questions/2390912/checking-for-an-empty-file-in-c
    return pFile.peek() == std::ifstream::traits_type::eof();
}

//overloading the function!
bool is_file_empty(std::ifstream& pFile){
    //https://stackoverflow.com/questions/2390912/checking-for-an-empty-file-in-c
    return pFile.peek() == std::ifstream::traits_type::eof();
}

void readRecords(fstream &recordsFile, vector<User> &userVec){
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

int openRecordsFile(fstream& recordsFile, string recoFileName){
    recordsFile.open(recoFileName, ios::in | ios::out);
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

void updateRecordFile(int currentBalance, int latestBal, int accNum, User userObj,fstream &recordsFile){
    cout<<"inside updateRecordFile()"<<endl;
    recordsFile.clear();
    recordsFile.seekg(0, ios::beg);

    string line;
    size_t lenCurrBal = to_string(currentBalance).length();
    cout<<endl<<"length of curr bal:: "<<lenCurrBal<<endl;


    //I am having trouble in this while loop
    while (getline(recordsFile, line)){
        if(!line.empty()){
            //recordsFile>>currWord;
            //cout<<"current line:: "<<line<<endl;
            size_t pos = line.find(to_string(accNum));
            if(pos != string::npos){
                cout<<"outer string::npos:: "<<string::npos<<endl;
                cout<<"outer found pos:: "<<pos<<endl<<endl;
                pos = line.find(to_string(currentBalance));
                size_t tellPos = recordsFile.tellp();
                recordsFile.seekp(tellPos - 6);
                cout<<"before tellPos:: "<<tellPos<<endl;
                cout<<"after tellPos:: "<<recordsFile.tellp()<<endl;
                string lb = std::to_string(latestBal);
                const char* lbPtr = lb.c_str();
                recordsFile.write(lbPtr, lb.length());
            }
        }
    }
}

void updateRecords(fstream &recordsFile, vector<User> &userVec, vector<Transaction> &transVec){
    for(Transaction t : transVec){
        int userIdx = isUser(t.getAccountNumber(), userVec);
        if(userIdx == -1){
            cout<<endl<<endl<<"user with account number:: "<<t.getAccountNumber()<<" not found!!"<<endl;
        }
        else{
            User userObj = userVec.at(userIdx);
            cout<<"user "<<userObj.getName()<<" with account number:: "<<t.getAccountNumber()<<" found!!"<<endl;
            int currentBalance = userObj.getAccountBalance();
            if(tolower(t.getTransactionType()) == 'w' && t.getAmount() <= userObj.getAccountBalance() && userObj.getAccountBalance() != 0){
                int latestBal = userObj.getAccountBalance() - t.getAmount();
                userObj.setAccountBalance(latestBal);
                updateRecordFile(currentBalance, latestBal, t.getAccountNumber(), userObj,recordsFile);
            }
            else if(tolower(t.getTransactionType()) == 'd' && t.getAmount() > 0){
                int latestBal = userObj.getAccountBalance() + t.getAmount();
                userObj.setAccountBalance(latestBal);
                updateRecordFile(currentBalance ,latestBal, t.getAccountNumber(),userObj,recordsFile);
            }
        }
    }
}

int main(){
    
    vector<User> userVec;
    vector<Transaction> transVec;
    string recoFileName = "../input-files/Records.txt";
    string transFileName = "../input-files/Transactions.txt";

    fstream recordsFile;
    ifstream transFile;
    ofstream newRecordsFile;
    
    //IMP: in Records.txt there must not be any empty line!!
    if(openRecordsFile(recordsFile, recoFileName) == 1){
        cout<<"records file opend!!"<<endl;
        readRecords(recordsFile, userVec);
    }
    else{
        cout<<"exting the program. First load the records file!";
        exit(1);
    }

    if(openTransactionFile(transFile, transFileName) == 1){
        cout<<"transaction file opend!!"<<endl;
        readTransactions(transFile, transVec);
        updateRecords(recordsFile, userVec, transVec);
    }
    else{
        cout<<"exting the program. First load the transactions file!";
        exit(1);
    }

    //updateRecords(recordsFile, userVec, transVec);

    recordsFile.close();

    return 0;
}