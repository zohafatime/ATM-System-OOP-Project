#include "ATM.h"
#include "Admin.h"
#include "Deposit.h"
#include "Withdraw.h"
ATM::ATM()
{
    currentAccount = nullptr;
    accounts = nullptr;
    count = 0;
    cashAvailable = 0;
    admins = nullptr;
    adminCount = 0;
}
ATM::~ATM()
{
    for (int i = 0; i < count; i++)
    {
        delete *(accounts + i);
    }
    delete[] accounts;
    for (int i = 0; i < adminCount; i++)
    {
        delete *(admins + i);
    }
    delete[] admins;
    if (logFile.is_open())
    {
        logFile.close();
    }
}
void ATM::run()
{
    loadAccounts();
    loadAdmins();
    checkFirstRun();

    while (true)
    {
        cout << "     WELCOME TO ATM BANKING SYSTEM     " << endl;
        cout << "1. Login as User" << endl;
        cout << "2. Login as Admin" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        if (choice == 1)
        {
            runUserMenu();
        }
        else if (choice == 2)
        {
            runAdminMenu();
        }
        else if (choice == 3)
        {
            cout << "Thank you for using our ATM" << endl;
            break;
        }
        else
        {
            cout << "Invalid choice" << endl;
        }
    }
}
void ATM::runUserMenu()
{
    loadAccounts();
    while (true)
    {
        cout << "     USER PANEL     " << endl
             << "Command List" << endl
             << "1 to Sign In (Existing User)" << endl
             << "2 to Sign Up for New User" << endl
             << "3 to Exit" << endl
             << "Enter your choice: ";
        int choice;
        cin >> choice;
        if (choice == 1)
        {
            string acc, pin;
            cout << "ACCOUNT LOGIN" << endl
                 << "Enter account number" << endl;
            cin >> acc;
            cout << "Enter PIN" << endl;
            cin >> pin;
            bool ch = authenticate(acc, pin);
            if (ch == 0)
            {
                cout << "Invalid Credentials please enter again" << endl;
                continue;
            }
            else
            {
                cout << "Login Successful" << endl
                     << "Welcome " << currentAccount->getHolderName() << endl;
            }
            while (ch == 1)
            {
                showMenu();
                int choice2;
                cout << "Enter choice" << endl;
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

            cout << "NEW ACCOUNT REGISTRATION" << endl
                 << "Enter Account Number " << endl;
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

            if (check == 1)
            {
                cout << "Account number already exists" << endl;
                continue;
            }
            else
            {
                cout << "Enter Account Holder Name " << endl;
                cin.ignore(); // to prevent the buffer issue
                getline(cin, holderName);
                while (true)
                {
                    cout << "Enter 4-digit PIN" << endl;
                    cin >> newPin;
                    int len = 0;
                    bool test = true;
                    while (newPin[len] != '\0')
                    {
                        if (newPin[len] < '0' || newPin[len] > '9')
                        {
                            test = false;
                        }
                        len++;
                    }

                    if (len == 4 && test == 1)
                    {
                        break;
                    }
                    else
                    {
                        cout << "PIN must be exactly 4 digits" << endl
                             << "Please enter again" << endl;
                    }
                }
                while (true)
                {
                    cout << "Confirm PIN " << endl;
                    cin >> Pin2;
                    if (newPin != Pin2)
                    {
                        cout << "PINs do not match Please enter again" << endl;
                    }
                    else
                    {
                        break;
                    }
                }
                while (true)
                {
                    cout << "Enter Initial Deposit" << endl;
                    cin >> initial;
                    if (initial < 0)
                    {
                        cout << "Invalid amount enter again" << endl;
                    }
                    else
                    {
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

            cout << "New Account created successfully" << endl
                 << "Account Number = " << accNo << endl
                 << " and Holder name is " << holderName << endl
                 << " with Initial Balance = " << initial << endl;
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
void ATM::runAdminMenu()
{
    cout << "    ADMIN LOGIN    " << endl;
    string acc, pin;
    cout << "Enter Admin ID: ";
    cin >> acc;
    cout << "Enter PIN: ";
    cin >> pin;

    Admin *currentAdmin = nullptr;
    for (int i = 0; i < adminCount; i++)
    {
        if ((*(admins + i))->getAccountNumber() == acc && (*(admins + i))->verifyPin(pin))
        {
            currentAdmin = *(admins + i);
            break;
        }
    }

    if (currentAdmin == nullptr)
    {
        cout << "Invalid admin credentials" << endl;
        return;
    }

    cout << "Admin login successful. Welcome " << currentAdmin->getName() << endl;

    while (true)
    {
        currentAdmin->showMenu();
        int choice;
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 0)
            break;
        else if (choice == 1)
        {
            double amt;
            cout << "Enter amount to refill: ";
            cin >> amt;
            currentAdmin->refillATMCash(*this, amt);
        }
        else if (choice == 2)
        {
            string accNo, name, pin2;
            double bal;
            cout << "Enter Account Number: ";
            cin >> accNo;
            cout << "Enter Holder Name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter PIN: ";
            cin >> pin2;
            cout << "Enter Initial Balance: ";
            cin >> bal;
            Account *newAcc = new Account(accNo, name, bal);
            newAcc->setPin(pin2);
            newAcc->saveToFile();
            currentAdmin->addAccount(accounts, count, newAcc);
        }
        else if (choice == 3)
        {
            string accNo;
            cout << "Enter Account Number to remove: ";
            cin >> accNo;
            currentAdmin->removeAccount(accounts, count, accNo);
        }
        else if (choice == 4)
        {
            currentAdmin->viewAllTransactions();
        }
        else if (choice == 5)
        {
            string accNo, newPin;
            cout << "Enter Account Number: ";
            cin >> accNo;
            cout << "Enter New PIN: ";
            cin >> newPin;
            currentAdmin->resetUserPin(accounts, count, accNo, newPin);
        }
        else if (choice == 6)
        {
            string id, name, pin, pin2;
            cout << "Enter New Admin ID: ";
            cin >> id;
            cout << "Enter Name: ";
            cin.ignore();
            getline(cin, name);

            while (true)
            {
                cout << "Enter 4-digit PIN: ";
                cin >> pin;
                int len = 0;
                bool valid = true;
                while (pin[len] != '\0')
                {
                    if (pin[len] < '0' || pin[len] > '9')
                        valid = false;
                    len++;
                }
                if (len == 4 && valid)
                    break;
                cout << "PIN must be exactly 4 digits" << endl;
            }
            while (true)
            {
                cout << "Confirm PIN: ";
                cin >> pin2;
                if (pin == pin2)
                    break;
                cout << "PINs do not match" << endl;
            }

            ofstream outfile("admin_record.txt", ios::app);
            outfile << id << " " << name << " " << pin << endl;
            outfile.close();

            Admin **temp = new Admin *[adminCount + 1];
            for (int i = 0; i < adminCount; i++)
                *(temp + i) = *(admins + i);
            *(temp + adminCount) = new Admin(name, id, pin);
            delete[] admins;
            admins = temp;
            adminCount++;

            cout << "New admin added successfully" << endl;
        }
        else
        {
            cout << "Invalid choice" << endl;
        }
    }
    cout << "Admin logged out" << endl;
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
    else if (choice == 6)
    {
        string newPin;
        string Pin2;
        while (true)
        {
            cout << "Enter 4-digit PIN" << endl;
            cin >> newPin;
            int len = 0;
            bool test = true;
            while (newPin[len] != '\0')
            {
                if (newPin[len] < '0' || newPin[len] > '9')
                {
                    test = false;
                }
                len++;
            }

            if (len == 4 && test == 1)
            {
                break;
            }
            else
            {
                cout << "PIN must be exactly 4 digits" << endl
                     << "Please enter again" << endl;
            }
        }
        while (true)
        {
            cout << "Confirm PIN " << endl;
            cin >> Pin2;
            if (newPin != Pin2)
            {
                cout << "PINs do not match Please enter again" << endl;
            }
            else
            {
                break;
            }
        }
        currentAccount->setPin(newPin);
        currentAccount->saveToFile();
        cout << "PIN changed successfully" << endl;
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

        infile >> t_pin >> bal >> t_count;

        Account **temp = new Account *[count + 1];

        for (int i = 0; i < count; i++)
        {
            *(temp + i) = *(accounts + i);
        }
        Account *new_acc = new Account(id, "User", bal);
        new_acc->setPin(t_pin);
        *(temp + count) = new_acc;

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
void ATM::showMenu()
{
    cout << "1. Check Balance" << endl;
    cout << "2. Deposit" << endl;
    cout << "3. Withdraw" << endl;
    cout << "4. Transfer" << endl;
    cout << "5. Mini Statement" << endl;
    cout << "6. Change Pin" << endl;
    cout << "-1. Logout" << endl;
}
void ATM::checkFirstRun()
{
    ifstream infile("admin_record.txt");
    bool hasAdmin = false;

    if (infile)
    {
        string test;
        infile >> test;
        if (test != "")
            hasAdmin = true;
        infile.close();
    }

    if (!hasAdmin)
    {
        cout << "     FIRST TIME SETUP       " << endl;
        cout << "No admin found. Create the first admin account" << endl;

        string id, name, pin, pin2;
        cout << "Enter Admin ID: ";
        cin >> id;
        cout << "Enter Admin Name: ";
        cin.ignore();
        getline(cin, name);

        while (true)
        {
            cout << "Enter 4-digit PIN: ";
            cin >> pin;
            int len = 0;
            bool valid = true;
            while (pin[len] != '\0')
            {
                if (pin[len] < '0' || pin[len] > '9')
                    valid = false;
                len++;
            }
            if (len == 4 && valid)
                break;
            cout << "PIN must be exactly 4 digits" << endl;
        }
        while (true)
        {
            cout << "Confirm PIN: ";
            cin >> pin2;
            if (pin == pin2)
                break;
            cout << "PINs do not match" << endl;
        }

        ofstream outfile("admin_record.txt");
        outfile << id << " " << name << " " << pin << endl;
        outfile.close();

        cout << "First admin created successfully." << endl;
        loadAdmins(); // reload after creation
    }
}
void ATM::loadAdmins()
{
    ifstream infile("admin_record.txt");
    if (!infile) return;

    adminCount = 0;
    admins = nullptr;

    string id, name, pin;
    while (infile >> id >> name >> pin)
    {
        Admin **temp = new Admin *[adminCount + 1];
        for (int i = 0; i < adminCount; i++)
            temp[i] = admins[i];

        temp[adminCount] = new Admin(name, id, pin);
        delete[] admins;
        admins = temp;
        adminCount++;
    }
    infile.close();
}