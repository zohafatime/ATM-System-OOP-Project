#include "Admin.h"
#include "User.h"
#include "ATM.h"
#include <iostream>
#include <cstring>
#include <fstream>

Admin::Admin(string n, string acc, string p) : User(n, acc, p) {}
bool Admin::verifyPin(string input)
{
    if (pin == input)
        return true;
    else
        return false;
}
void Admin::showMenu()
{
    cout << endl;
    cout << "--- Admin Menu ---" << endl;
    cout << "1. Add Account" << endl;
    cout << "2. Remove Account" << endl;
    cout << "3. View All Transactions" << endl;
    cout << "4. Reset User PIN" << endl;
    cout << "5. Add New Admin" << endl;
    cout << "0. Exit" << endl;
}
string Admin::getRole()
{
    return "Admin";
}
void Admin::addAccount(Account **&accounts, int &count, Account *newAcc)
{
    Account **temp = new Account *[count + 1];

    for (int i = 0; i < count; i++)
    {
        temp[i] = accounts[i];
    }

    temp[count] = newAcc;

    delete[] accounts;
    accounts = temp;
    count++;
    cout << "Account added successfully" << endl;
}
void Admin::removeAccount(Account **&accounts, int &count, string accNo)
{
    int index = -1;
    for (int i = 0; i < count; i++)
    {
        if (accounts[i]->getAccountNumber() == accNo)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        cout << "Account not found" << endl;
        return;
    }
    ifstream infile("account_record.txt");
    ofstream temp_f("temp.txt");
    string line;
    while(getline(infile,line)){
        if(line.substr(0,accNo.length())!=accNo)//skips the lines not starting with desired account
        temp_f<<line<<"\n";
    }
    infile.close();
    temp_f.close();
    remove("account_record.txt");
    rename("temp.txt","account_record.txt");
    delete accounts[index];
    // left shift
    for (int i = index; i < count - 1; i++)
    {
        accounts[i] = accounts[i + 1];
    }
    count--;
    cout << "Account removed successfully" << endl;
}
void Admin::viewAllTransactions()
{
    ifstream file("transactions.txt");

    if (!file)
    {
        cout << "No transaction record found" << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        cout << line << endl;
    }

    file.close();
}
void Admin::resetUserPin(Account **accounts, int count, string accNo, string newPin)
{
    for (int i = 0; i < count; i++)
    {
        if (accounts[i]->getAccountNumber() == accNo)
        {
            accounts[i]->setPin(newPin);
            accounts[i]->saveToFile();

            cout << "PIN reset successfully" << endl;
            return;
        }
    }
    cout << "Account not found" << endl;
}
Admin::~Admin() {}
