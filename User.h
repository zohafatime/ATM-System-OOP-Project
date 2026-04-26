#ifndef USER_H
#define USER_H
#include <cstring>
#include<iostream>
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

#endif