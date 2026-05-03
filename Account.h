#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;
class User;
class Transaction;
class Account
{
private:
    string accountNumber;
    string holdername;
    int balance;
    int failedAttempts;
    bool isLocked;
    Transaction **transactions;
    User *user;
    int transactionCount;
    string pin;

public:
    Account(string accNo, string holderName, int initialBalance);
    ~Account();
    bool deposit(int amount);
    bool withdraw(int amount);
    bool transfer(Account *target, int amount);
    void lockAccount();
    void unlockAccount();
    void resetLock();
    void printMiniStatement();
    void addTransaction(Transaction *t);
    void saveToFile();
    void loadFromFile();
    void showBalance();
    bool verifyPin(string input);
    void setPin(string newPin);

    // getter
    int getBalance();
    string getAccountNumber();
    string getHolderName();
    bool getIsActive();
};
#endif