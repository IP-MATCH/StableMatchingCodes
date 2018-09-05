#include "main.h"

/*	*************************************************************************************
	*************************************  MAIN *****************************************
	************************************************************************************* */

int main(int argc, char **argv){
	
	// local variables
	Allocation allo ;
	string filein = argv[2];
	string path = argv[1];
	string pathAndFileout = argv[3];

	// functions
	allo.load(path,filein);
	allo.printProb();

	manlove(allo);

	allo.printSol();
	allo.checkSolution();
	allo.printInfo(pathAndFileout);
}

int manlove(Allocation& allo){
	double initTimeModelCPU = getCPUTime();
	GRBEnv env = GRBEnv();

	allo.reduction();
	allo.printProb();
	allo.infos.timeCPUPP =  getCPUTime() - initTimeModelCPU;

	// Model
	try{
		// Local variables
		GRBModel model = GRBModel(env);
		GRBLinExpr objFun = 0;
		
		vector<vector<GRBVar> > isDoctorIAllocatedToHospitalJ (allo.nbDoctors);
		vector<vector<GRBVar> > isHospitalJFullAtRankK (allo.nbHospitals);

		vector<GRBLinExpr> allocationOfDoctorI(allo.nbDoctors);
		vector<GRBLinExpr> allocationOfHospitalJ(allo.nbHospitals);
		vector<vector<GRBLinExpr> > fillingOfHospitalJUpToRankK(allo.nbHospitals);

		// Initialization
		for (int i = 0; i < allo.nbDoctors; i++){
			allocationOfDoctorI[i] = 0;
			isDoctorIAllocatedToHospitalJ[i].resize(allo.doctors[i].nbPref);
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				isDoctorIAllocatedToHospitalJ[i][j] = model.addVar(0, 1, 0, GRB_BINARY);
			}
		}

		for (int j = 0; j < allo.nbHospitals; j++){
			allocationOfHospitalJ[j] = 0;
			isHospitalJFullAtRankK[j].resize(allo.hospitals[j].preferences.size()+1);
			for(int k=0; k<allo.hospitals[j].preferences.size()+1; k++){
				isHospitalJFullAtRankK[j][k] = model.addVar(0, 1, 0, GRB_BINARY);
			}
			fillingOfHospitalJUpToRankK[j].resize(allo.hospitals[j].preferences.size());
			for(int k=0; k<allo.hospitals[j].preferences.size(); k++){
				fillingOfHospitalJUpToRankK[j][k] = 0;
			}
		}

		model.update();

		// Perform values
		for (int i = 0; i < allo.nbDoctors; i++){
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				allocationOfDoctorI[i] += isDoctorIAllocatedToHospitalJ[i][j];
				allocationOfHospitalJ[allo.doctors[i].preferences[j] - 1] += isDoctorIAllocatedToHospitalJ[i][j];
				objFun += isDoctorIAllocatedToHospitalJ[i][j];
				for (int k= allo.doctors[i].ranks[j]-1; k < allo.hospitals[allo.doctors[i].preferences[j] - 1].preferences.size(); k++){
					fillingOfHospitalJUpToRankK[allo.doctors[i].preferences[j] - 1][k] += isDoctorIAllocatedToHospitalJ[i][j];
				}
			}
		}

		// Objective function
		model.setObjective(objFun, GRB_MAXIMIZE);

		// Unique assignment for doctors costraints
		for (int i = 0; i < allo.nbDoctors; i++){
			model.addConstr(allocationOfDoctorI[i] <= 1);
		}

		// Hospital capacity contsraints
		for (int j = 0; j < allo.nbHospitals; j++){
			model.addConstr(allocationOfHospitalJ[j] <= allo.hospitals[j].cap);
		}

		// Stability constraints
		for (int i = 0; i < allo.nbDoctors; i++){
			GRBLinExpr leftside = 0;
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				leftside += isDoctorIAllocatedToHospitalJ[i][j];
				model.addConstr(/*isDoctorIAllocatedToHospitalJ[i][j]  +*/ allo.hospitals[allo.doctors[i].preferences[j] - 1].cap * ( 1 - leftside) - fillingOfHospitalJUpToRankK[allo.doctors[i].preferences[j] - 1][allo.doctors[i].ranks[j]-1] <= 0);
			}	
		} 

		// Doctor rank constraints
		for (int i = 0; i < allo.nbDoctors; i++){
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				model.addConstr(isDoctorIAllocatedToHospitalJ[i][j] <= 1 - isHospitalJFullAtRankK[allo.doctors[i].preferences[j] - 1][allo.doctors[i].ranks[j]-1]);
			}
		}

		// Increasing rank constraints 
		for (int j = 0; j < allo.nbHospitals; j++){
			for(int k=0; k<allo.hospitals[j].preferences.size(); k++){
				model.addConstr(isHospitalJFullAtRankK[j][k] <= isHospitalJFullAtRankK[j][k+1]);
			}
		}

		// Stability constraints about the last rank
		for (int j = 0; j < allo.nbHospitals; j++){
			model.addConstr(allo.hospitals[j].cap * isHospitalJFullAtRankK[j][allo.hospitals[j].preferences.size()] - allocationOfHospitalJ[j] <= 0);
		} 

		// Constraint about opening a new rank
		for (int j = 0; j < allo.nbHospitals; j++){
			for(int k=0; k<allo.hospitals[j].preferences.size(); k++){
				GRBLinExpr tempV = 0;
				for(int i=0; i<allo.hospitals[j].preferences[k].size();i++){
					int l = 0;
					while(allo.doctors[allo.hospitals[j].preferences[k][i]-1].preferences[l] != j+1){
						tempV += isDoctorIAllocatedToHospitalJ[allo.hospitals[j].preferences[k][i]-1][l];
						l++;
					}
					tempV += isDoctorIAllocatedToHospitalJ[allo.hospitals[j].preferences[k][i]-1][l];
				}
				model.addConstr(allo.hospitals[j].preferences[k].size() * (1 - isHospitalJFullAtRankK[j][k+1]) <= tempV);
			}
		}
				
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  3600 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);

	/*	for (int j = 0; j < allo.nbHospitals; j++){
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
				if (ceil(isDoctorIAllocatedToHospitalJ[i][j].get(GRB_DoubleAttr_X) - EPSILON) == 1){
					int idHospital = allo.doctors[i].preferences[j];
					allo.assignmentByDoctor[i] = idHospital;
					allo.assignmentByHospital[idHospital-1].push_back(i+1);
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
