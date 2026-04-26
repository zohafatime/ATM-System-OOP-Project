#include <iostream>
#include <fstream>
#include <time.h>
#include <cstring>
using namespace std;

class User//abstract base, meaning no direct obj creation
{
protected:
    string name;
    string accountNumber;
    string pin;

public:
    User(string n, string acc, string p);
    virtual bool verifyPin(string input) = 0; // pure virtual meaning abstraction
    virtual void showMenu() = 0;              // pure virtual — each user type has different menu
    virtual string getRole() = 0;             // pure virtual — returns "RegisteredUser" or "Admin"
    string getName();
    string getAccountNumber();
    virtual ~User();
};
//--------------------------------------------------------------------------------------
class RegisteredUser : public User
{
private:
    int failedAttempts;
    bool isLocked;
    Account *account; // composition
public:
    RegisteredUser(string n, string acc, string p, Account *a);
    bool verifyPin(string input) override;
    void showMenu() override;
    string getRole() override;
    void lockAccount();
    void changePin(string oldPin, string newPin);
    Account *getAccount();
    bool getIsLocked();
    ~RegisteredUser();
};
//----------------------------------------------------------------------------------
class Admin : public User
{
private:
    double atmCashAvailable; 
public:
    Admin(string n, string acc, string p);
    bool verifyPin(string input) override;
    void showMenu() override;
    string getRole() override;
    void refillATMCash(double amount);              
    void addAccount(Account *acc);                  
    void removeAccount(string accNo);               
    void viewAllTransactions();                     
    void resetUserPin(string accNo, string newPin); 
    double getATMCash();
    ~Admin();
};
//-------------------------------------------------------------------------------------------
class Account
{
private:
    string accountNumber;
    string holderName;
    double balance;
    Transaction** transactions;  // array of pointers meaning aggregation
    int transactionCount;
    User *user;
    bool status;
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
    void loadFromFile();
    void showBalance();  
};
//---------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
class Withdraw : public Transaction
{
private:
    double limit;

public:
    Withdraw(double amt, double limit = 25000);
    void execute(Account *acc) override;
    void printReceipt() override;
    ~Withdraw();
};
//---------------------------------------------------------------------------------
class Deposit : public Transaction
{
private:
    string source;

public:
    Deposit(double amount, string source = "Cash");
    void execute(Account *acc) override;
    void printReceipt() override;
    ~Deposit();
};
//----------------------------------------------------------------------------
class Transfer : public Transaction
{
private:
    string toAccountNumber;
    Account *targetAccount; // aggregation
public:
    Transfer(double amount, string toAccNo, Account *target);
    void execute(Account *acc) override;
    void printReceipt() override;
    ~Transfer();
};
//---------------------------------------------------------------------------------
class ATM
{
private:
    Account* currentAccount;//aggregation
    Account** accounts;//aggregation
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
    void refillCash(double amount); // called by Admin
    double getCashAvailable();
};