#include "main.h"
#include "gale_shapley.h"

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

	//allo.reduction();
	//allo.printProb();
	allo.infos.timeCPUPP =  getCPUTime() - initTimeModelCPU;

	// Model
	try{
		// Local variables
		GRBModel model = GRBModel(env);
		GRBLinExpr objFun = 0;
		
		vector<vector<GRBVar> > isDoctorIAllocatedToHospitalJ (allo.nbDoctors);
		vector<vector<GRBVar> > isHospitalJFullAtRankK (allo.nbHospitals);

		vector<vector<GRBVar> > hIBFD (allo.nbDoctors);
		vector<vector<GRBVar> > hIBFH (allo.nbHospitals);

		// Initialization
		for (int i = 0; i < allo.nbDoctors; i++){
			isDoctorIAllocatedToHospitalJ[i].resize(allo.doctors[i].nbPref);
			hIBFD[i].resize(allo.doctors[i].nbPref);
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				hIBFD[i][j] = model.addVar(0, 1, 0, GRB_BINARY);
				isDoctorIAllocatedToHospitalJ[i][j] = model.addVar(0, 1, 0, GRB_BINARY);
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
				GRBLinExpr exp = isDoctorIAllocatedToHospitalJ[i][j];
				if(j > 0) exp += hIBFD[i][j-1];
				model.addConstr(hIBFD[i][j] == exp);
			}
		}

		for (int j = 0; j < allo.nbHospitals; j++){
			for (int k = 0; k<allo.hospitals[j].nbPref; k++){
				GRBLinExpr exp = 0;
				for(int l=0; l<allo.hospitals[j].preferences[k].size();l++){
					int idxDoc = allo.hospitals[j].preferences[k][l]-1;
					for(int m=0; m < allo.doctors[idxDoc].preferences.size();m++){								
						if(allo.doctors[idxDoc].preferences[m]-1 == j){
							exp += isDoctorIAllocatedToHospitalJ[idxDoc][m];
						}
					}
				}
				if(k > 0) exp += hIBFH[j][k-1];
				model.addConstr(hIBFH[j][k] == exp);
			}
		}

		// Stability constraints
		for (int i = 0; i < allo.nbDoctors; i++){
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				model.addConstr(allo.hospitals[allo.doctors[i].preferences[j] - 1].cap * (1 - hIBFD[i][j]) - hIBFH[allo.doctors[i].preferences[j] - 1][allo.doctors[i].ranks[j]-1] <= 0);
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
			model.addConstr(allo.hospitals[j].cap * isHospitalJFullAtRankK[j][allo.hospitals[j].preferences.size()] -  hIBFH[j][allo.hospitals[j].preferences.size()-1]  <= 0);
		} 

		// Constraint about opening a new rank
		for (int j = 0; j < allo.nbHospitals; j++){
			for(int k=0; k<allo.hospitals[j].preferences.size(); k++){
				for(int i=0; i<allo.hospitals[j].preferences[k].size();i++){
					int idxDoc = allo.hospitals[j].preferences[k][i]-1;
					int idxRank = -1;
					for(int m=0; m < allo.doctors[idxDoc].preferences.size();m++){								
						if(allo.doctors[idxDoc].preferences[m]-1 == j){
							idxRank = m;
						}
					}
					model.addConstr(1 - isHospitalJFullAtRankK[j][k+1] <= hIBFD[idxDoc][idxRank]);
				}
			}
		}
				
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  3600 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);

		for (int j = 0; j < allo.nbHospitals; j++){
			for(int k=0; k<allo.hospitals[j].preferences.size()+1; k++){
				isHospitalJFullAtRankK[j][k].set(GRB_IntAttr_BranchPriority, 10);
			}
		}

		for (int i = 0; i < allo.nbDoctors; i++){
			for (int j = 0; j<allo.doctors[i].nbPref; j++){
				hIBFD[i][j].set(GRB_IntAttr_BranchPriority, 5);
			}
		}

		for (int j = 0; j < allo.nbHospitals; j++){
			for(int k=0; k<allo.hospitals[j].nbPref; k++){
			//	hIBFH[j][k].set(GRB_IntAttr_BranchPriority, 5);
			}
		}

		/****************************
		 * Gale-Shapley starts here *
		 ****************************/

		// #define DEBUG

		#ifdef DEBUG
		#define GSLOG(x) std::cout << x
		#else
		#define GSLOG(x) do {} while (0)
		#endif


				// Pre-solve by running Gale-Shapley
				int galeShapleyRuns = 100;
				std::cout << "Presolving with " << galeShapleyRuns << " Gale-Shapley runs." << std::endl;
				model.update();
				std::vector<std::vector<int>> hospital_matches = gale_shapley(allo, galeShapleyRuns);
				for(unsigned int hosp_nr = 0; hosp_nr < hospital_matches.size(); ++hosp_nr) {
					std::vector<int> matches = hospital_matches[hosp_nr];
					Hospital &hosp = allo.hospitals[hosp_nr];
		#ifdef DEBUG
					hosp.print();
					for(auto match: matches)
						std::cout << match << " ";
					std::cout << std::endl;
		#endif
					int index = 0;
					// Set y^h_j,k to 0 for all hospitals/ranks. We later on update the value
					// where doctors are assigned.
					for(int i = 0; i < hosp.preferences.size(); ++i) {
						GSLOG("set hIBFH[" << hosp_nr << "][" << i << "] to 0" << std::endl);
						hIBFH[hosp_nr][i].set(GRB_DoubleAttr_Start, 0);
					}
					while (index < matches.size()) {
						int match = matches[index];
						// not sure which variable this corresponds to, as it is actually "is
						// doctor I allocated to their j'th preference"
						for(int j = 0; j < allo.doctors[match].preferences.size(); ++j) {
							isDoctorIAllocatedToHospitalJ[match][j].set(GRB_DoubleAttr_Start, 0);
							if(allo.doctors[match].preferences[j] == (hosp_nr+1)) { // preferences start at 1, arrays at 0.
								GSLOG("set isDoctorIAllocatedToHospitalJ[" << match << "][" << j << "] to 1" << std::endl);
								isDoctorIAllocatedToHospitalJ[match][j].set(GRB_DoubleAttr_Start, 1);
							}
						}
						// Assign y^d_i,k
						{
							int ind;
							// Set y^d_i,k to start at 1, until we find this hospital
							for(ind = 0; ind < allo.doctors[match].preferences.size(); ++ind) {
								GSLOG("set hIBFD[" << match << "][" << ind << "] to 0" << std::endl);
								hIBFD[match][ind].set(GRB_DoubleAttr_Start, 0);
								if(allo.doctors[match].preferences[ind] == (hosp_nr+1))
									break;
							}
							// Then set all further y^d_i,k to start at 0
							for(; ind < allo.doctors[match].preferences.size(); ++ind) {
								GSLOG("set hIBFD[" << match << "][" << ind << "] to 1" << std::endl);
								hIBFD[match][ind].set(GRB_DoubleAttr_Start, 1);
							}
						}
						// Now for y^h_j,k. First find out at what rank this doctor is added
						int rank;
						for(rank = 0; rank < hosp.preferences.size(); ++rank) {
							std::vector<int> &prefs = hosp.preferences[rank];
							if(std::find(prefs.begin(), prefs.end(), match+1) != prefs.end()) { // doctors in preferences start at 1
								break;
							}
						}
						// For everything after this, set y^h_j,k to (index+1). Remember
						// index=0 is the first doctor allocated.
						for(int i = rank; i < hosp.preferences.size(); ++i) {
							GSLOG("set hIBHF[" << hosp_nr << "][" << i << "] to " << (index+1) << std::endl);
							hIBFH[hosp_nr][i].set(GRB_DoubleAttr_Start, index+1);
						}
						index++;
					}
					// set z_j,k. These are only set if a hospital is at capacity at some
					// rank
					if (index == hosp.cap) {
						int lastDoctor = matches.back() + 1; // doctors in preferences start at 1
						unsigned int lastRank = 0;
						std::vector<int> &hosp_rank = hosp.preferences[lastRank];
						while(std::find(hosp_rank.begin(), hosp_rank.end(), lastDoctor) == hosp_rank.end()) {
							GSLOG("set isHospitalJFullAtRankK[" << hosp_nr << "][" << lastRank << "] to 0" << std::endl);
							isHospitalJFullAtRankK[hosp_nr][lastRank].set(GRB_DoubleAttr_Start, 0);
							lastRank++;
							hosp_rank = hosp.preferences[lastRank];
						}
						// Doctor in rank lastRank is assigned
						GSLOG("set isHospitalJFullAtRankK[" << hosp_nr << "][" << lastRank << "] to 0" << std::endl);
						isHospitalJFullAtRankK[hosp_nr][lastRank].set(GRB_DoubleAttr_Start, 0);
						lastRank++;
						// No doctor of higher rank is in this assignment
						while(lastRank < hosp.preferences.size()+1) {
							GSLOG("set isHospitalJFullAtRankK[" << hosp_nr << "][" << lastRank << "] to 1" << std::endl);
							isHospitalJFullAtRankK[hosp_nr][lastRank].set(GRB_DoubleAttr_Start, 1);
							lastRank++;
						}
					} else {
						for(unsigned int i = 0; i < hosp.preferences.size()+1; ++i) {
							GSLOG("set isHospitalJFullAtRankK[" << hosp_nr << "][" << i << "] to 0" << std::endl);
							isHospitalJFullAtRankK[hosp_nr][i].set(GRB_DoubleAttr_Start, 0);
						}
					}
				}

		/****************************
		 * Gale-Shapley ends here   *
		 ****************************/

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
