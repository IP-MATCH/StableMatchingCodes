#ifndef MAIN_H 
	#define MAIN_H
	using namespace std;

	#include <iostream> 
	#include <math.h> 

	#include "gurobi_c++.h"
	#include "time.h"
	#include "Allocation.h" 

	float EPSILON = 0.001;

	int manlove(Allocation& allo);

#endif 
