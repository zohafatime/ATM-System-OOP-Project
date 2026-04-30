#include "ATM.h"
#include "Deposit.h"    
#include "Withdraw.h"   
ATM::ATM()
{
    currentAccount = nullptr;
    accounts = nullptr;
    count = 0;
    cashAvailable = 0;
}
ATM::~ATM()
{
    for (int i = 0; i < count; i++)
    {
        delete *(accounts + i);
    }
    delete[] accounts;
    if (logFile.is_open())
    {
        logFile.close();
    }
}
void ATM::run()
{
    loadAccounts();
    while (true)
    {
        cout << "     WELCOME TO ATM BANKING SYSTEM     " << endl<<"Command List"<<endl<< "1 to Sign In (Existing User)" <<endl<< "2 to Sign Up for New User" << endl<< "3 to Exit" << endl << "Enter your choice";
        int choice;
        cin >> choice;
        if (choice == 1)
        {
            string acc, pin;
            cout << "ACCOUNT LOGIN" << endl<< "Enter account number"<<endl;
            cin >> acc;
            cout << "Enter PIN"<<endl;
            cin >> pin;
            bool ch = authenticate(acc, pin);
            if (ch == 0)
            {
                cout << "Invalid Credentials please enter again" << endl;
                continue;
            }
            else{
            cout << "Login Successful" << endl<< "Welcome " << currentAccount->getHolderName()<< endl;
            }
            while (ch == 1)
            {
                showMenu();
                int choice2;
                cout << "Enter choice"<<endl;
                cin >> choice2;
                
                if (choice2 == -1)
                {
                    ch = 0;
                    currentAccount = nullptr;
                    cout << "Logged out successfully" << endl;
                }
                else
                {
                    processTransaction(choice2);
                }
            }
        }
        else if (choice == 2)
        {
            string accNo, holderName, newPin, Pin2;
            double initial;
            
            cout << "NEW ACCOUNT REGISTRATION" << endl<< "Enter Account Number "<<endl;
            cin >> accNo;
            bool check = false;
            for (int i = 0; i < count; i++)
            {
                if ((*(accounts + i))->getAccountNumber() == accNo)
                {
                    check = true;
                    break;
                }
            }
            
            if (check==1)
            {
                cout << "Account number already exists" << endl;
                continue;
            }
            else{
            cout << "Enter Account Holder Name "<<endl;
            cin.ignore();     //to prevent the buffer issue
            getline(cin, holderName);
            while (true)
            {
                cout << "Enter 4-digit PIN"<<endl;
                cin >> newPin;
                int len = 0;
                while (newPin[len] != '\0') { 
                    len++; }

                if (len == 4) 
                {
                    break;
                }
                else{
                cout << "PIN must be exactly 4 digits" << endl<<"Please enter again"<<endl;
                }
            }
            while(true){
            cout << "Confirm PIN "<<endl;
            cin >> Pin2;
            if (newPin != Pin2)
            {
                cout << "PINs do not match Please enter again" << endl;
            }
            else{
                break;
            }
        }
        while(true){
            cout << "Enter Initial Deposit"<<endl;
            cin >> initial;
            if (initial < 0)
            {
                cout << "Invalid amount enter again" << endl;
            }
            else{
                break;
            }
        }
    }
            Account **temp = new Account *[count + 1];
            for (int i = 0; i < count; i++)
            {
                *(temp + i) = *(accounts + i);
            }
            
            *(temp + count) = new Account(accNo, holderName, initial);
            (*(temp + count))->setPin(newPin);
            (*(temp + count))->saveToFile();
            
            delete[] accounts;
            accounts = temp;
            count++;
            
            cout << "New Account created successfully" << endl<< "Account Number = " << accNo << endl<< " and Holder name is " << holderName << endl<< " with Initial Balance = " << initial << endl;
        }
        else if (choice == 3)
        {
            cout << "Thank you for using our ATM" << endl;
            break;
        }
        else
        {
            cout << "Invalid choice Please enter again" << endl;
        }
    }
    }
