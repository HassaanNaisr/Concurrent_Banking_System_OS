#include "Bank.h"
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

Bank::Bank() : logger("logs/transactions.log") 
{
        accountCount = 0;
        sem_init(&transactionSlots, 0, MAX_TRANSACTIONS);
        for (int i = 0; i < MAX_ACCOUNTS; i++) 
	{
                accounts[i].active = false;
                pthread_mutex_init(&accounts[i].mutex, NULL);
        }
}

Bank::~Bank() 
	{
        sem_destroy(&transactionSlots);
        for (int i = 0; i < MAX_ACCOUNTS; i++) 
	{
                pthread_mutex_destroy(&accounts[i].mutex);
        }
}

int Bank::createAccount(const string& owner, double initialBalance, int pin)
{
        if (accountCount >= MAX_ACCOUNTS)
        {
                logger.log("ERROR: Max account limit reached");
                return -1;
        }
        int id = accountCount;
        accounts[id].id = id;
        accounts[id].owner = owner;
        accounts[id].balance = initialBalance;
        accounts[id].pin = pin;
        accounts[id].active = true;
        accountCount++;
        logger.log("Account created | ID: " + to_string(id) + " | Owner: " + owner);
        return id;
}

bool Bank::login(int accountId, int pin) 
{
	Account* acc = getAccount(accountId);
    	if (acc == NULL) return false;
    	return acc->pin == pin;
}

bool Bank::deposit(int accountId, double amount, int threadId) 
{
        sem_wait(&transactionSlots);

        Account* acc = getAccount(accountId);
        if (acc == NULL) 
	{
                logger.log("Thread-" + to_string(threadId) + " | DEPOSIT FAILED | Invalid account: " + to_string(accountId));
                sem_post(&transactionSlots);
                return false;
        }

        pthread_mutex_lock(&acc->mutex);
        acc->balance += amount;
        logger.log("Thread-" + to_string(threadId) + " | DEPOSIT | Account: " + to_string(accountId) + " | Amount: $" + to_string(amount) + " | Balance: $" + to_string(acc->balance));
        pthread_mutex_unlock(&acc->mutex);

        sem_post(&transactionSlots);
        return true;
}

bool Bank::withdraw(int accountId, double amount, int threadId) 
{
        sem_wait(&transactionSlots);

        Account* acc = getAccount(accountId);
        if (acc == NULL) 
	{
                logger.log("Thread-" + to_string(threadId) + " | WITHDRAW FAILED | Invalid account: " + to_string(accountId));
                sem_post(&transactionSlots);
                return false;
        }

	pthread_mutex_lock(&acc->mutex);
        if (acc->balance < amount) 
	{
                logger.log("Thread-" + to_string(threadId) + " | WITHDRAW FAILED | Account: " + to_string(accountId) + " | Insufficient funds");
                pthread_mutex_unlock(&acc->mutex);
                sem_post(&transactionSlots);
                return false;
        }
        acc->balance -= amount;
        logger.log("Thread-" + to_string(threadId) + " | WITHDRAW | Account: " + to_string(accountId) + " | Amount: $" + to_string(amount) + " | Balance: $" + to_string(acc->balance));
        pthread_mutex_unlock(&acc->mutex);

        sem_post(&transactionSlots);
        return true;
}

bool Bank::transfer(int fromId, int toId, double amount, int threadId)
{
        sem_wait(&transactionSlots);

        Account* acc1 = getAccount(min(fromId, toId));
        Account* acc2 = getAccount(max(fromId, toId));

        if (acc1 == NULL || acc2 == NULL) 
	{
                logger.log("Thread-" + to_string(threadId) + " | TRANSFER FAILED | Invalid account");
                sem_post(&transactionSlots);
                return false;
        }

        pthread_mutex_lock(&acc1->mutex);
        pthread_mutex_lock(&acc2->mutex);

        Account* from = getAccount(fromId);
        Account* to = getAccount(toId);

        if (from->balance < amount) 
	{
                logger.log("Thread-" + to_string(threadId) + " | TRANSFER FAILED | Account: " + to_string(fromId) + " | Insufficient funds");
                pthread_mutex_unlock(&acc1->mutex);
                pthread_mutex_unlock(&acc2->mutex);
                sem_post(&transactionSlots);
                return false;
        }

        from->balance -= amount;
        to->balance += amount;
        logger.log("Thread-" + to_string(threadId) + " | TRANSFER | From: " + to_string(fromId) + " To: " + to_string(toId) + " | Amount: $" + to_string(amount));

        pthread_mutex_unlock(&acc1->mutex);
        pthread_mutex_unlock(&acc2->mutex);

        sem_post(&transactionSlots);
        return true;
}

void Bank::printAllAccounts() 
{
        cout << "\n****** BANK ACCOUNTS ******" << endl;
        for (int i = 0; i < accountCount; i++) 
	{
                if (accounts[i].active) 
		{
                        cout << "Account " << accounts[i].id
                             << " | Owner: " << accounts[i].owner
                             << " | Balance: $" << accounts[i].balance << endl;
                }
        }
        cout << "********************\n" << endl;
}

void Bank::verifyConsistency(double expectedTotal) 
{
        double total = 0;
        for (int i = 0; i < accountCount; i++) 
	{
                if (accounts[i].active) 
		{
                        pthread_mutex_lock(&accounts[i].mutex);
                        total += accounts[i].balance;
                        pthread_mutex_unlock(&accounts[i].mutex);
                }
        }
        if (abs(total - expectedTotal) < 0.01) 
	{
                logger.log("AUDIT PASSED | Total: $" + to_string(total));
        }
	else 
	{
                logger.log("AUDIT FAILED | Expected: $" + to_string(expectedTotal) + " | Got: $" + to_string(total));
        }
}

Account* Bank::getAccount(int accountId) 
{
        if (accountId < 0 || accountId >= accountCount) 
	{
                return NULL;
        }
        if (!accounts[accountId].active) 
	{
                return NULL;
        }
        return &accounts[accountId];
}

int Bank::getAccountCount() 
{
        return accountCount;
}
