#ifndef TRANSFER_H
#define TRANSFER_H
#include"Transaction.h"
#include"Account.h"
#include <cstring>
using namespace std;
class Transfer :public Transaction
{
private:
	string toAccountNumber;
	Account* targetAccount;//aggre concept
public:
	Transfer(double amount, string toAccNo, Account* target);
	void execute(Account* acc) override;
	void printReceipt() override;
	~Transfer();
};
#endif
