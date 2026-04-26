#include"Withdraw.h"
#include<iostream>
using namespace std;
Withdraw::Withdraw(double amt, double limitValue) : Transaction("Withdraw", amt)
{
	this->limit = limitValue;
}
void Withdraw::execute(Account* acc)
{
	if (this->amount > this->limit)
	{
		cout << "Error: TRANSACTION EXCEED WITHDRAW LIMIT OF " << this->limit << endl;
		retrun;
	}
	bool success = acc->withdraw(this->amount);
	if (succes)
	{
		cout << "WITHDRAWAL IS EXECUTED SUCCESSFULLY." << endl;
	}
	else
	{
		cout << "WITHDRAWAL FAILED: insufficient account balance" << endl;
	}
	
}
void Withdraw::printReceipt()
{
	cout << "---------------------------------------------------------------------" << endl;
	cout << "                           WITHDRAWAL RECEIPT                       " << endl;
	cout << "TYPE:       " << this->type << endl;
	cout << "ACCOUNT:    " << this->amount << endl;
	cout << "TIMESTAMP:  " << this->timestamp << endl;
	cout << "---------------------------------------------------------------------" << endl;

}
Withdraw::~Withdraw()
{ }
