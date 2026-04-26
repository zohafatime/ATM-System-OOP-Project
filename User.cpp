#include "User.h"
#include <iostream>
#include <cstring>

User::User(string n, string acc, string p)
{
    name = n;
    accountNumber = acc;
    pin = p;
}
string User::getName()
{
    return name;
}
string User::getAccountNumber()
{
    return accountNumber;
}
User::~User() {}
