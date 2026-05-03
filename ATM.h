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
    ofstream logFile;
    Account **accounts;
    int count;
    Admin **admins;
    int adminCount;

public:
    Account *currentAccount; // public cuz gui reads balance from it

    // for gui:
    //getters
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
    void showMenu();
    void processTransaction(int choice);

    void loadAccounts();
    void saveAccounts();
    void logTransaction(Account *source, Transaction *t);
    void checkFirstRun();
    void unlockAccount(string accNo);
    string generateUserID();
    string generateAdminID();

    bool isAccountActive(const string &accNo) const;
    bool accountExists(const string &accNo) const;
    bool authenticate(string accNo, string pin);
    friend class Admin;//to access private datamembers
};

#endif