#ifndef USER_HPP
#define USER_HPP

using namespace std;
class User{
    private:
        string _name;
        int _accountNumber;  //for the sake of simplicity defining account number as interger.
        int _accountBalance;    //for the sake of simplicity defining balance as integer
    public:
        User();
        string const getName();
        void setName(const string name);
        int const getAccountNumber();
        void setAccountNumber(const int accountNumber);
        int const getAccountBalance();
        void setAccountBalance(const int accountBalance);
        ~User();
};

#endif