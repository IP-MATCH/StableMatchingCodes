#include "Allocation.h" 

/*	*************************************************************************************
	***********************************  DOCTOR *****************************************
	************************************************************************************* */

void Doctor::print(){
	cout << "Doctor " << id << "\t Preferences (" << nbPref << ") hospital [ranks] \t";
	for(int i=0; i<nbPref; i++) cout << preferences[i] << " [" << ranks[i] << "]\t" ;
	cout << endl;
}

/*	*************************************************************************************
	*********************************** HOSPITAL ****************************************
	************************************************************************************* */

void Hospital::print(){
	cout << "Hospital " << id << "\t Capacity " << cap << "\t Preferences (" << nbPref << " groups, " << nbTotPref << " in total)\t";
	for(int i=0; i<nbPref; i++){
		cout << "(";
			for(int j=0; j<preferences[i].size(); j++){
				if(j > 0) cout << " ";
				cout << preferences[i][j];
			}
			cout << ") "; 
	}
	cout << endl;
}

/*	*************************************************************************************
	********************************** ALLOCATION ***************************************
	************************************************************************************* */

void Allocation::load(const string& path, const string& filein){
	// Local variables 
	istringstream iss;
	string parser;
	string garbage;
	string nameFile = path + filein;

	// File opening
	ifstream file(nameFile.c_str(), ios::in);

	// File lecture
	if (file){
		// Name of the instance is filein
		name = filein;

		// Skip the first line
		getline(file, parser); iss.str(parser); iss >> garbage; iss.clear();

		// Read the number of doctors
		getline(file, parser); iss.str(parser); iss >> nbDoctors; iss.clear(); 

		// Read the number of hospitals
		getline(file, parser); iss.str(parser); iss >> nbHospitals; iss.clear();

		// Read the preferences of each doctors
		for (int i = 0; i < nbDoctors; i++){
			Doctor d;
			int temp;

			d.nbPref = 0;
			
			getline(file, parser);
			iss.str(parser);
			iss >> d.id;
			while(iss >> temp){
				d.preferences.push_back(temp);
				d.ranks.push_back(-1);
				d.nbPref++;
			}
			doctors.push_back(d);	
			iss.clear();
		}

		// Read the preferences of each hospital
		for (int i = 0; i < nbHospitals; i++){
			Hospital h;
			int temp;
			istringstream tempIss;
			string tempString;
			
			h.nbTotPref = 0;

			getline(file, parser);
			iss.str(parser);
			iss >> h.id; 
			iss >> h.cap;

			int currRank = 1;

			for(;;){
				char tempChar = iss.get();
				//cout << "Just read " << tempChar << endl;
				if(iss.eof()) 
					break;
				else{
					if(tempChar == '('){
						getline(iss, tempString, ')');
						tempIss.str(tempString);
						vector<int> tempPref;
						while(tempIss >> temp){
							tempPref.push_back(temp);
							for(int j=0; j<doctors[temp-1].nbPref; j++){
								if(doctors[temp-1].preferences[j] == i+1)
									doctors[temp-1].ranks[j] = currRank;
							}
						}
						h.preferences.push_back(tempPref);
						currRank++;
						h.nbTotPref+= tempPref.size();
						tempIss.clear();
					}
					else{
						if((tempChar >= '0') && (tempChar <= '9')){
							iss.putback(tempChar);
							vector<int> tempPref;
							iss >> temp;
							tempPref.push_back(temp);
							for(int j=0; j<doctors[temp-1].nbPref; j++){
								if(doctors[temp-1].preferences[j] == i+1)
									doctors[temp-1].ranks[j] = currRank;
							}
							h.preferences.push_back(tempPref);
							currRank++;
							h.nbTotPref+= tempPref.size();
							tempIss.clear();
						}
					}
				}
			}

			h.nbPref = h.preferences.size();
			hospitals.push_back(h);
			iss.clear();
		}

		file.close();
	}
	else cout << "Could not open the file " << nameFile << endl;
}

void Allocation::printProb(){
	cout << "Instance " << name << endl;
	for(int i=0; i<nbDoctors; i++){
		doctors[i].print();
	}
	for(int i=0; i<nbHospitals; i++){
		hospitals[i].print();
	}
}

