#ifndef BANK_H
#define BANK_H

#include "Account.h"
#include "Logger.h"
#include <semaphore.h>
#include <string>

#define MAX_TRANSACTIONS 5

class Bank 
{
	private:
    		Account accounts[MAX_ACCOUNTS];
    		int accountCount;
		sem_t transactionSlots;
    		Logger logger;

	public:
    		Bank();
    		~Bank();

    		int createAccount(const std::string& owner, double initialBalance, int pin);
		bool login(int accountId, int pin);
    		bool deposit(int accountId, double amount, int threadId);
    		bool withdraw(int accountId, double amount, int threadId);
    		bool transfer(int fromId, int toId, double amount, int threadId);
    		void printAllAccounts();
    		void verifyConsistency(double expectedTotal);
    		Account* getAccount(int accountId);
    		int getAccountCount();
};

#endif
