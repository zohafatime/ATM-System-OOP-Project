#ifndef REGISTEREDUSER_H
#define REGISTEREDUSER_H

#include "User.h"
#include <string>
#include<iostream>
using namespace std;
class Account; 

class RegisteredUser : public User {
private:
    int failedAttempts;
    bool isLocked;
    Account* account; 

public:
    RegisteredUser(string n, string acc, string p, Account* a);

    bool verifyPin(string input) override;
    void showMenu() override;
    string getRole() override;

    void lockAccount();
    void changePin(string oldPin, string newPin);
    Account* getAccount();
    bool getIsLocked();
    
    ~RegisteredUser();
};

#endif