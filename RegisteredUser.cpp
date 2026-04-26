#include "RegisteredUser.h"

RegisteredUser::RegisteredUser(string n, string acc, string p, Account* a) 
    : User(n, p) {
    account = a;
    failedAttempts = 0;
    isLocked = false;
}
bool RegisteredUser::verifyPin(string input) {
    if (isLocked) {
        cout << "Access denied as account is currently locked" << endl;
        return false;
    }

    if (User::verifyPin(input)) {
        failedAttempts = 0; 
        return true;
    } else {
        failedAttempts++;
        cout << "Incorrect PIN. Attempts remaining are " << (3 - failedAttempts) << endl;

        if (failedAttempts >= 3) {
            lockAccount();
        }
        return false;
    }
}
void RegisteredUser::changePin(string oldPin,string newPin){
    if(oldPin!=pin){
        cout<<"Invalid pin enter again"<<endl;
    }
    else{
        pin=newPin;
        cout<<"PIN changed successfully"<<endl;
        if (account != nullptr) {
            account->saveToFile(); 
        }
    }
}
RegisteredUser::~RegisteredUser(){}
