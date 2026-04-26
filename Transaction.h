#ifndef TRANSACTION_H
#define TRANSACTION_H
#include<iostream>
#include <cstring>
using namespace std;
class Account;
class Transaction//abstract base, cant make a transaction obj directly
{
protected:
    string type;
    string timestamp;
    double amount;

public:
    Transaction(string t, double amt);
    virtual void execute(Account *acc) = 0; // pure virtual
    virtual void printReceipt() = 0;        // pure virtual
    double getAmount();
    string getType();
    string getTimestamp();
    virtual ~Transaction(); 
};
class LogEntry : public Transaction {
public:
    LogEntry(string t, double amt) : Transaction(t, amt) {}
    void execute(Account* acc) override;  
    void printReceipt() override;
};
#endif