#ifndef ALLOCATION_H
	#define ALLOCATION_H
	
	using namespace std;
	#include <iostream> 
	#include <fstream>
	#include <sstream>
	#include <vector>
	#include <string>
	#include <set>
	#include <map>
	#include <algorithm> 

	class Child;
	typedef Child Family;
	class Assignment;
	class Allocation;

/*	*************************************************************************************
	***********************************  DOCTOR *****************************************
	************************************************************************************* */

	class Child{
	public:
		int id;
		int nbPref;
		int nbTotPref; 
		vector<vector<int> > preferences;
		vector<vector<int> > ranks;
		vector<vector<int> > positions;
		void print(bool);
	};

/*	*************************************************************************************
	************************************* INFO ******************************************
	************************************************************************************* */

	class Info{
	public:
		bool opt;
		double timeCPU;
		double timeCPUPP;
		double LB;
		double UB;
		float contUB;
		int nbCons;
		int nbVar;
		int nbNZ;
		float contUB2;
		int nbCons2;
		int nbVar2;
		int nbNZ2;
		float altInfo;
	};

/*	*************************************************************************************
	********************************** ALLOCATION ***************************************
	************************************************************************************* */
	class Allocation{
	public:
		// Data read from the file
		string name;
		int nbChildren;
		int nbFamilies;
		int total_reduced;
		vector<vector<float> > grades;

		vector<Child> children;
		vector<Family> families;

		// Given by the ILP model
		vector<int> assignmentByChild;
		vector<int> assignmentByFamily;

		Info infos;
		
		void load(const string& path, const string& filein, const int& threshold);
		void printProb();
		int reductionMine(bool children_side=true, int mode=0);
		void polish();
		void reduction(int mode);
		void printSol();
		void printInfo(const string& pathAndFileout);
		void checkSolution();
	};
	

#endif
