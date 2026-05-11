#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <iomanip>
#include <sys/stat.h>
#include "Bank.h"
#include "TransactionWorker.h"
using namespace std;

Bank bank;

int getIntInput()
{
        int val;
        while (!(cin >> val))
        {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << ">> Invalid input. Please enter a number: ";
        }
        return val;
}

double getDoubleInput()
{
        double val;
        while (!(cin >> val) || val <= 0)
        {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << ">> Invalid amount. Please enter a positive number: ";
        }
        return val;
}

void* stressWorker(void* arg)
{
        ThreadArgs* args = (ThreadArgs*)arg;
        int accCount = args->bank->getAccountCount();
        int type = rand() % 3;

        args->fromAccountId = rand() % accCount;
        args->toAccountId   = rand() % accCount;
        args->amount        = (rand() % 100) + 10;

        if (type == DEPOSIT)
        {
                depositWorker(args);
        }
        else if (type == WITHDRAW)
        {
                withdrawWorker(args);
        }
        else
        {
                if (args->fromAccountId != args->toAccountId)
                        transferWorker(args);
                else
                        depositWorker(args);
        }

        return NULL;
}

void runStartupStressTest()
{
        cout << "\n>> Initializing system stress test" << endl;
        cout << ">> Spawning 20 concurrent transaction threads" << endl;
        cout << ">> Semaphore limit: " << MAX_TRANSACTIONS << " threads at a time" << endl;
        cout << "****************************************" << endl;

        const int N = 20;
        pthread_t threads[N];
        ThreadArgs args[N];

        double totalBefore = 0;
        for (int i = 0; i < bank.getAccountCount(); i++)
        {
                Account* acc = bank.getAccount(i);
                if (acc) totalBefore += acc->balance;
        }

        for (int i = 0; i < N; i++)
        {
                args[i].bank     = &bank;
                args[i].threadId = i;
                pthread_create(&threads[i], NULL, stressWorker, &args[i]);
        }

        for (int i = 0; i < N; i++)
                pthread_join(threads[i], NULL);

        cout << "*********************************" << endl;
        bank.verifyConsistency(totalBefore);
        cout << ">> Stress test complete. System is functional and ready to use" << endl;
}

// MENU AFTER LOGGING IN
void loggedInMenu(int accountId)
{
        int choice;

        while (true)
        {
                Account* acc = bank.getAccount(accountId);

                cout << "\n============================================" << endl;
                cout << "  Welcome, " << acc->owner << "(Account #" << accountId << ")" << endl;
                cout << "  Balance: $" << fixed << setprecision(2) << acc->balance << endl;
                cout << "============================================" << endl;
                cout << "1. Check Balance" << endl;
                cout << "2. Deposit money" << endl;
                cout << "3. Withdraw money" << endl;
                cout << "4. Transfer money" << endl;
                cout << "0. Logout" << endl;
                cout << "**********************************************" << endl;
                cout << "Enter choice: ";
                choice = getIntInput();

                switch (choice)
                {
                        case 1:
                        {
                                acc = bank.getAccount(accountId);
                                cout << "\n>> Account #" << accountId << endl;
                                cout << ">> Owner:   " << acc->owner << endl;
                                cout << ">> Balance: $" << fixed << setprecision(2) << acc->balance << endl;
                                break;
                        }

                        case 2:
                        {
                                cout << "Enter amount to deposit: $";
                                double amount = getDoubleInput();

                                pthread_t t;
                                ThreadArgs args;
                                args.bank            = &bank;
                                args.threadId        = accountId;
                                args.fromAccountId   = accountId;
                                args.toAccountId     = -1;
                                args.amount          = amount;
                                args.transactionType = DEPOSIT;
                                pthread_create(&t, NULL, depositWorker, &args);
                                pthread_join(t, NULL);
                                break;
                        }

                        case 3:
                        {
                                cout << "Enter amount to withdraw: $";
                                double amount = getDoubleInput();

                                pthread_t t;
                                ThreadArgs args;
                                args.bank            = &bank;
                                args.threadId        = accountId;
                                args.fromAccountId   = accountId;
                                args.toAccountId     = -1;
                                args.amount          = amount;
                                args.transactionType = WITHDRAW;
                                pthread_create(&t, NULL, withdrawWorker, &args);
                                pthread_join(t, NULL);
                                break;
                        }

                        case 4:
                        {
                                cout << "Enter destination account ID: ";
                                int toId = getIntInput();

                                if (toId == accountId)
                                {
                                        cout << ">> Cannot transfer to your own account" << endl;
                                        break;
                                }

                                if (bank.getAccount(toId) == NULL)
                                {
                                        cout << ">> Account #" << toId << " does not exist" << endl;
                                        break;
                                }

                                cout << "Enter amount to transfer: $";
                                double amount = getDoubleInput();

                                pthread_t t;
                                ThreadArgs args;
                                args.bank            = &bank;
                                args.threadId        = accountId;
                                args.fromAccountId   = accountId;
                                args.toAccountId     = toId;
                                args.amount          = amount;
                                args.transactionType = TRANSFER;
                                pthread_create(&t, NULL, transferWorker, &args);
                                pthread_join(t, NULL);
                                break;
                        }

                        case 0:
                        {
                                acc = bank.getAccount(accountId);
                                cout << ">> Logged out" << acc->owner << endl;
                                return;
                        }

                        default:
                                cout << ">> Invalid choice. Try again." << endl;
                }
        }
}

