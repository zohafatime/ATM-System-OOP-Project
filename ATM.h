#ifndef ATM_H
#define ATM_H

#include <iostream>
#include <cstring>
#include <fstream>
#include "User.h"
#include "Account.h"
#include "Transaction.h"
using namespace std;

class Admin; 

class ATM
{
private:
    Account *currentAccount;
    Account **accounts;
    int count;
    Admin **admins;      
    int adminCount;
    double cashAvailable;
    ofstream logFile;

public:
    ATM();
    ~ATM();

    void run();
    void runUserMenu();   
    void runAdminMenu();  
    void loadAdmins();    
    bool authenticate(string accNo, string pin);
    void showMenu();
    void processTransaction(int choice);

    void loadAccounts();
    void saveAccounts();
    void logTransaction(Account *source, Transaction *t);
    void refillCash(double amount);
    double getCashAvailable();
    void checkFirstRun();
    friend class Admin;
};

#endif