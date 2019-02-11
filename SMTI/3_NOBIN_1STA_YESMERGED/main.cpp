#include "main.h"

#define MAXTIME 3600

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
	//allo.printProb();
	//allo.reduction();
	//allo.printProb();

	manlove(allo, mode);
	
	allo.printSol();
	allo.checkSolution();
	allo.printInfo(pathAndFileout);
}


int manlove(Allocation& allo, int mode){
	double initTimeModelCPU = getCPUTime();
	GRBEnv env = GRBEnv();

	double initTimeModelCPUPP = getCPUTime();
	allo.reduction(mode);
	//allo.printProb();
	allo.infos.timeCPUPP =  getCPUTime() - initTimeModelCPUPP;
	if (allo.infos.timeCPUPP > MAXTIME) {
		cout << "Preprocessing took over MAXTIME seconds" << endl;
		allo.infos.LB  = 0;
		allo.assignmentByChild.resize(allo.nbChildren, -1);
		allo.assignmentByFamily.resize(allo.nbFamilies,-1);
		return -1;
	}

	// Model
	try{
		// Local variables
		GRBModel model = GRBModel(env);
		GRBLinExpr objFun = 0;
		
		vector<vector<vector<GRBVar> > > isChildIAllocatedToFamilyJ (allo.nbChildren);

		vector<GRBLinExpr> allocationOfChildI(allo.nbChildren);
		vector<GRBLinExpr> allocationOfFamilyJ(allo.nbFamilies);
		vector<vector<GRBLinExpr> > fillingOfFamilyJUpToRankK(allo.nbFamilies);

		// Initialization
		for (int i = 0; i < allo.nbChildren; i++){
			allocationOfChildI[i] = 0;
			isChildIAllocatedToFamilyJ[i].resize(allo.children[i].nbPref);
			for (int j = 0; j<allo.children[i].nbPref; j++){
				isChildIAllocatedToFamilyJ[i][j].resize(allo.children[i].preferences[j].size());
				for (int k = 0; k<allo.children[i].preferences[j].size(); k++){
					isChildIAllocatedToFamilyJ[i][j][k] = model.addVar(0, 1, 0, GRB_BINARY);
				}
			}
		}

		for (int j = 0; j < allo.nbFamilies; j++){
			allocationOfFamilyJ[j] = 0;
			fillingOfFamilyJUpToRankK[j].resize(allo.families[j].preferences.size());
			for(int k=0; k<allo.families[j].preferences.size(); k++){
				fillingOfFamilyJUpToRankK[j][k] = 0;
			}
		}

		model.update();

		// Perform values
		for (int i = 0; i < allo.nbChildren; i++){
			for (int j = 0; j<allo.children[i].nbPref; j++){
				for(int k=0; k<allo.children[i].preferences[j].size();k++){
					int idxFam = allo.children[i].preferences[j][k];
					int idxRank = allo.children[i].ranks[j][k];
					allocationOfChildI[i] += isChildIAllocatedToFamilyJ[i][j][k];
					allocationOfFamilyJ[idxFam] += isChildIAllocatedToFamilyJ[i][j][k];
					objFun += isChildIAllocatedToFamilyJ[i][j][k] * 1;
					for (int l= idxRank; l < allo.families[idxFam].preferences.size(); l++){
						fillingOfFamilyJUpToRankK[idxFam][l] += isChildIAllocatedToFamilyJ[i][j][k];
					}
				}
			}
		}

		// Objective function
		model.setObjective(objFun, GRB_MAXIMIZE);

		// Unique assignment for children constraints
		for (int i = 0; i < allo.nbChildren; i++){
			model.addConstr(allocationOfChildI[i] <= 1);
		}

		//  Unique assignment for family constraints
		for (int j = 0; j < allo.nbFamilies; j++){
			model.addConstr(allocationOfFamilyJ[j] <= 1);
		}

		// Stability constraints
		for (int i = 0; i < allo.nbChildren; i++){
			GRBLinExpr leftside = 0;
			for (int j = 0; j<allo.children[i].nbPref; j++){
				GRBLinExpr rightside = 0;
				for(int k=0; k<allo.children[i].preferences[j].size();k++) {
					leftside += isChildIAllocatedToFamilyJ[i][j][k];
					int idxFam = allo.children[i].preferences[j][k];
					int idxRank = allo.children[i].ranks[j][k];		
					rightside += fillingOfFamilyJUpToRankK[idxFam][idxRank];
				}
				model.addConstr(allo.children[i].preferences[j].size() * (1 - leftside) <= rightside);
			}	
		} 
			
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit, MAXTIME - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);
		model.optimize();

		// Filling Info
		allo.infos.timeCPU =  getCPUTime() - initTimeModelCPU;
		allo.infos.UB = model.get(GRB_DoubleAttr_ObjBound);
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
			allo.assignmentByChild.resize(allo.nbChildren, -1);
			allo.assignmentByFamily.resize(allo.nbFamilies,-1);
			return -1;
		}

		// If solution found
		allo.infos.LB = model.get(GRB_DoubleAttr_ObjVal);	
		if(std::abs(allo.infos.UB - allo.infos.LB) <= EPSILON) allo.infos.opt = true;

		// Filling Solution
		allo.assignmentByChild.resize(allo.nbChildren, -1);
		allo.assignmentByFamily.resize(allo.nbFamilies,-1);

		for (int i = 0; i < allo.nbChildren; i++){
			for (int j = 0; j<allo.children[i].nbPref; j++){
				for(int k=0; k<allo.children[i].preferences[j].size();k++){
					if (ceil(isChildIAllocatedToFamilyJ[i][j][k].get(GRB_DoubleAttr_X) - EPSILON) == 1){
						int idxFam = allo.children[i].preferences[j][k];
						allo.assignmentByChild[i] = idxFam;
						allo.assignmentByFamily[idxFam] = i;
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
