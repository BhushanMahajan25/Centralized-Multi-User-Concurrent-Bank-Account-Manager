/** @file User.hpp
 *  @brief This file contains User class definition.
*/

#ifndef USER_HPP
#define USER_HPP

using namespace std;

class User{
    private:
        string _name;
        int _accountNumber;
        float _accountBalance;
    public:
        //constructor
        User();
        //getters and setters
        string const getName();
        void setName(const string name);
        int const getAccountNumber();
        void setAccountNumber(const int accountNumber);
        float const getAccountBalance();
        void setAccountBalance(const float accountBalance);
        //destructor
        ~User();
};

#endif