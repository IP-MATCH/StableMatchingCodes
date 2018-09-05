#include "Allocation.h" 

/*	*************************************************************************************
	***********************************  DOCTOR *****************************************
	************************************************************************************* */

void Child::print(){
	cout << "Child " << id << "\t Preferences (" << nbPref << " groups, " << nbTotPref << " in total)\t";
	for(int i=0; i<nbPref; i++){
		cout << "(";
			for(int j=0; j<preferences[i].size(); j++){
				if(j > 0) cout << " ";
				cout << preferences[i][j];
				cout << "[" << ranks[i][j] << "_" << positions[i][j] <<  "]";
			}
			cout << ") "; 
	}
	cout << endl;
}

/*	*************************************************************************************
	*********************************** HOSPITAL ****************************************
	************************************************************************************* */

void Family::print(){
	cout << "Family " << id << "\t Preferences (" << nbPref << " groups, " << nbTotPref << " in total)\t";
	for(int i=0; i<nbPref; i++){
		cout << "(";
			for(int j=0; j<preferences[i].size(); j++){
				if(j > 0) cout << " ";
				cout << preferences[i][j];
				cout << "[" << ranks[i][j] << "_" << positions[i][j] <<  "]";
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
	vector<vector<int> > allRanksC, allRanksF, allPreferencesC, allPreferencesF;

	// File opening
	ifstream file(nameFile.c_str(), ios::in);

	// File lecture
	if (file){
		// Name of the instance is filein
		name = filein;

		// Skip the first line
		getline(file, parser); iss.str(parser); iss >> garbage; iss.clear();

		// Read the number of doctors
		getline(file, parser); iss.str(parser); iss >> nbFamilies; iss.clear(); 

		// Read the number of hospitals
		getline(file, parser); iss.str(parser); iss >> nbChildren; iss.clear();

		// Resize allRanks and allPreferences
		allRanksC.resize(nbChildren); allPreferencesC.resize(nbChildren); 
		allRanksF.resize(nbFamilies); allPreferencesF.resize(nbFamilies); 

		// Read the preferences of each child
		for (int i = 0; i < nbChildren; i++){
			Child c;
			int temp;
			allRanksC[i].resize(nbFamilies,-1); allPreferencesC[i].resize(nbFamilies,-1); 

			istringstream tempIss;
			string tempString;
			c.nbTotPref = 0;
			int rank = 0;

			getline(file, parser);
			iss.str(parser);
			iss >> c.id; 

			for(;;){
				char tempChar = iss.get();
				if(iss.eof()) 
					break;
				else{
					if(tempChar == '('){
						int pref = 0;
						getline(iss, tempString, ')');
						tempIss.str(tempString);
						vector<int> tempPref;
						while(tempIss >> temp){
							allRanksC[i][temp-1] = rank;
							allPreferencesC[i][temp-1] = pref;
							tempPref.push_back(temp-1);
							pref++;
						}
						c.preferences.push_back(tempPref);
						c.nbTotPref+= tempPref.size();
						rank++;
						tempIss.clear();
					}
					else{
						if((tempChar >= '0') && (tempChar <= '9')){
							iss.putback(tempChar);
							vector<int> tempPref;
							iss >> temp;
							allRanksC[i][temp-1] = rank;
							allPreferencesC[i][temp-1] = 0;
							tempPref.push_back(temp-1);
							c.preferences.push_back(tempPref);
							c.nbTotPref+= tempPref.size();
							rank++;
							tempIss.clear();
						}
					}
				}
			}
			
			c.nbPref = c.preferences.size();
			children.push_back(c);
			iss.clear();
		}

		// Read the preferences of each family
		for (int i = 0; i < nbFamilies; i++){
			Family f;
			int temp;
			allRanksF[i].resize(nbChildren,-1); allPreferencesF[i].resize(nbChildren,-1); 

			istringstream tempIss;
			string tempString;
			
			f.nbTotPref = 0;
			int rank = 0;

			getline(file, parser);
			iss.str(parser);
			iss >> f.id; 
			iss >> garbage;

			for(;;){
				char tempChar = iss.get();
				//cout << "Just read " << tempChar << endl;
				if(iss.eof()) 
					break;
				else{
					if(tempChar == '('){
						int pref = 0;
						getline(iss, tempString, ')');
						tempIss.str(tempString);
						vector<int> tempPref;
						while(tempIss >> temp){
							allRanksF[i][temp-1] = rank;
							allPreferencesF[i][temp-1] = pref;
							tempPref.push_back(temp-1);
							pref++;
						}
						f.preferences.push_back(tempPref);
						f.nbTotPref+= tempPref.size();
						rank++;
						tempIss.clear();
					}
					else{
						if((tempChar >= '0') && (tempChar <= '9')){
							iss.putback(tempChar);
							vector<int> tempPref;
							iss >> temp;
							allRanksF[i][temp-1] = rank;
							allPreferencesF[i][temp-1] = 0;
							tempPref.push_back(temp-1);
							f.preferences.push_back(tempPref);
							f.nbTotPref+= tempPref.size();
							rank++;
							tempIss.clear();
						}
					}
				}
			}

			f.nbPref = f.preferences.size();
			families.push_back(f);
			iss.clear();
		}

		file.close();

		for (int i = 0; i < nbChildren; i++){
			children[i].ranks = children[i].preferences;
			children[i].positions = children[i].preferences;
		}

		for (int i = 0; i < nbFamilies; i++){
			families[i].ranks = families[i].preferences;
			families[i].positions = families[i].preferences;
		}

		// Fill ranks and positions
		
		for (int i = 0; i < nbChildren; i++){
			for(int k= 0; k < children[i].nbPref; k++){
				for(int l=0; l<children[i].preferences[k].size();l++){
					int idxFam = children[i].preferences[k][l];
					children[i].ranks[k][l] = allRanksF[idxFam][i];
					children[i].positions[k][l] =  allPreferencesF[idxFam][i];
				}
			}
		}

		for (int i = 0; i < nbFamilies; i++){
			for(int k= 0; k < families[i].nbPref; k++){
				for(int l=0; l<families[i].preferences[k].size();l++){
					int idxChi = families[i].preferences[k][l];
					families[i].ranks[k][l] = allRanksC[idxChi][i];
					families[i].positions[k][l] =  allPreferencesC[idxChi][i];
				}
			}
		}


	}
	else cout << "Could not open the file " << nameFile << endl;
}

void Allocation::printProb(){
	cout << "Instance " << name << endl;
	for(int i=0; i<nbChildren; i++){
		children[i].print();
	}
	for(int i=0; i<nbFamilies; i++){
		families[i].print();
	}
}

int Allocation::reductionFam1(){
	
	// Step 1 -- Create map
	
	int count = 0;
	map<vector<int>, vector<int> >::iterator it;
	map<vector<int>, vector<int> > mapFirstChoice;
	for(int j=0; j<nbFamilies;j++){
		vector<int> temp(nbChildren,0);
		if(families[j].nbPref > 0){ 
			for(int k=0;k<families[j].preferences[0].size();k++){
				temp[families[j].preferences[0][k]] = 1;
			}
			it = mapFirstChoice.find(temp);
			if(it == mapFirstChoice.end()){
				pair<vector<int>, vector<int> > add;
				add.first = temp;
				add.second.push_back(j);
				mapFirstChoice.insert(add);
			}
			else{
				(*it).second.push_back(j);
			}
		}
		else count++;
	}

/*	for(it = mapFirstChoice.begin(); it!= mapFirstChoice.end(); ++it){
		cout << (*it).second.size() << " x (" << families[(*it).second[0]].preferences[0].size() << ") [";
		for(int i=0;i<(*it).second.size();i++){
			cout << (*it).second[i] << " ";
		}
		cout << "] == [";
		for(int i=0;i<(*it).first.size();i++){
			cout << (*it).first[i] << " ";
		}
		cout << "]" << endl;
	}

	cout << "Count = " << count << endl;*/

	// Step 2 -- See worst rank

	vector<int> worstRank(nbChildren);
	for(int i=0;i<nbChildren;i++){
		worstRank[i] = children[i].nbPref - 1;
	}

	int co = 0;
	for(it = mapFirstChoice.begin(); it!= mapFirstChoice.end(); ++it){
		if((*it).second.size() >= families[(*it).second[0]].preferences[0].size() ){
		//	cout << "Updates from " << co << endl;; co++;
			for(int i=0; i < families[(*it).second[0]].preferences[0].size();i++){
				multiset<int> orderedRanks;
				multiset<int>::iterator it2; 			
				for(int j=0; j<(*it).second.size();j++){
					orderedRanks.insert(families[(*it).second[j]].ranks[0][i]);
				}
			//	cout << "worstRank of "  << families[(*it).second[0]].preferences[0][i] << " updated from " << worstRank[families[(*it).second[0]].preferences[0][i]];
				it2 = orderedRanks.begin();
				advance(it2, families[(*it).second[0]].preferences[0].size() - 1);
				worstRank[families[(*it).second[0]].preferences[0][i]]=min(worstRank[families[(*it).second[0]].preferences[0][i]], *(it2));
			//	cout << " to " << worstRank[families[(*it).second[0]].preferences[0][i]] << endl;
			}
		}
	}

	// Step 3 -- Remove families after worst rank from child's preferences

	int nbTotRem = 0;

	for(int i=0; i<nbChildren;i++){
		for(int k= worstRank[i] + 1; k < children[i].nbPref; k++){
			nbTotRem += children[i].preferences[k].size();
			children[i].nbTotPref -= children[i].preferences[k].size();
			for(int l=0; l<children[i].preferences[k].size();l++){
				int idxFam = children[i].preferences[k][l];
				int idxPos = children[i].positions[k][l];
				int idxRank = children[i].ranks[k][l];
				for(int m = idxPos+1; m < families[idxFam].preferences[idxRank].size();m++){
					int idxF2 = children[families[idxFam].preferences[idxRank][m]].positions[families[idxFam].ranks[idxRank][m]][families[idxFam].positions[idxRank][m]]--;
				}
				families[idxFam].nbTotPref--;
				families[idxFam].positions[idxRank].erase(families[idxFam].positions[idxRank].begin() + idxPos);
				families[idxFam].ranks[idxRank].erase(families[idxFam].ranks[idxRank].begin() + idxPos);
				families[idxFam].preferences[idxRank].erase(families[idxFam].preferences[idxRank].begin() + idxPos);
			}
		}
		children[i].nbPref = worstRank[i] + 1;
		children[i].preferences.resize(children[i].nbPref);
		children[i].ranks.resize(children[i].nbPref);
		children[i].positions.resize(children[i].nbPref);
	}
	
	polish();
	return nbTotRem;
}

int Allocation::reductionChi1(){
	
	// Step 1 -- Create map
	
	int count = 0;
	map<vector<int>, vector<int> >::iterator it;
	map<vector<int>, vector<int> > mapFirstChoice;
	for(int j=0; j<nbChildren;j++){
		vector<int> temp(nbFamilies,0);
		if(children[j].nbPref > 0){ 
			for(int k=0;k<children[j].preferences[0].size();k++){
				temp[children[j].preferences[0][k]] = 1;
			}
			it = mapFirstChoice.find(temp);
			if(it == mapFirstChoice.end()){
				pair<vector<int>, vector<int> > add;
				add.first = temp;
				add.second.push_back(j);
				mapFirstChoice.insert(add);
			}
			else{
				(*it).second.push_back(j);
			}
		}
		else count++;
	}

/*	for(it = mapFirstChoice.begin(); it!= mapFirstChoice.end(); ++it){
		cout << (*it).second.size() << " x (" << children[(*it).second[0]].preferences[0].size() << ") [";
		for(int i=0;i<(*it).second.size();i++){
			cout << (*it).second[i] << " ";
		}
		cout << "] == [";
		for(int i=0;i<(*it).first.size();i++){
			cout << (*it).first[i] << " ";
		}
		cout << "]" << endl;
	}

	cout << "Count = " << count << endl;*/

	// Step 2 -- See worst rank

	vector<int> worstRank(nbFamilies);
	for(int i=0;i<nbFamilies;i++){
		worstRank[i] = families[i].nbPref - 1;
	}

	int co = 0;
	for(it = mapFirstChoice.begin(); it!= mapFirstChoice.end(); ++it){
		if((*it).second.size() >= children[(*it).second[0]].preferences[0].size() ){
		//	cout << "Updates from " << co << endl;; co++;
			for(int i=0; i < children[(*it).second[0]].preferences[0].size();i++){
				multiset<int> orderedRanks;
				multiset<int>::iterator it2; 			
				for(int j=0; j<(*it).second.size();j++){
					orderedRanks.insert(children[(*it).second[j]].ranks[0][i]);
				}
			//	cout << "worstRank of "  << children[(*it).second[0]].preferences[0][i] << " updated from " << worstRank[children[(*it).second[0]].preferences[0][i]];
				it2 = orderedRanks.begin();
				advance(it2, children[(*it).second[0]].preferences[0].size() - 1);
				worstRank[children[(*it).second[0]].preferences[0][i]]=min(worstRank[children[(*it).second[0]].preferences[0][i]], *(it2));
			//	cout << " to " << worstRank[children[(*it).second[0]].preferences[0][i]] << endl;
			}
		}
	}

	// Step 3 -- Remove families after worst rank from child's preferences

	int nbTotRem = 0;

	for(int i=0; i<nbFamilies;i++){
		for(int k= worstRank[i] + 1; k < families[i].nbPref; k++){
			nbTotRem += families[i].preferences[k].size();
			families[i].nbTotPref -= families[i].preferences[k].size();
			for(int l=0; l<families[i].preferences[k].size();l++){
				int idxChi = families[i].preferences[k][l];
				int idxPos = families[i].positions[k][l];
				int idxRank = families[i].ranks[k][l];
				for(int m = idxPos+1; m < children[idxChi].preferences[idxRank].size();m++){
					int idxF2 = families[children[idxChi].preferences[idxRank][m]].positions[children[idxChi].ranks[idxRank][m]][children[idxChi].positions[idxRank][m]]--;
				}
				children[idxChi].nbTotPref--;
				children[idxChi].positions[idxRank].erase(children[idxChi].positions[idxRank].begin() + idxPos);
				children[idxChi].ranks[idxRank].erase(children[idxChi].ranks[idxRank].begin() + idxPos);
				children[idxChi].preferences[idxRank].erase(children[idxChi].preferences[idxRank].begin() + idxPos);
			}
		}
		families[i].nbPref = worstRank[i] + 1;
		families[i].preferences.resize(families[i].nbPref);
		families[i].ranks.resize(families[i].nbPref);
		families[i].positions.resize(families[i].nbPref);
	}
	
	polish();
	return nbTotRem;
}

int Allocation::reductionFam2(){
	
	int nbTotRem = 0;
	vector<int> worstRank(nbChildren);
	for(int i=0;i<nbChildren;i++){
		worstRank[i] = children[i].nbPref - 1;
	}

	for(int i=0; i<nbChildren;i++){
		set<int> allChildren;
		int count = 0;
		for(int j=0; j<children[i].nbPref;j++){
			for(int k=0; k<children[i].preferences[j].size();k++){
				count++;
				int idxFam = children[i].preferences[j][k];
				int idxPos = children[i].positions[j][k];
				int idxRank = children[i].ranks[j][k];
				for(int l = 0; l <= idxRank; l++){
					for(int m=0; m<families[idxFam].preferences[l].size();m++) 
						allChildren.insert(families[idxFam].preferences[l][m]);
				}
			}
			if(count >= allChildren.size()){ 
				worstRank[i] = j;
			//	cout << "worst rank of " << i << " is " << j << endl;
				break;
			}
		}
	}

	for(int i=0; i<nbChildren;i++){
		for(int k= worstRank[i] + 1; k < children[i].nbPref; k++){
			nbTotRem += children[i].preferences[k].size();
			children[i].nbTotPref -= children[i].preferences[k].size();
			for(int l=0; l<children[i].preferences[k].size();l++){
				int idxFam = children[i].preferences[k][l];
				int idxPos = children[i].positions[k][l];
				int idxRank = children[i].ranks[k][l];
				for(int m = idxPos+1; m < families[idxFam].preferences[idxRank].size();m++){
					int idxF2 = children[families[idxFam].preferences[idxRank][m]].positions[families[idxFam].ranks[idxRank][m]][families[idxFam].positions[idxRank][m]]--;
				}
				families[idxFam].nbTotPref--;
				families[idxFam].positions[idxRank].erase(families[idxFam].positions[idxRank].begin() + idxPos);
				families[idxFam].ranks[idxRank].erase(families[idxFam].ranks[idxRank].begin() + idxPos);
				families[idxFam].preferences[idxRank].erase(families[idxFam].preferences[idxRank].begin() + idxPos);
			}
		}
		children[i].nbPref = worstRank[i] + 1;
		children[i].preferences.resize(children[i].nbPref);
		children[i].ranks.resize(children[i].nbPref);
		children[i].positions.resize(children[i].nbPref);
	}
	
	polish();
	return nbTotRem;
}

int Allocation::reductionChi2(){
	
	int nbTotRem = 0;
	vector<int> worstRank(nbFamilies);
	for(int i=0;i<nbFamilies;i++){
		worstRank[i] = families[i].nbPref - 1;
	}

	for(int i=0; i<nbFamilies;i++){
		set<int> allFamilies;
		int count = 0;
		for(int j=0; j<families[i].nbPref;j++){
			for(int k=0; k<families[i].preferences[j].size();k++){
				count++;
				int idxChi = families[i].preferences[j][k];
				int idxPos = families[i].positions[j][k];
				int idxRank = families[i].ranks[j][k];
				for(int l = 0; l <= idxRank; l++){
					for(int m=0; m<children[idxChi].preferences[l].size();m++) 
						allFamilies.insert(children[idxChi].preferences[l][m]);
				}
			}
			if(count >= allFamilies.size()){ 
				worstRank[i] = j;
			//	cout << "worst rank of " << i << " is " << j << endl;
				break;
			}
		}
	}

	for(int i=0; i<nbFamilies;i++){
		for(int k= worstRank[i] + 1; k < families[i].nbPref; k++){
			nbTotRem += families[i].preferences[k].size();
			families[i].nbTotPref -= families[i].preferences[k].size();
			for(int l=0; l<families[i].preferences[k].size();l++){
				int idxChi = families[i].preferences[k][l];
				int idxPos = families[i].positions[k][l];
				int idxRank = families[i].ranks[k][l];
				for(int m = idxPos+1; m < children[idxChi].preferences[idxRank].size();m++){
					int idxF2 = families[children[idxChi].preferences[idxRank][m]].positions[children[idxChi].ranks[idxRank][m]][children[idxChi].positions[idxRank][m]]--;
				}
				children[idxChi].nbTotPref--;
				children[idxChi].positions[idxRank].erase(children[idxChi].positions[idxRank].begin() + idxPos);
				children[idxChi].ranks[idxRank].erase(children[idxChi].ranks[idxRank].begin() + idxPos);
				children[idxChi].preferences[idxRank].erase(children[idxChi].preferences[idxRank].begin() + idxPos);
			}
		}
		families[i].nbPref = worstRank[i] + 1;
		families[i].preferences.resize(families[i].nbPref);
		families[i].ranks.resize(families[i].nbPref);
		families[i].positions.resize(families[i].nbPref);
	}
	
	polish();
	return nbTotRem;
}

void Allocation::polish(){
	// Remove unused groups in families
	for(int j=0; j<nbFamilies;j++){
		vector<vector<int> > preferences;
		vector<vector<int> > ranks;
		vector<vector<int> > positions;
		int idxRem = 0;
		for(int k=0;k<families[j].nbPref;k++){
			if(families[j].preferences[k].size() == 0){
				idxRem++;
			}
			else{
				preferences.push_back(families[j].preferences[k]);
				ranks.push_back(families[j].ranks[k]);
				positions.push_back(families[j].positions[k]);
				for(int i=0;i<families[j].preferences[k].size();i++){
					int idxChi = families[j].preferences[k][i];
					int idxRank = families[j].ranks[k][i];
					int idxPosition = families[j].positions[k][i];
					children[idxChi].ranks[idxRank][idxPosition] -= idxRem;
				}
			}
		}
		families[j].preferences = preferences;
		families[j].ranks = ranks;
		families[j].positions = positions;
		families[j].nbPref = preferences.size();
	}

	// Remove unused groups in children
	for(int j=0; j<nbChildren;j++){
		vector<vector<int> > preferences;
		vector<vector<int> > ranks;
		vector<vector<int> > positions;
		int idxRem = 0;
		for(int k=0;k<children[j].nbPref;k++){
			if(children[j].preferences[k].size() == 0){
				idxRem++;
			}
			else{
				preferences.push_back(children[j].preferences[k]);
				ranks.push_back(children[j].ranks[k]);
				positions.push_back(children[j].positions[k]);
				for(int i=0;i<children[j].preferences[k].size();i++){
					int idxFam = children[j].preferences[k][i];
					int idxRank = children[j].ranks[k][i];
					int idxPosition = children[j].positions[k][i];
					families[idxFam].ranks[idxRank][idxPosition] -= idxRem;
				}
			}
		}
		children[j].preferences = preferences;
		children[j].ranks = ranks;
		children[j].positions = positions;
		children[j].nbPref = preferences.size();
	}
}

void Allocation::reduction(){
	int nbRed1 = 0;
	int nbRed2 = 0;
	int nbRed3 = 0;
	int nbRed4 = 0;
	int i = 0;
	do{
		nbRed1 = reductionFam1();
		nbRed2 = reductionChi1();
		nbRed3 = reductionFam2();
		nbRed4 = reductionChi2();
		cout << "Reduction iteration " << i << " reductionFam1 " << nbRed1 << " reductionChi1 " << nbRed2 << " reductionFam2 " << nbRed3 << " reductionChi2 " << nbRed4 << endl;
		i++;
	}while(nbRed1 + nbRed2 + nbRed3 + nbRed4 != 0);
}

void Allocation::printSol(){
	cout << "Allocation by child" << endl;
	for(int i=0; i<nbChildren; i++){
		cout << i << "\t" << assignmentByChild[i] << endl;
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

	vector<int> childAllocated (nbChildren, 0);
	vector<int> familyAllocated (nbFamilies, 0);

	// Fill the vectors
	for(int i=0; i<nbChildren; i++){
		if(assignmentByChild[i] >= 0) familyAllocated[assignmentByChild[i]]++;
	}
	for(int i=0; i<nbFamilies; i++){
		if(assignmentByFamily[i] >= 0) childAllocated[assignmentByFamily[i]]++;
	}

	// Rank of the family assigned to children and vice versa
	vector<int> rankC(nbChildren, -1);
	for(int i=0;i<nbChildren;i++){
		for(int j=0;j<children[i].nbPref;j++){
			for(int k=0;k<children[i].preferences[j].size();k++){
				if(assignmentByChild[i] == children[i].preferences[j][k]) rankC[i] = j;
			}
		}
	}

	vector<int> rankF(nbFamilies, -1);
	for(int i=0;i<nbFamilies;i++){
		for(int j=0;j<families[i].nbPref;j++){
			for(int k=0;k<families[i].preferences[j].size();k++){
				if(assignmentByFamily[i] == families[i].preferences[j][k]) rankF[i] = j;
			}
		}
	}

	// Check for child used twice
	int sol1 = 0; 
	for(int i=0; i<nbChildren; i++){
		if(childAllocated[i] > 1){
			cout << "Allocation error: child " << i << " is allocated " << childAllocated[i] << " times " << endl;
			mistake = true;
		}
		else sol1 += childAllocated[i];
	}

	// Check for family used twice
	int sol2 = 0; 
	for(int i=0; i<nbFamilies; i++){
		if(familyAllocated[i] > 1){
			cout << "Allocation error: family " << i << " is allocated " << familyAllocated[i] << " times " << endl;
			mistake = true;
		}
		else sol2 += familyAllocated[i];
	}

	
	// Check for stability by child
	for(int i=0; i<nbChildren; i++){
		for(int j=0;j<rankC[i];j++){
			for(int k=0;k<children[i].preferences[j].size();k++){
				int idxFam = children[i].preferences[j][k];
				for(int l=0; l<rankF[idxFam];l++){
					for(int m = 0; m<families[idxFam].preferences[l].size();m++){
						if(families[idxFam].preferences[l][m] == i){
							mistake = true;
							cout << "Stability error: blocking pair family " << idxFam << " child " << i << endl;
						}
					}
				}
			}
		}
	}
			
	// Check for stability by family
	for(int i=0; i<nbFamilies; i++){
		for(int j=0;j<rankF[i];j++){
			for(int k=0;k<families[i].preferences[j].size();k++){
				int idxChi = families[i].preferences[j][k];
				for(int l=0; l<rankC[idxChi];l++){
					for(int m = 0; m<children[idxChi].preferences[l].size();m++){
						if(children[idxChi].preferences[l][m] == i){
							mistake = true;
							cout << "Stability error: blocking pair family " << i << " child " << idxChi << endl;
						}
					}
				}
			}
		}
	}

	// Check for solution
	if(sol1 != sol2){
		cout << "Solution error: solution value do not match: " << sol1 << " children allocated " << sol2 << " families allocated "<< endl;
		mistake = true;
	}

	if(sol1 != infos.LB){
		cout << "Solution error: solution value do not match: " << infos.LB << " solution " << sol1 << " children allocated "<< endl;
		mistake = true;
	}

	// If no mistake
	if(!mistake) cout << "Solution is fine !" << endl;
}
