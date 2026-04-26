#include "Account.h"
#include "Transaction.h" 
#include <iostream>      
#include <fstream>

    Account::Account(string accNo,string holderName, double initialBalance,User *u){
        accountNumber=accNo;
        this->holdername=holderName;
        balance=initialBalance;
        transactionCount=0;
        user=u;
    }
    Account::~Account(){
        delete [] transactions;
    }
    void Account::deposit(double amount){
        if(amount>0){
          balance=balance+amount;
          addTransaction(new Transaction("Deposit", amount));
          saveToFile();
        }
    }
    bool Account::withdraw(double amount){
        if(amount<=balance){
            balance=balance-amount;
            addTransaction(new Transaction("Withdraw", -amount));
            saveToFile();
            return true;
        }
        return false;
    }
    bool Account::transfer(Account* target, double amount){
        if(amount<=0||amount>balance){
            cout<<"Invalid amount cant work on it"<<endl;
            return false;
        }
        else if(accountNumber==target->accountNumber){
            cout<<"Invalid number cant transfer"<<endl;
            return false;
        }
        balance=balance-amount;
        target->balance=target->balance+amount;
        addTransaction(new Transaction("Deducted",-amount));
        target->addTransaction(new Transaction("Received",amount));
        saveToFile();
        target->saveToFile();
        cout<<"Transaction Successful and money Transferred"<<endl;
        return true;
    }
    double Account::getBalance(){
         return balance;
    }
    string Account::getAccountNumber(){
        return accountNumber;
    }
    string Account::getHolderName(){
        return holdername;
    }
    bool Account::getIsActive() {
    return status;
}

void Account::setIsActive(bool status) {
    this->status = status;
}
    void Account::printMiniStatement(){
        for(int i=0;i<transactionCount;i++){
            if(*(transactions+i)!=nullptr){
            (*(transactions+i))->printReceipt();
        }
    }
    }
    void Account::addTransaction(Transaction* t){
    Transaction** temp = new Transaction*[transactionCount + 1];

    for (int i = 0; i < transactionCount; i++) {
        *(temp + i) = *(transactions + i);
    }

    *(temp + transactionCount) = t;
    delete[] transactions;
    transactions = temp;
    transactionCount++;
    }
 void Account::saveToFile() {
    ifstream infile("account_record.txt");
    ofstream temp_f("temp.txt");
    
    string id;
    if (infile.is_open()) {
        while (!infile.eof()) {
            infile>>id;
            if(!infile){
                break;
            }
            if (id != accountNumber) {
                
                float t_bal;
                int count2;
                infile >> t_bal >> count2;
                temp_f << id << " " << t_bal << " " << count2 << " ";
                for (int i = 0; i < count2; i++) {
                    float trashAmt;
                    infile >> trashAmt;
                    temp_f << trashAmt << " ";
                }
                temp_f << endl;
            } 
            else {
                float t_bal;
                int count2;
                infile >> t_bal >> count2;
                for (int i = 0; i < count2; i++) {
                    float trashAmt;
                    infile >> trashAmt; 
                }
            }
        }
        infile.close();
    }

    temp_f << accountNumber << " " << balance << " " << transactionCount << " ";
    for (int i = 0; i < transactionCount; i++) {
        temp_f << (*(transactions + i))->getAmount() << " ";
    }
    temp_f << endl;
    temp_f.close();
    remove("account_record.txt");
    rename("temp.txt", "account_record.txt");
}
    void Account::loadFromFile(){
         ifstream infile("account_record.txt");
        if(!infile){
            return;
        }
        for (int i = 0; i < transactionCount; i++) {
        delete transactions[i];
    }
    delete[] transactions;
    transactions = nullptr;
    transactionCount = 0;
        while(!infile.eof()){
            string id;
            infile>>id;

            if(id==""){
                break;
            }
            if(id==accountNumber){
                int tempcount;
                infile>>balance;
                infile>>tempcount;
                for (int i = 0; i < tempcount; i++) {
                float amt;
                infile>>amt;
                addTransaction(new Transaction("History", amt));
            }
            break;
        }
        else{
            float t_bal;
            int count2;
            infile >> t_bal >> count2;
            for (int i = 0; i < count2; i++) {
                float trashAmt;
                infile >> trashAmt; 
            }
        }
    }
    infile.close();
    }
    void Account::showBalance(){
        cout<<"Current balance is "<<balance<<endl;
    }