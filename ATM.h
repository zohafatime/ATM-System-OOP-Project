#ifndef ATM_H
#define ATM_H

#include <iostream>
#include <string>
#include <fstream>
#include "User.h"
#include "Account.h"

using namespace std;

class ATM {
private:
    Account *currentAccount;
    Account **accounts;
    int count;
    double cashAvailable;
    ofstream logFile;

public:
    ATM();
    ~ATM();

    void run();
    bool authenticate(string accNo, string pin);
    void showMenu();
    void processTransaction(int choice);

    void loadAccounts();
    void saveAccounts();
    void logTransaction(Account *source,Transaction *t);
    void refillCash(double amount);
    double getCashAvailable();
};

#endif