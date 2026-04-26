#include "Transfer.h"
#include <iostream>
using namespace std;
Transfer::Transfer(double amount, string toAccNo, Account *target) : Transaction("Transfer", amount)
{
    this->toAccountNumber = toAccNo;
    this->targetAccount = target;
}

void Transfer::execute(Account *acc)
{
    if (this->targetAccount == nullptr)
    {
        cout << "Transfer Error: RECIPIENT OBJ IS NULL" << endl;
        return;
    }
    bool success = acc->transfer(this->targetAccount, this->amount);

    if (success)
    {
        cout << "Transfer to account " << this->toAccountNumber << " completed." << endl;
    }
    else
    {
        cout << "TRANSFER FAILED : Please check balance or account details" << endl;
    }
}

void Transfer::printReceipt()
{
    cout << "------------------------------------------------------------" << endl;
    cout << "                          TRANSFER RECEIPT                  " << endl;
    cout << "Type:      " << this->type << endl;
    cout << "Recipient: " << this->toAccountNumber << endl;
    cout << "Amount:    " << this->amount << endl;
    cout << "Timestamp: " << this->timestamp << endl;
    cout << "------------------------------" << endl;
}

Transfer::~Transfer() {}