bool ATM::authenticate(string accNo, string pin)
{
    currentAccount = nullptr;
    for (int i = 0; i < count; i++)
    {
        if (*(accounts + i) != nullptr)
        {
            string temp = (*(accounts + i))->getAccountNumber();
            bool ch = (*(accounts + i))->verifyPin(pin);
            if (ch == 1 && accNo == temp)
            {
                currentAccount = (*(accounts + i));
                return true;
            }
        }
    }
    return false;
}
void ATM::processTransaction(int choice)
{
    if (choice == 1)
    {
        cout << "Current Balance is " << currentAccount->getBalance() << endl;
    }
    else if (choice == 2)
    {
        double amount;
        cout << "Enter amount to deposit " << endl;
        cin >> amount;

        currentAccount->deposit(amount);
        logTransaction(currentAccount, new Deposit(amount));
        currentAccount->saveToFile();
    }
    else if (choice == 3)
    {
        double amount;
        cout << "Enter amount to withdraw " << endl;
        cin >> amount;

        currentAccount->withdraw(amount);
        logTransaction(currentAccount, new Withdraw(amount));
        currentAccount->saveToFile();
    }
    else if (choice == 4)
    {
        string target;
        cout << "Enter destination account Number " << endl;
        cin >> target;
        Account *destination = nullptr;
        for (int i = 0; i < count; i++)
        {
            if ((*(accounts + i))->getAccountNumber() == target)
            {
                destination = *(accounts + i);
                break;
            }
        }

        if (destination == nullptr)
        {
            cout << "Destination account not found" << endl;
        }
        else if (currentAccount == destination)
        {
            cout << "cannot transfer to yourself" << endl;
        }
        else
        {
            double amount;
            cout << "Enter amount to transfer " << endl;
            cin >> amount;
            currentAccount->transfer(destination, amount);
            logTransaction(currentAccount, new Withdraw(amount));
            logTransaction(destination, new Deposit(amount));

            currentAccount->saveToFile();
            destination->saveToFile();
        }
    }
    else if (choice == 5)
    {
        cout << "Printing mini statement" << endl;
        currentAccount->printMiniStatement();
    }
    else
    {
        cout << "Invalid choice" << endl;
    }
}
void ATM::saveAccounts()
{
    for (int i = 0; i < count; i++)
    {
        (*(accounts + i))->saveToFile();
    }
}
void ATM::logTransaction(Account *source, Transaction *t)
{
    source->addTransaction(t);
}
void ATM::loadAccounts()
{
    ifstream infile("account_record.txt");
    if (!infile)
    {
        return;
    }

    count = 0;
    accounts = nullptr;

    while (!infile.eof())
    {
        string id;
        string t_pin;
        float bal;
        int t_count;

        infile >> id;
        if (id == "")
        {
            break;
        }

        infile >>t_pin>> bal >> t_count;

        Account **temp = new Account *[count + 1];

        for (int i = 0; i < count; i++)
        {
            *(temp + i) = *(accounts + i);
        }
    Account *new_acc= new Account(id, "User", bal);
    new_acc->setPin(t_pin);
    *(temp+count)=new_acc;

        for (int j = 0; j < t_count; j++)
        {
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
void ATM::refillCash(double amount)
{
    if (amount < 0)
    {
        cout << "Invalid amount" << endl;
        return;
    }
    else
    {
        cashAvailable = cashAvailable + amount;
    }
}
double ATM::getCashAvailable()
{
    return cashAvailable;
}
void ATM::showMenu() {
    cout << "1. Check Balance" << endl;
    cout << "2. Deposit" << endl;
    cout << "3. Withdraw" << endl;
    cout << "4. Transfer" << endl;
    cout << "5. Mini Statement" << endl;
    cout << "-1. Logout" << endl;
}