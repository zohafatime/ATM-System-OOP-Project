#ifndef WITHDRAW_H
#define WITHDRAW_H
#include"Transaction.h"
#include"Account.h"
#include <cstring>
using namespace std;
class Withdraw :public Transaction
{
private:
	double limit;
public:
	Withdraw(double amt, double limitValue = 25000.0);//amt is desired amount user wanna take out
	void execute(Account* acc) override;
	void printReceipt() override;
	~Withdraw();

};
#endif
