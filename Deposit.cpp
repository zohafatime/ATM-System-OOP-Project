#include "Deposit.h"
#include <iostream>
using namespace std;

Deposit::Deposit(double amount, string src) : Transaction("Deposit", amount) {
    this->source = src;
}
void Deposit::execute(Account* acc) {
    acc->deposit(this->amount);
    cout << "Deposit executed successfully" << endl;
}
void Deposit::printReceipt() {
    cout << "---------------------------------------------" << endl;
    cout << "               DEPOSIT RECEIPT               " << endl;
    cout << "Type:      " << this->type << endl;
    cout << "Amount:    " << this->amount << endl;
    cout << "Source:    " << this->source << endl;
    cout << "Timestamp: " << this->timestamp << endl;
    cout << "---------------------------------------------" << endl;
}
Deposit::~Deposit() {}