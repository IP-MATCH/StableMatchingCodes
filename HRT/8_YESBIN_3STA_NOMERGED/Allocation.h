#ifndef ALLOCATION_H
	#define ALLOCATION_H
	
	using namespace std;
	#include <iostream> 
	#include <fstream>
#include <list>
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
		bool mustBeAllocated;

		void print();

		Doctor() : mustBeAllocated(false) { }
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
		bool mustBeAllocated;
		vector<vector<int> > preferences;

		void print();

		Hospital() : mustBeAllocated(false) { }
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


		int total_removed;
		int pp_mode;

		vector<Doctor> doctors;
		vector<Hospital> hospitals;

		std::list<int> doctorsMustBeAllocated;
		std::list<int> hospitalsMustBeAllocated;

		// Given by the ILP model
		vector<int> assignmentByDoctor;
		vector<vector<int> > assignmentByHospital;
		Info infos;
		
		void load(const string& path, const string& filein);
		void printProb();
		int reductionHosOff();
		int reductionResApp();
		void polish();
		void reduction(int mode);
		int reductionMineDoctors(int mode);
		int reductionMineHospitals(int mode);
		int reductionExactHospital(bool supp);
		int reductionExactDoctor(bool supp);
		void printSol();
		void printInfo(const string& pathAndFileout);
		void checkSolution();
	};
	

#endif 