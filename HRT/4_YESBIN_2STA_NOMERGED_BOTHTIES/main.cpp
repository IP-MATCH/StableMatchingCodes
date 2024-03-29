#include "main.h"

#include <map>

/*	*************************************************************************************
	*************************************  MAIN *****************************************
	************************************************************************************* */

int main(int argc, char **argv){
	
	// local variables
	Allocation allo ;
	string filein = argv[2];
	string path = argv[1];
	string pathAndFileout = argv[3];
	int mode = atoi(argv[4]);

	// functions
	allo.load(path,filein);
	allo.printProb();
	manlove(allo, mode);

	allo.printProb();
	allo.printSol();
	allo.checkSolution();
	allo.printInfo(pathAndFileout);
}

int manlove(Allocation& allo, int mode){
	double initTimeModelCPU = getCPUTime();
	GRBEnv env = GRBEnv();

	if (mode != 12) {
		allo.reduction(mode);
		allo.printProb();
	}
	allo.infos.timeCPUPP =  getCPUTime() - initTimeModelCPU;

	// Model
	try{
		// Local variables
		GRBModel model = GRBModel(env);
		GRBLinExpr objFun = 0;
		
		std::map<int, std::map<int, GRBVar> > isDoctorIAllocatedToHospitalJ;
		vector<vector<GRBVar> > isHospitalJFullAtRankK (allo.nbHospitals);

		vector<vector<GRBVar> > hIBFD (allo.nbDoctors);
		vector<vector<GRBVar> > hIBFH (allo.nbHospitals);

		// Initialization
		for (int i = 0; i < allo.nbDoctors; i++){
			isDoctorIAllocatedToHospitalJ[i] = std::map<int, GRBVar>();
			hIBFD[i].resize(allo.doctors[i].nbPref);
			for (int j = 0; j<allo.doctors[i].nbPref; j++) {
				hIBFD[i][j] = model.addVar(0, 1, 0, GRB_BINARY);
				for (size_t id = 0; id < allo.doctors[i].preferences[j].size(); ++id) {
					int pref = allo.doctors[i].preferences[j][id];
					isDoctorIAllocatedToHospitalJ[i][pref-1] = model.addVar(0, 1, 0, GRB_BINARY);
				}
			}
		}

		for (int j = 0; j < allo.nbHospitals; j++){
			hIBFH[j].resize(allo.hospitals[j].nbPref);
			isHospitalJFullAtRankK[j].resize(allo.hospitals[j].preferences.size()+1);
			for(int k=0; k<allo.hospitals[j].nbPref; k++){
				hIBFH[j][k] = model.addVar(0, allo.hospitals[j].cap, 0, GRB_INTEGER);
				isHospitalJFullAtRankK[j][k] = model.addVar(0, 1, 0, GRB_BINARY);
			}
			isHospitalJFullAtRankK[j][allo.hospitals[j].nbPref] = model.addVar(0, 1, 0, GRB_BINARY);
		}

		model.update();

		// Objective function
		for (int i = 0; i < allo.nbDoctors; i++){
			if(allo.doctors[i].nbPref > 0)
				objFun += hIBFD[i].back();
		}
		
		model.setObjective(objFun, GRB_MAXIMIZE);

		// hIBF values
		for (int i = 0; i < allo.nbDoctors; i++){
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				GRBLinExpr exp;
				for (size_t id = 0; id < allo.doctors[i].preferences[j].size(); ++id) {
					int pref = allo.doctors[i].preferences[j][id];
					exp += isDoctorIAllocatedToHospitalJ[i][pref-1];
				}
				if(j > 0) exp += hIBFD[i][j-1];
				model.addConstr(hIBFD[i][j] == exp);
			}
		}

		for (int j = 0; j < allo.nbHospitals; j++){
			for (int k = 0; k<allo.hospitals[j].nbPref; k++){
				GRBLinExpr exp;
				for(size_t l=0; l<allo.hospitals[j].preferences[k].size();l++){
					int idxDoc = allo.hospitals[j].preferences[k][l]-1;
					auto it = isDoctorIAllocatedToHospitalJ[idxDoc].find(j);
					if (it != isDoctorIAllocatedToHospitalJ[idxDoc].end()) {
						exp += isDoctorIAllocatedToHospitalJ[idxDoc][j];
					}
				}
				if(k > 0) exp += hIBFH[j][k-1];
				model.addConstr(hIBFH[j][k] == exp);
			}
		}

		// Doctor rank constraints
		for (int i = 0; i < allo.nbDoctors; i++){
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				for (size_t id = 0; id < allo.doctors[i].preferences[j].size(); ++id) {
					int pref = allo.doctors[i].preferences[j][id];
					model.addConstr(isDoctorIAllocatedToHospitalJ[i][pref-1] <= 1 - isHospitalJFullAtRankK[pref-1][allo.doctors[i].ranks[j][id]-1]);
				}
			}
		}

		// Increasing rank constraints 
		for (int j = 0; j < allo.nbHospitals; j++){
			for(size_t k=0; k<allo.hospitals[j].preferences.size(); k++){
				model.addConstr(isHospitalJFullAtRankK[j][k] <= isHospitalJFullAtRankK[j][k+1]);
			}
		}

		// Stability constraints about the last rank
		for (int j = 0; j < allo.nbHospitals; j++){
			model.addConstr(allo.hospitals[j].cap * isHospitalJFullAtRankK[j][allo.hospitals[j].preferences.size()] - hIBFH[j][allo.hospitals[j].preferences.size()-1] <= 0);
		} 

		// Constraint about opening a new rank
		for (int j = 0; j < allo.nbHospitals; j++){
			for(size_t k=0; k<allo.hospitals[j].preferences.size(); k++){
				for(size_t i=0; i<allo.hospitals[j].preferences[k].size();i++){
					int idxDoc = allo.hospitals[j].preferences[k][i]-1;
					int idxRank = -1;
					for(size_t m=0; (idxRank == -1) && m < allo.doctors[idxDoc].preferences.size();m++) {
						for(size_t id = 0; id < allo.doctors[idxDoc].preferences[m].size(); ++id) {
							if(allo.doctors[idxDoc].preferences[m][id]-1 == j) {
								idxRank = m;
								break;
							}
						}
					}
					model.addConstr(1 - isHospitalJFullAtRankK[j][k+1] <= hIBFD[idxDoc][idxRank]);
				}
			}
		}
				
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit, 3600);
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);

		/*for (int j = 0; j < allo.nbHospitals; j++){
			for(int k=0; k<allo.hospitals[j].preferences.size()+1; k++){
				isHospitalJFullAtRankK[j][k].set(GRB_IntAttr_BranchPriority, 10);
			}
		}*/
		model.optimize();

		// Filling Info
		allo.infos.timeCPU =  getCPUTime() - initTimeModelCPU;
		allo.infos.UB = ceil(model.get(GRB_DoubleAttr_ObjBound) - EPSILON);
		allo.infos.opt = false;

		// Get Info pre preprocessing
		allo.infos.nbVar =  model.get(GRB_IntAttr_NumVars);
		allo.infos.nbCons = model.get(GRB_IntAttr_NumConstrs);
		allo.infos.nbNZ = model.get(GRB_IntAttr_NumNZs);
		GRBModel modelRelaxed = model.relax();
		modelRelaxed.optimize();
		allo.infos.contUB = modelRelaxed.get(GRB_DoubleAttr_ObjVal);

		// Get Info post preprocessing
		GRBModel presolvedModel = model.presolve();
		allo.infos.nbVar2 =  presolvedModel.get(GRB_IntAttr_NumVars);
		allo.infos.nbCons2 = presolvedModel.get(GRB_IntAttr_NumConstrs);
		allo.infos.nbNZ2 = presolvedModel.get(GRB_IntAttr_NumNZs);
		GRBModel presolvedModelRelaxed = presolvedModel.relax();
		presolvedModelRelaxed.optimize();
		allo.infos.contUB2 = presolvedModelRelaxed.get(GRB_DoubleAttr_ObjVal);

		// If no solution found
		if (model.get(GRB_IntAttr_SolCount) < 1){
			cout << "Failed to optimize ILP. " << endl;
			allo.infos.LB  = 0;
			allo.assignmentByDoctor.resize(allo.nbDoctors, -1);
			allo.assignmentByHospital.resize(allo.nbHospitals);
			return -1;
		}

		// If solution found
		allo.infos.LB = ceil(model.get(GRB_DoubleAttr_ObjVal) - EPSILON);	
		if(allo.infos.LB == allo.infos.UB) allo.infos.opt = true;

		// Filling Solution
		allo.assignmentByDoctor.resize(allo.nbDoctors, -1);
		allo.assignmentByHospital.resize(allo.nbHospitals);

		for (int i = 0; i < allo.nbDoctors; i++){
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				for (size_t id = 0; id < allo.doctors[i].preferences[j].size(); ++id) {
					if (ceil(isDoctorIAllocatedToHospitalJ[i][allo.doctors[i].preferences[j][id] - 1].get(GRB_DoubleAttr_X) - EPSILON) == 1){
						int idHospital = allo.doctors[i].preferences[j][id];
						allo.assignmentByDoctor[i] = idHospital;
						allo.assignmentByHospital[idHospital-1].push_back(i+1);
					}
				}
			}
		}
	}

	// Exceptions
	catch (GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "Exception during optimization" << endl;
	}


	// End
	return 0;
}
