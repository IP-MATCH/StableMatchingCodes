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
	class Family;
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
		void print();
	};

/*	*************************************************************************************
	*********************************** HOSPITAL ****************************************
	************************************************************************************* */

	class Family{
	public:
		int id;
		int nbPref;
		int nbTotPref;
		vector<vector<int> > preferences;
		vector<vector<int> > ranks;
		vector<vector<int> > positions;
		void print();
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

		vector<Child> children;
		vector<Family> families;

		// Given by the ILP model
		vector<int> assignmentByChild;
		vector<int> assignmentByFamily;

		Info infos;
		
		void load(const string& path, const string& filein);
		void printProb();
		int reductionFam1();
		int reductionChi1();
		int reductionFam2();
		int reductionChi2();
		void polish();
		void reduction();
		void printSol();
		void printInfo(const string& pathAndFileout);
		void checkSolution();
	};
	

#endif 