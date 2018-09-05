#ifndef MAIN_H 
	#define MAIN_H
	using namespace std;

	#include <iostream> 
	#include <math.h> 
	#include <cmath>  
	#include "gurobi_c++.h"
	#include "time.h"
	#include "Allocation.h" 
	#include "gale_shapley.h"

	float EPSILON = 0.001;

	int manlove(Allocation& allo, bool presolveGaleShapley, bool optimiseSize, int galeShapleyRuns);

#endif 
