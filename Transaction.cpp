#include "Transaction.h"
#include <iostream>
#include <cstring>
#include <ctime>

Transaction::Transaction(string t, double amt)
{
    type = t;
    amount = amt;
    time_t now = time(0);//gets the current time from laptop and stores in time_t
    timestamp = string(ctime(&now));
    if (!timestamp.empty() && timestamp.back() == '\n')
        timestamp.pop_back(); // stripping a new line if any
}
double Transaction::getAmount()
{
    return amount;
}
string Transaction::getType()
{
    return type;
}
string Transaction::getTimestamp()
{
    return timestamp;
}
Transaction::~Transaction() {}
void LogEntry::execute(Account* acc){}
void LogEntry::printReceipt() {
    cout << type << " | " << amount << " | " << timestamp << endl;
}