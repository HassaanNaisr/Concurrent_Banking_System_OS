#include "TransactionWorker.h"
#include <iostream>
using namespace std;

void* depositWorker(void* arg)
{
        ThreadArgs* args = (ThreadArgs*)arg;

        bool result = args->bank->deposit(
                args->fromAccountId,
                args->amount,
                args->threadId
        );

        if (result)
        {
                cout << "Thread-" << args->threadId
                     << " | SUCCESSFUL DEPOSIT" << endl;
        }
        else
        {
                cout << "Thread-" << args->threadId
                     << " | DEPOSIT FAILURE" << endl;
        }

        return NULL;
}

void* withdrawWorker(void* arg)
{
        ThreadArgs* args = (ThreadArgs*)arg;

        bool result = args->bank->withdraw(
                args->fromAccountId,
                args->amount,
                args->threadId
        );

        if (result)
        {
                cout << "Thread-" << args->threadId
                     << " | SUCCESSFUL WITHDRAWAL" << endl;
        }
        else
        {
                cout << "Thread-" << args->threadId
                     << " | WITHDRAW FAILURE" << endl;
        }

        return NULL;
}

void* transferWorker(void* arg)
{
        ThreadArgs* args = (ThreadArgs*)arg;

        bool result = args->bank->transfer(
                args->fromAccountId,
                args->toAccountId,
                args->amount,
                args->threadId
        );

        if (result)
        {
                cout << "Thread-" << args->threadId
                     << " | SUCCESSFUL TRANSFER" << endl;
        }
        else
        {
                cout << "Thread-" << args->threadId
                     << " | TRANSFER FAILURE" << endl;
        }

        return NULL;
}
