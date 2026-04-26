#include "ATM.h"

ATM::ATM(){
    currentAccount=nullptr;
    accounts=nullptr;
    count=0;
    cashAvailable=0;
}
ATM::~ATM(){
    for (int i = 0; i < count; i++) {
        delete *(accounts+i); 
    }
    delete[] accounts; 
    if (logFile.is_open()) {
        logFile.close();
    }
}
void ATM::run(){
    loadAccounts();
    while(true){
        string acc;
        cout<<"Welcome to ATM"<<endl<<"Enter account number"<<endl;
        cin>>acc;
        string pin;
        cout<<"Enter Pin"<<endl;
        cin>>pin;
        bool ch=authenticate(acc,pin);
        if(ch==0){
            cout<<"Invalid Credentials"<<endl;
            continue;
        }
         while(ch==1){
        showMenu();
       int choice;
       cout<<"Enter choice(-1 to logout and 1 for transaction)"<<endl;
       cin>>choice;
       if(choice==-1){
        ch=0;
        currentAccount=nullptr;
       }
       else{
       processTransaction(choice);
       }
}}
}
bool ATM::authenticate(string accNo, string pin){
    currentAccount=nullptr;
    for(int i=0;i<count;i++){
        if(*(accounts+i)!=nullptr){
            string temp=(*(accounts+i))->getAccountNumber();
            bool ch=(*(accounts+i))->verifyPin(pin);
            if(ch==1&&accNo==temp){
                currentAccount=(*(accounts+i));
                return true;
            }

        }
    }
    return false;
}
void ATM::processTransaction(int choice){
    if(choice==1){
        cout << "Current Balance is " << currentAccount->getBalance() << endl;
    }
    else if(choice==2){
        double amount;
        cout << "Enter amount to deposit "<<endl;
        cin >> amount;
        
        currentAccount->deposit(amount);
        logTransaction(currentAccount, new Deposit(amount));
        currentAccount->saveToFile();
    }
    else if(choice==3){
         double amount;
        cout << "Enter amount to withdraw "<<endl;
        cin >> amount;
        
        currentAccount->withdraw(amount);
        logTransaction(currentAccount, new Withdraw(amount));
        currentAccount->saveToFile();
          }
    else if(choice==4){
        string target;
        cout << "Enter destination account Number "<<endl;
        cin >> target;
        Account* destination = nullptr;
        for (int i = 0; i < count; i++) {
            if ((*(accounts + i))->getAccountNumber() == target) {
                destination = *(accounts + i);
                break;
            }
        }

        if (destination == nullptr) {
            cout << "Destination account not found" << endl;
        }
        else if (currentAccount == destination) {
            cout << "cannot transfer to yourself" << endl;
        }
        else {
            double amount;
            cout << "Enter amount to transfer "<<endl;
            cin >> amount;
            currentAccount->transfer(destination, amount);
            logTransaction(currentAccount, new Withdraw(amount));
            logTransaction(destination, new Deposit(amount));
            
            currentAccount->saveToFile();
            destination->saveToFile();
        }
    }
    else if(choice==5){
            cout<<"Printing mini statement"<<endl;
            currentAccount->printMiniStatement();
     
}else{
    cout<<"Invalid choice"<<endl;
}
}
void ATM::saveAccounts() {
    for (int i = 0; i < count; i++) {
        (*(accounts + i))->saveToFile(); 
    }
}
void ATM::logTransaction(Account* source,Transaction *t){
source->addTransaction(t);

}
void ATM::loadAccounts() {
    ifstream infile("account_record.txt");
    if (!infile) {
        return;
    }

    count = 0;
    accounts = nullptr;

    while (!infile.eof()) {
        string id;
        float bal;
        int t_count;

        infile >> id;
        if (id == "") {
            break;
        }

        infile >> bal >> t_count;

        Account** temp = new Account*[count + 1];

        for (int i = 0; i < count; i++) {
            *(temp+i) = *(accounts+i);
        }

        *(temp+count) = new Account(id, "User", bal, nullptr);
        
        for (int j = 0; j < t_count; j++) {
            float dummy;
            infile >> dummy;
        }

        delete[] accounts;
        accounts = temp;
        
        accounts[count]->loadFromFile();

        count++;
    }
    infile.close();
}
void ATM::refillCash(double amount){
    if(amount<0){
        cout<<"Invalid amount"<<endl;
        return;
    }
    else{
        cashAvailable=cashAvailable+amount;
    }
}
double ATM::getCashAvailable(){
    return cashAvailable;
}