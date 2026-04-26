#ifndef ACCOUNT_H
#define ACCOUNT_H
#include<iostream>
using namespace std;
class User;
class Transaction;
class Account {
private:
    string accountNumber;
    string holdername;
    double balance;
    Transaction** transactions; 
    User *user; 
    int transactionCount;
    bool status;
public:
    Account(string accNo, double initialBalance,User *u);
    ~Account();
    void deposit(double amount);
    bool withdraw(double amount);
    bool transfer(Account* target, double amount);
    double getBalance();
    string getAccountNumber();
    string getHolderName();
    bool getIsActive();
    void setIsActive(bool status);
    void printMiniStatement();
    void addTransaction(Transaction* t);
    void saveToFile(){
    }
    void loadFromFile();
      void showBalance();  
};
#endif