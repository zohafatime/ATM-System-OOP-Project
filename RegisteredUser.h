#ifndef REGISTEREDUSER_H
#define REGISTEREDUSER_H

#include "User.h"
#include "Account.h"
#include <string>
#include <iostream>
using namespace std;

class RegisteredUser : public User
{
private:
    Account *account;

public:
    RegisteredUser(string n, string acc, string p, Account *a);
    void showMenu() override;
    string getRole() override;
    void changePin(string oldPin, string newPin);
    Account *getAccount();
    bool getIsLocked();

    ~RegisteredUser();
};

#endif