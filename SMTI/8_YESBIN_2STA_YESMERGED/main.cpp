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
	allo.load(path, filein);
	allo.printProb();

	manlove(allo);
	
	allo.printSol();
	allo.checkSolution();
	allo.printInfo(pathAndFileout);
}


int manlove(Allocation& allo){
	double initTimeModelCPU = getCPUTime();
	GRBEnv env = GRBEnv();

	double initTimeModelCPUPP = getCPUTime();
	allo.reduction();
	allo.printProb();
	allo.infos.timeCPUPP =  getCPUTime() - initTimeModelCPUPP;

	// Model
	try{
		// Local variables
		GRBModel model = GRBModel(env);
		GRBLinExpr objFun = 0;
		
		vector<vector<vector<GRBVar> > > isChildIAllocatedToFamilyJ (allo.nbChildren);

		vector<vector<GRBVar> > hIBFF (allo.nbFamilies);
		vector<vector<GRBVar> > hIBFC (allo.nbChildren);

		// Initialization
		for (int i = 0; i < allo.nbChildren; i++){
			isChildIAllocatedToFamilyJ[i].resize(allo.children[i].nbPref);
			hIBFC[i].resize(allo.children[i].nbPref);
			for (int j = 0; j<allo.children[i].nbPref; j++){
				hIBFC[i][j] = model.addVar(0, 1, 0, GRB_BINARY);
				isChildIAllocatedToFamilyJ[i][j].resize(allo.children[i].preferences[j].size());
				for (int k = 0; k<allo.children[i].preferences[j].size(); k++){
					isChildIAllocatedToFamilyJ[i][j][k] = model.addVar(0, 1, 0, GRB_BINARY);
				}
			}
		}

		for (int j = 0; j < allo.nbFamilies; j++){
			hIBFF[j].resize(allo.families[j].nbPref);
			for(int k=0; k<allo.families[j].nbPref; k++){
				hIBFF[j][k] = model.addVar(0, 1, 0, GRB_BINARY);
			}
		}

		model.update();

		// Objective function
		for (int i = 0; i < allo.nbChildren; i++){
			/* if(allo.children[i].nbPref > 0)
				objFun += hIBFC[i].back(); */
			if(allo.children[i].nbPref > 0){
				float mult = 0; 
				if(allo.children[i].preferences[0].size() > 0) 
					mult = 1;
				objFun += hIBFC[i][0] * mult; 	
				for (int j = 1; j<allo.children[i].nbPref; j++){
					mult = 0; 
					if(allo.children[i].preferences[j].size() > 0) 
						mult = 1;
					objFun += (hIBFC[i][j] - hIBFC[i][j-1]) * mult; 			
				}
			}
		}
		model.setObjective(objFun, GRB_MAXIMIZE);

		// hIBF values
		for (int i = 0; i < allo.nbChildren; i++){
			for (int j = 0; j<allo.children[i].nbPref; j++){
				GRBLinExpr exp = 0;
				for(int k=0; k<allo.children[i].preferences[j].size();k++){
					exp += isChildIAllocatedToFamilyJ[i][j][k];
				}
				if(j > 0) exp += hIBFC[i][j-1];
				model.addConstr(hIBFC[i][j] == exp);
			}
		}

		for (int j = 0; j < allo.nbFamilies; j++){
			for (int k = 0; k<allo.families[j].nbPref; k++){
				GRBLinExpr exp = 0;
				for(int l=0; l<allo.families[j].preferences[k].size();l++){
					int idxChi = allo.families[j].preferences[k][l];
					int idxRank = allo.families[j].ranks[k][l];
					int idxPos = allo.families[j].positions[k][l];
					exp += isChildIAllocatedToFamilyJ[idxChi][idxRank][idxPos];
				}
				if(k > 0) exp += hIBFF[j][k-1];
				model.addConstr(hIBFF[j][k] == exp);
			}
		}


		// Stability constraints
		for (int i = 0; i < allo.nbChildren; i++){
			for (int j = 0; j<allo.children[i].nbPref; j++){
				GRBLinExpr exp = 0;
				for(int k=0; k<allo.children[i].preferences[j].size();k++){
					int idxFam = allo.children[i].preferences[j][k];
					int idxRank = allo.children[i].ranks[j][k];
					exp += hIBFF[idxFam][idxRank];
				}
				model.addConstr(allo.children[i].preferences[j].size() * (1 - hIBFC[i][j]) <= exp);
			}	
		} 

		// Stability constraints
		for (int i = 0; i < allo.nbFamilies; i++){
			for (int j = 0; j<allo.families[i].nbPref; j++){
				GRBLinExpr exp = 0;
				for(int k=0; k<allo.families[i].preferences[j].size();k++){
					int idxChi = allo.families[i].preferences[j][k];
					int idxRank = allo.families[i].ranks[j][k];
					exp += hIBFC[idxChi][idxRank];				
				}
				model.addConstr(allo.families[i].preferences[j].size() * (1 - hIBFF[i][j]) <= exp);
			}
		}
		
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit, 3600 - (getCPUTime() - initTimeModelCPU));
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