int Allocation::reductionHosOff(){
	
	int nbTotRem = 0;
	vector<int> remainingCap (nbHospitals); 
	vector<int> currentAssignmentByDoctor (nbDoctors,-1);
	vector<int> maxRank (nbHospitals,-1);
	bool hbm = true;

	for(int j=0; j<nbHospitals;j++){
		remainingCap[j] = hospitals[j].cap;
	}
	
	// Loop - doctor assigned to hospital index in the table, not hospital id

	while(hbm){
		hbm = false;
		for(int j=0; j<nbHospitals;j++){
			for(int k=maxRank[j]+1; k<hospitals[j].nbPref; k++){
				if(remainingCap[j] < hospitals[j].preferences[k].size()) break;
				hbm = true;
				maxRank[j]++;

				for(int i=0; i< hospitals[j].preferences[k].size();i++){
					int idxDoc = hospitals[j].preferences[k][i] - 1;
					if(currentAssignmentByDoctor[idxDoc] != -1) remainingCap[currentAssignmentByDoctor[idxDoc]]++ ;
					currentAssignmentByDoctor[idxDoc] = j;

					// Remove hospital after j from doctor preferences
					for(int l=0; l<doctors[idxDoc].nbPref; l++){
						if(doctors[idxDoc].preferences[l]-1 == j){
							for(int m=l+1 ; m<doctors[idxDoc].nbPref; m++){
								int idxHos = doctors[idxDoc].preferences[m]-1;
								for(int n=0; n<hospitals[idxHos].preferences[doctors[idxDoc].ranks[m]-1].size();n++){								
									if(hospitals[idxHos].preferences[doctors[idxDoc].ranks[m]-1][n]-1 == idxDoc){
										hospitals[idxHos].preferences[doctors[idxDoc].ranks[m]-1].erase(hospitals[idxHos].preferences[doctors[idxDoc].ranks[m]-1].begin() + n); 
										hospitals[idxHos].nbTotPref--;
										nbTotRem++;
									}
								}
							}
							doctors[idxDoc].nbPref = l+1;
							doctors[idxDoc].preferences.resize(l+1);
							doctors[idxDoc].ranks.resize(l+1);		
						}
					}
				}
				remainingCap[j] -= hospitals[j].preferences[k].size();
			}
		}
	}
	
	return nbTotRem;
}

int Allocation::reductionResApp(){

	int nbTotRem = 0;
	vector<int> capacityUsed (nbHospitals, 0); 
	vector<vector<vector<int> > > currentAssignmentByHospital (nbHospitals);
	vector<int> currentAssignmentByDoctor (nbDoctors,-1);
	vector<int> maxRank (nbHospitals,-1);
	bool hbm = true;

	for(int j=0; j<nbHospitals;j++)
		currentAssignmentByHospital[j].resize(hospitals[j].nbPref);
	
	// Loop - doctor assigned to hospital index in the table, not hospital id

	while(hbm){
		hbm = false;
		for(int i=0; i<nbDoctors;i++){
			if(currentAssignmentByDoctor[i] == -1 && doctors[i].preferences.size() > 0){
				hbm = true;
				int idxHos = doctors[i].preferences[0]-1;
				currentAssignmentByDoctor[i] = idxHos;
				currentAssignmentByHospital[idxHos][doctors[i].ranks[0]-1].push_back(i);
				capacityUsed[idxHos]++;
				if(capacityUsed[idxHos] >= hospitals[idxHos].cap){
					int count = 0;
					for(int k=0;k<hospitals[idxHos].nbPref;k++){
						count += currentAssignmentByHospital[idxHos][k].size();
						if(count >= hospitals[idxHos].cap){
							for(int l = k+1; l<hospitals[idxHos].nbPref;l++){
								for(int m=0; m<currentAssignmentByHospital[idxHos][l].size();m++){
									currentAssignmentByDoctor[currentAssignmentByHospital[idxHos][l][m]] = -1;
									capacityUsed[idxHos]--;
								}
								for(int m=0; m<hospitals[idxHos].preferences[l].size();m++){
									int idxDoc = hospitals[idxHos].preferences[l][m] - 1;
									for(int n=0; n < doctors[idxDoc].preferences.size();n++){								
										if(doctors[idxDoc].preferences[n]-1 == idxHos){
											doctors[idxDoc].preferences.erase(doctors[idxDoc].preferences.begin() + n); 
											doctors[idxDoc].ranks.erase(doctors[idxDoc].ranks.begin() + n); 
											doctors[idxDoc].nbPref--;
											nbTotRem++;
										}
									}
								}
								hospitals[idxHos].nbTotPref -= 	hospitals[idxHos].preferences[l].size()	;
							}
							hospitals[idxHos].nbPref = k+1;
							hospitals[idxHos].preferences.resize(k+1);
							currentAssignmentByHospital[idxHos].resize(k+1);
						}
					}
				}
			}
		}
	}

	return nbTotRem;
}