// MAIN MENU
void mainMenu()
{
        int choice;

        while (true)
        {
                cout << "\n============================================" << endl;
                cout << "       CONCURRENT BANKING SYSTEM            " << endl;
                cout << "============================================" << endl;
                cout << "1. Login" << endl;
                cout << "2. Create New Account" << endl;
                cout << "0. Exit" << endl;
                cout << "**********************************************" << endl;
                cout << "Enter choice: ";
                choice = getIntInput();

                switch (choice)
                {
                        case 1:
                        {
                                int id, pin;
                                int attempts = 0;

                                while (attempts < 3)
                                {
                                        cout << "Enter Account ID: ";
                                        id = getIntInput();
                                        cout << "Enter PIN: ";
                                        pin = getIntInput();

                                        if (bank.login(id, pin))
                                        {
                                                cout << ">> Login was  successful" << endl;
                                                loggedInMenu(id);
                                                break;
                                        }
                                        else
                                        {
                                                attempts++;
                                                cout << ">> Invalid Account ID or PIN." << endl;
                                                if (attempts < 3)
                                                        cout << ">> " << 3 - attempts << " attempts remaining." << endl;
                                        }
                                }

                                if (attempts == 3)
                                        cout << ">> Too many failed attempts. Returning to main menu." << endl;

                                break;
                        }

                        case 2:
                        {
                                string owner;
                                double balance;
                                int pin;

                                cout << "Enter your name: ";
                                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                getline(cin, owner);

                                cout << "Enter initial deposit: $";
                                balance = getDoubleInput();

                                cout << "Set a 4-digit PIN: ";
                                pin = getIntInput();

                                int id = bank.createAccount(owner, balance, pin);
                                if (id != -1)
                                {
                                        cout << ">> Account created successfully" << endl;
                                        cout << ">> Your Account ID is: " << id << endl;
                                        cout << ">> Please always remember your ID and PIN to login." << endl;
                                }
                                break;
                        }

                        case 0:
                        {
                                cout << ">> Thank you for using Concurrent Banking System" << endl;
                                exit(0);
                        }

                        default:
                                cout << ">> Invalid choice. Try again." << endl;
                }
        }
}

int main()
{
        srand(time(0));

        mkdir("logs", 0777);

        cout << "============================================" << endl;
        cout << "   CONCURRENT BANKING SYSTEM - OS PROJECT  " << endl;
        cout << "============================================" << endl;
        cout << ">> System starting up" << endl;

        bank.createAccount("Bot1",   5000.0, 1111);
        bank.createAccount("Bot2",     3000.0, 2222);
        bank.createAccount("Bot3", 4000.0, 3333);
        bank.createAccount("Bot4",   2000.0, 4444);
        bank.createAccount("Bot5",     6000.0, 5555);

        cout << ">> Default accounts loaded" << endl;

        runStartupStressTest();

        mainMenu();

        return 0;
}
