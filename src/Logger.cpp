#include "Logger.h"
#include <iostream>
#include <ctime>
using namespace std;

Logger::Logger(const string& filename) 
{
	pthread_mutex_init(&logMutex, NULL);
	logFile.open(filename, ios::app);
	if (!logFile.is_open()) 
	{
		cout << "The log file could not be opened(ERROR)" << endl;
	}
}

Logger::~Logger() 
{
	if (logFile.is_open()) 
	{
		logFile.close();
    	}
    	pthread_mutex_destroy(&logMutex);
}

void Logger::log(const string& message) 
{
    	time_t current = time(0);
    	char timestamp[20];
    	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&current));

	pthread_mutex_lock(&logMutex);
    	logFile << "[" << timestamp << "] " << message << endl;
    	cout  << "[" << timestamp << "] " << message << endl;
    	pthread_mutex_unlock(&logMutex);
}
