#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <pthread.h>

class Logger 	
{
	private:
    		std::ofstream logFile;
    		pthread_mutex_t logMutex;

	public:
    		Logger(const std::string& filename);
    		~Logger();
   		 void log(const std::string& message);
};

#endif
