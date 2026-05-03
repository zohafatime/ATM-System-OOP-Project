#include "Account.h"
#include "Transaction.h"
#include <iostream>
#include <fstream>

// helper
string encryptPin(const string &pin)
{
    string result = pin;
    for (char &c : result)
        c = c ^ 0x5A; // XOR key 0x5A
    return result;
}
string decryptPin(const string &pin)
{
    return encryptPin(pin); // XOR is symmetric
}

Account::Account(string accNo, string holderName, int initialBalance)
{
    accountNumber = accNo;
    this->holdername = holderName;
    balance = initialBalance;
    transactionCount = 0;
    failedAttempts = 0;
    isLocked = false;
    transactions = nullptr;
}
Account::~Account()
{
    delete[] transactions;
}
bool Account::deposit(int amount)
{
    if (amount <= 0)
        return false;
    if (balance + amount > 500000)
    {
        cout << "Error: Deposit would exceed 5lac limit" << endl;
        return false;
    }
    balance = balance + amount;
    addTransaction(new LogEntry("Deposit", amount));
    saveToFile();
    return true;
}
bool Account::withdraw(int amount)
{
    if (amount <= 0 || amount > balance)
    {
        cout << "Invalid amount" << endl;
        return false;
    }
    if (amount <= balance)
    {
        balance = balance - amount;
        addTransaction(new LogEntry("Withdraw", -amount));
        saveToFile();
        return true;
    }
    return false;
}
bool Account::transfer(Account *target, int amount)
{
    if (amount <= 0 || amount > balance)
    {
        cout << "Invalid amount cant work on it" << endl;
        return false;
    }
    if (accountNumber == target->accountNumber)
    {
        cout << "Invalid number cant transfer" << endl;
        return false;
    }
    if (target->balance + amount > 500000)
        return false;
    balance = balance - amount;
    target->balance = target->balance + amount;
    addTransaction(new LogEntry("Deducted", -amount));
    target->addTransaction(new LogEntry("Received", amount));
    saveToFile();
    target->saveToFile();
    cout << "Transaction Successful and money Transferred" << endl;
    return true;
}
int Account::getBalance()
{
    return balance;
}
string Account::getAccountNumber()
{
    return accountNumber;
}
string Account::getHolderName()
{
    return holdername;
}
//--
bool Account::getIsActive()
{
    return !isLocked;
}
void Account::lockAccount()
{
    isLocked = true;
    failedAttempts = 0;
    saveToFile();
}
void Account::unlockAccount()
{
    isLocked = false;
    failedAttempts = 0;
    saveToFile();
}
//--
void Account::printMiniStatement()
{
    for (int i = 0; i < transactionCount; i++)
    {
        if (*(transactions + i) != nullptr)
        {
            (*(transactions + i))->printReceipt();
        }
    }
}
void Account::addTransaction(Transaction *t)
{
    Transaction **temp = new Transaction *[transactionCount + 1];

    for (int i = 0; i < transactionCount; i++)
    {
        *(temp + i) = *(transactions + i);
    }

    *(temp + transactionCount) = t;
    delete[] transactions;
    transactions = temp;
    transactionCount++;
}
void Account::saveToFile()
{
    ifstream infile("account_record.txt");
    ofstream temp_f("temp.txt");

    string id;
    if (infile.is_open())
    {
        while (!infile.eof())
        {
            infile >> id;
            if (!infile)
            {
                break;
            }
            if (id != accountNumber)
            {
                string t_pin, t_name;
                int t_bal;
                bool t_lock;
                int count2;
                infile >> t_pin >> t_bal >> t_lock >> t_name >> count2;

                temp_f << id << " " << t_pin << " " << t_bal << " " << t_lock << " " << t_name << " " << count2 << " ";
                for (int i = 0; i < count2; i++)
                {
                    float trashAmt;
                    infile >> trashAmt;
                    temp_f << trashAmt << " ";
                }
                temp_f << endl;
            }
            else
            {
                char ch;
                while (infile.get(ch) && ch != '\n')
                    ;
            }
        }
        infile.close();
    }

    temp_f << accountNumber << " " << encryptPin(pin) << " " << balance << " " << isLocked << " " << holdername << " " << transactionCount << " ";
    for (int i = 0; i < transactionCount; i++)
    {
        temp_f << (*(transactions + i))->getAmount() << " ";
    }
    temp_f << endl;
    temp_f.close();
    remove("account_record.txt");
    rename("temp.txt", "account_record.txt");
}
void Account::loadFromFile()
{
    ifstream infile("account_record.txt");
    if (!infile)
    {
        return;
    }
    for (int i = 0; i < transactionCount; i++)
    {
        delete transactions[i];
    }
    delete[] transactions;
    transactions = nullptr;
    transactionCount = 0;
    while (!infile.eof())
    {
        string id;
        string t_pin, t_name;
        infile >> id;

        if (id == "")
        {
            break;
        }
        if (id == accountNumber)
        {
            int tempcount;
            infile >> t_pin;
            this->pin = decryptPin(t_pin);
            infile >> balance;
            infile >> isLocked;
            infile >> t_name;
            this->holdername = t_name;
            infile >> tempcount;

            for (int i = 0; i < tempcount; i++)
            {
                int amt;
                infile >> amt;
                addTransaction(new LogEntry("History", amt));
            }
            break;
        }
        else
        {
            char ch;
            while (infile.get(ch) && ch != '\n')
                ;
        }
    }
    infile.close();
}
void Account::showBalance()
{
    cout << "Current balance is " << balance << endl;
}
void Account::setPin(string newPin)
{
    this->pin = newPin;
}
bool Account::verifyPin(string input)
{
    if (input.length() != 4)
        return false;
    for (char c : input) // range based for loop
    {
        if (c < '0' || c > '9')
            return false;
    }
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
            isLocked = true;
            cout << "Account has been locked due to 3 failed attempts" << endl;
            saveToFile();
        }
        return false;
    }
}

void Account::resetLock() // admin uses this to unlock
{
    isLocked = false;
    failedAttempts = 0;
    saveToFile();
}