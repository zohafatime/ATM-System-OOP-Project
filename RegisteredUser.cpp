#include "RegisteredUser.h"

RegisteredUser::RegisteredUser(string n, string acc, string p, Account *a)
    : User(n, acc, p)
{
    account = a;
    failedAttempts = 0;
    isLocked = false;
}
bool RegisteredUser::verifyPin(string input)
{
    if (isLocked)
    {
        cout << "Access denied as account is currently locked" << endl;
        return false;
    }

    if (pin == input)
    {
        failedAttempts = 0;
        return true;
    }
    else
    {
        failedAttempts++;
        cout << "Incorrect PIN. Attempts remaining are " << (3 - failedAttempts) << endl;

        if (failedAttempts >= 3)
        {
            lockAccount();
        }
        return false;
    }
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
    return isLocked;
}
void RegisteredUser::lockAccount()
{
    isLocked = true;
    cout << "Account locked due to too many failed attempts" << endl;
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
        cout << "PIN changed successfully" << endl;
        if (account != nullptr)
        {
            account->saveToFile();
        }
    }
}
RegisteredUser::~RegisteredUser() {}
