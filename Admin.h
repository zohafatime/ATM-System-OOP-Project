#ifndef ADMIN_H
#define ADMIN_H
#include <cstring>
#include "User.h"    
#include "Account.h" 
#include "ATM.h" 
using namespace std;
class Admin : public User
{
private:
    double atmCashAvailable;

public:
    Admin(string n, string acc, string p);
    bool verifyPin(string input) override;
    void showMenu() override;
    string getRole() override;
    void refillATMCash(ATM &atm, double amount);
    void addAccount(Account **&accounts, int &count, Account *newAcc);
    void removeAccount(Account **&accounts, int &count,string accNo);
    void viewAllTransactions();
    void resetUserPin(Account **accounts, int count, string accNo, string newPin);
    double getATMCash();
    ~Admin();
};

#endif