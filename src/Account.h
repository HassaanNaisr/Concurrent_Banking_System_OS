#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <pthread.h>
#include <string>

#define MAX_ACCOUNTS 20

struct Account 
{
    	int id;
	int pin;
    	std::string owner;
    	double balance;
    	pthread_mutex_t mutex;
    	bool active;
};

#endif
