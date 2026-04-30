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
    double balance;
    int failedAttempts;
    bool isLocked;
    Transaction **transactions;
    User *user;
    int transactionCount;
    bool status;
    string pin;

public:
    Account(string accNo, string holderName, double initialBalance);
    ~Account();
    void deposit(double amount);
    bool withdraw(double amount);
    bool transfer(Account *target, double amount);
    double getBalance();
    string getAccountNumber();
    string getHolderName();
    bool getIsActive();
    void setIsActive(bool status);
    void printMiniStatement();
    void addTransaction(Transaction *t);
    void saveToFile();
    void setPin(string newPin);
    void loadFromFile();
    void showBalance();
    bool verifyPin(string input); 
};
#endif