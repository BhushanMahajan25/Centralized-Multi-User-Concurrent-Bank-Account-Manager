#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

class Transaction{
    private:
        char _transType;
        int _accountNumber;
        int _amount;
        int _timest;
    public:
        Transaction();
        const int getTimestamp();
        void setTimestamp(const int timestamp);
        const int getAccountNumber();
        void setAccountNumber(const int accountNumber);
        const char getTransactionType();
        void setTransactionType(const char transType);
        const int getAmount();
        void setAmount(const int amount);
        ~Transaction();
};

#endif