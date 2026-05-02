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
    double cashAvailable;
    ofstream logFile;

    // back to private
    Account **accounts;
    int count;
    Admin **admins;
    int adminCount;

public:
    Account *currentAccount; // public cuz gui reads balance from it

    // for gui:
    int getCount() const;
    int getAdminCount() const;
    Account *getAccount(int i) const;
    Account **getAccounts() const;
    Admin *getAdmin(int i) const;
    int validateAndSetPin(string p1);
    void addAccountToArray(Account *acc);
    void addAdminToArray(Admin *adm);
    void removeAccount(const string &accNo);
    void resetUserPin(const string &accNo, const string &newPin);
    //--

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
    string generateUserID();
    string generateAdminID();
    void unlockAccount(string accNo);
    friend class Admin;
};

#endif