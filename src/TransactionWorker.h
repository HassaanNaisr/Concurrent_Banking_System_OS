#ifndef TRANSACTIONWORKER_H
#define TRANSACTIONWORKER_H

#include "Bank.h"

struct ThreadArgs
{
        Bank* bank;
        int threadId;
        int fromAccountId;
        int toAccountId;
        double amount;
        int transactionType;
};

#define DEPOSIT  0
#define WITHDRAW 1
#define TRANSFER 2

void* depositWorker(void* arg);
void* withdrawWorker(void* arg);
void* transferWorker(void* arg);

#endif
