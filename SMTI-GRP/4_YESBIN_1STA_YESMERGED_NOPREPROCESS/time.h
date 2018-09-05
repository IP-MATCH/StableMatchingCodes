#ifndef TIME_H 
	
	#define TIME_H

	#ifdef WIN32
		#include <Windows.h>
	#else
		#include <sys/time.h>
		#include <ctime>
	#endif

	double getWallTime();
	double getCPUTime();

#endif 
