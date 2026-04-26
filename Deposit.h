#ifndef DEPOSIT_H
#define DEPOSIT_H
#include "Transaction.h"
#include "Account.h"
#include <string>
using namespace std;

class Deposit : public Transaction {
private:
    string source;
public:
    Deposit(double amount, string src = "Cash");
    void execute(Account* acc) override;
    void printReceipt() override;
    ~Deposit();
};
#endif