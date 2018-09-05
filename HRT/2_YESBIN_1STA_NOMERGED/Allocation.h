#ifndef ALLOCATION_H
	#define ALLOCATION_H
	
	using namespace std;
	#include <iostream> 
	#include <fstream>
	#include <sstream>
	#include <vector>
	#include <string>
	#include <set>

	class Hospital;
	class Doctor;
	class Assignment;
	class Allocation;

/*	*************************************************************************************
	***********************************  DOCTOR *****************************************
	************************************************************************************* */

	class Doctor{
	public:
		int id;
		int nbPref;
		vector<int> preferences;
		vector<int> ranks;

		void print();
	};

/*	*************************************************************************************
	*********************************** HOSPITAL ****************************************
	************************************************************************************* */

	class Hospital{
	public:
		int id;
		int cap;
		int nbPref;
		int nbTotPref;
		vector<vector<int> > preferences;
		
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
		int LB;
		int UB;
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
		int nbDoctors;
		int nbHospitals;

		vector<Doctor> doctors;
		vector<Hospital> hospitals;

		// Given by the ILP model
		vector<int> assignmentByDoctor;
		vector<vector<int> > assignmentByHospital;
		Info infos;
		
		void load(const string& path, const string& filein);
		void printProb();
		int reductionHosOff();
		int reductionResApp();
		void polish();
		void reduction();
		void printSol();
		void printInfo(const string& pathAndFileout);
		void checkSolution();
	};
	

#endif 