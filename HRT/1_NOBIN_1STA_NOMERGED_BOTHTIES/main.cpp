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
		
		std::map<int, std::map<int, GRBVar>> isDoctorIAllocatedToHospitalJ;
		std::vector<GRBLinExpr> allocationOfDoctorI(allo.nbDoctors);
		std::vector<GRBLinExpr> allocationOfHospitalJ(allo.nbHospitals);
		vector<vector<GRBLinExpr>> fillingOfHospitalJUpToRankK(allo.nbHospitals);

		// Initialization
		for (int i = 0; i < allo.nbDoctors; i++){
			isDoctorIAllocatedToHospitalJ[i] = std::map<int, GRBVar>();
			for (int j = 0; j<allo.doctors[i].nbPref; j++) {
				for (size_t id = 0; id < allo.doctors[i].preferences[j].size(); ++id) {
					int pref = allo.doctors[i].preferences[j][id];
					isDoctorIAllocatedToHospitalJ[i][pref-1] = model.addVar(0, 1, 0, GRB_BINARY);
				}
			}
		}

		for (int j = 0; j < allo.nbHospitals; j++){
			allocationOfHospitalJ[j] = 0;
			fillingOfHospitalJUpToRankK[j].resize(allo.hospitals[j].preferences.size());
			for(int k=0; k<allo.hospitals[j].nbPref; k++){
				fillingOfHospitalJUpToRankK[j][k] = 0;
			}
		}

		model.update();


		// perform values
		for (int i = 0; i < allo.nbDoctors; i++){
			for (int j = 0; j<allo.doctors[i].nbPref; j++) {
				for(size_t id = 0; id < allo.doctors[i].preferences[j].size(); ++id) {
					int pref = allo.doctors[i].preferences[j][id];
					allocationOfDoctorI[i] += isDoctorIAllocatedToHospitalJ[i][pref-1];
					allocationOfHospitalJ[pref-1] += isDoctorIAllocatedToHospitalJ[i][pref-1];
					objFun += isDoctorIAllocatedToHospitalJ[i][pref-1];
					for (size_t k = allo.doctors[i].ranks[j][id]-1; k < allo.hospitals[pref-1].preferences.size(); ++k) {
						fillingOfHospitalJUpToRankK[pref-1][k] += isDoctorIAllocatedToHospitalJ[i][pref-1];
					}
				}
			}
		}
		model.setObjective(objFun, GRB_MAXIMIZE);

		// Unique assignment for doctors constraints
		for (int i = 0; i < allo.nbDoctors; i++) {
			model.addConstr(allocationOfDoctorI[i] <= 1);
		}

		// Hospital capacity constraints
		for (int j = 0; j < allo.nbHospitals; j++){
			model.addConstr(allocationOfHospitalJ[j] <= allo.hospitals[j].cap);
		}

		// Stability constraints
		for (int i = 0; i < allo.nbDoctors; ++i) {
			GRBLinExpr leftside = 0;
			for( int j = 0; j < allo.doctors[i].nbPref; ++j) {
				GRBLinExpr rightside = 0;
				for( size_t id = 0; id < allo.doctors[i].preferences[j].size(); ++id) {
					int pref = allo.doctors[i].preferences[j][id];
					leftside += isDoctorIAllocatedToHospitalJ[i][pref-1];
				}
				// If doctor has not anything at this rank, each hospitals at this rank
				// better be full
				for( size_t id = 0; id < allo.doctors[i].preferences[j].size(); ++id) {
					int pref = allo.doctors[i].preferences[j][id];
					int rank = allo.doctors[i].ranks[j][id];
					model.addConstr(allo.hospitals[pref-1].cap * (1 - leftside) - fillingOfHospitalJUpToRankK[pref-1][rank-1] <= 0);
				}
			}
		}


				
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit, 3600);
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);

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
