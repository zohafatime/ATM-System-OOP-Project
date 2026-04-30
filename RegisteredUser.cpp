#include "RegisteredUser.h"

RegisteredUser::RegisteredUser(string n, string acc, string p, Account *a)
    : User(n, acc, p)
{
    account = a;
}
void RegisteredUser::showMenu()
{
    cout << endl;
    cout << "--- Account Menu ---" << endl;
    cout << "1. Check balance" << endl;
    cout << "2. Deposit" << endl;
    cout << "3. Withdraw" << endl;
    cout << "4. Transfer" << endl;
    cout << "5. Mini statement" << endl;
    cout << "6. Change PIN" << endl;
    cout << "0. Logout" << endl;
}
string RegisteredUser::getRole()
{
    return "RegisteredUser";
}
Account *RegisteredUser::getAccount()
{
    return account;
}
bool RegisteredUser::getIsLocked()
{
    if (account != nullptr)
    {
        bool ch = account->getIsActive();
        if (ch == false)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}
void RegisteredUser::changePin(string oldPin, string newPin)
{
    if (oldPin != pin)
    {
        cout << "Invalid pin enter again" << endl;
    }
    else
    {
        pin = newPin;
        if (account != nullptr)
        {   account->setPin(newPin);
            account->saveToFile();
        }
         cout << "PIN changed successfully" << endl;
    }
}
RegisteredUser::~RegisteredUser() {}