void Allocation::polish(){
	// Remove unused groups 
	for(int j=0; j<nbHospitals;j++){
		vector<vector<int> > preferences;
		int idxRem = 0;
		for(int k=0;k<hospitals[j].nbPref;k++){
			if(hospitals[j].preferences[k].size() == 0){
				idxRem++;
			}
			else{
				preferences.push_back(hospitals[j].preferences[k]);
				for(int i=0;i<hospitals[j].preferences[k].size();i++){
					int idxDoc = hospitals[j].preferences[k][i] - 1;
					for(int l=0;l<doctors[idxDoc].nbPref;l++){
						if(doctors[idxDoc].preferences[l]-1 == j){
							doctors[idxDoc].ranks[l] -= idxRem;
						}
					}
				}
			}
		}
		hospitals[j].preferences = preferences;
		hospitals[j].nbPref = preferences.size();
	}
}

void Allocation::reduction(){
	int nbRed1 = 0;
	int nbRed2 = 0;
	int i = 0;
	do{
		nbRed1 = reductionHosOff();
		nbRed2 = reductionResApp();
		cout << "Reduction iteration " << i << " reductionHosOff " << nbRed1 << " reductionResApp " << nbRed2 << endl;
		i++;
	}while(nbRed1 + nbRed2 != 0);
	polish();
}

void Allocation::printSol(){
	cout << "Allocation by hospitals" << endl;
	for(int i=0; i<nbHospitals; i++){
		cout << i+1 << ":";
		for(int j=0; j<assignmentByHospital[i].size();j++){
			cout << "\t" << assignmentByHospital[i][j];
		}
		cout << endl;
	}
}

void Allocation::printInfo(const string& pathAndFileout){
	string nameFile = pathAndFileout;
	std::ofstream file(nameFile.c_str(), std::ios::out | std::ios::app);
	file << name << "\t" << infos.opt << "\t" << infos.timeCPU << "\t" << infos.timeCPUPP << "\t"<< infos.LB << "\t" << infos.UB << "\t" << infos.contUB << "\t" << infos.nbVar << "\t" << infos.nbCons << "\t" << infos.nbNZ 
		<< "\t" << infos.contUB2 << "\t" << infos.nbVar2 << "\t" << infos.nbCons2 << "\t" << infos.nbNZ2  << endl;
	file.close();
}

void Allocation::checkSolution(){

	bool mistake = false;
	vector<int> maxRankHospital (nbHospitals, -1);
	vector<int> capacityHospital (nbHospitals, 0); 
	int solByDoctor = 0;

	vector<int> doctorAllocated (nbDoctors, 0);

	// Get the maxRank of the hospital and the number of doctors allocated to each hospital
	for(int i=0; i<nbDoctors; i++){
		for(int j=0; j<doctors[i].nbPref; j++){
			if(assignmentByDoctor[i] == doctors[i].preferences[j]){
				maxRankHospital[doctors[i].preferences[j]-1] = max(maxRankHospital[doctors[i].preferences[j]-1], doctors[i].ranks[j]);
				capacityHospital[doctors[i].preferences[j]-1]++ ;
				solByDoctor++;
			}
		}
	}

	// Check for stability
	for(int i=0; i<nbDoctors; i++){
		int j = 0;
		while(j < doctors[i].nbPref && assignmentByDoctor[i] != doctors[i].preferences[j]){
			if(maxRankHospital[doctors[i].preferences[j]-1] > doctors[i].ranks[j]){
				cout << "Stability error: blocking pair Hospital " << doctors[i].preferences[j] << " (rank " << maxRankHospital[doctors[i].preferences[j]-1] << ") Patient "
					<< i+1 << "(rank" << doctors[i].ranks[j] << ")"<< endl;
				mistake = true;
			}
			j++;
		}
	}

	// Check for capacity
	for(int i=0; i<nbHospitals; i++){
		if(capacityHospital[i] > hospitals[i].cap){
			cout << "Capacity error: capacity overloaded Hospital " << i+1 << ": " << hospitals[i].cap << " allowed and " << capacityHospital[i] << " used "<< endl;
			mistake = true;
		}
	}

	// Check for solution
	if(solByDoctor != infos.LB){
		cout << "Solution error: solution value and doctors do not match: " << infos.LB << " solution " << solByDoctor << " doctors allocated "<< endl;
		mistake = true;
	}

	// Check for doctors used twice 
	for(int i=0; i<nbHospitals; i++){
		for(int j = 0; j< assignmentByHospital[i].size(); j++){
			doctorAllocated[assignmentByHospital[i][j] - 1]++;
			if(doctorAllocated[assignmentByHospital[i][j] - 1] > 1){
				cout << "Allocation error: doctor " << assignmentByHospital[i][j] << " is allocated " << doctorAllocated[assignmentByHospital[i][j] - 1] << " times " << endl;
				mistake = true;
			}
		}
	}

	// If no mistake
	if(!mistake) cout << "Solution is fine !" << endl;
}

