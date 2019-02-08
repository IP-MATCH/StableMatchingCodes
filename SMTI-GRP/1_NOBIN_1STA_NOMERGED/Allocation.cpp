#include "Allocation.h" 
#include "AgentIterator.h"

/*	*************************************************************************************
	***********************************  DOCTOR *****************************************
	************************************************************************************* */

void Child::print(bool family){
	if (family)
		cout << "Family ";
	else
		cout << "Child ";
	cout << id << "\t Preferences (" << nbPref << " groups, " << nbTotPref << " in total)\t";
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

void Allocation::load(const string& path, const string& filein, const int& threshold){
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
		stringstream ss; ss << threshold;
		name = filein + "_" + ss.str();

		// Read the number of doctors
		getline(file, parser); iss.str(parser); iss >> nbFamilies; iss.clear(); 

		// Read the number of hospitals
		getline(file, parser); iss.str(parser); iss >> nbChildren; iss.clear();

		// Read the grades for each couple family / child.
		grades.resize(nbFamilies);
		for (int i = 0; i < nbFamilies; i++){
			grades[i].resize(nbChildren);
			getline(file, parser);
			iss.str(parser);
			for (int j = 0; j < nbChildren; j++) iss >> grades[i][j];
			iss.clear();
		}

		vector<vector<int> > familyChildRank, familyChildPosition, childFamilyRank, childFamilyPosition; 
		childFamilyRank.resize(nbChildren); childFamilyPosition.resize(nbChildren); 
		for(int i=0;i<nbChildren;i++){
			childFamilyRank[i].resize(nbFamilies); childFamilyPosition[i].resize(nbFamilies,-1); 
		}

		familyChildRank.resize(nbFamilies); familyChildPosition.resize(nbFamilies); 
		for(int i=0;i<nbFamilies;i++){
			familyChildRank[i].resize(nbChildren); familyChildPosition[i].resize(nbChildren,-1); 
		}

		// Create the family
		for (int i = 0; i < nbFamilies; i++){
			set<float> grade; 
			for (int j = 0; j < nbChildren; j++){
				if(grades[i][j] >= threshold) grade.insert(grades[i][j]);
			}
			Family f; f.id = i; f.nbPref = grade.size(); f.nbTotPref = 0; f.preferences.resize(f.nbPref); f.ranks.resize(f.nbPref); f.positions.resize(f.nbPref); 
			vector<float> gradeVector(grade.begin(), grade.end());
			reverse(gradeVector.begin(),gradeVector.end()); 
			for (int j = 0; j < nbChildren; j++){ 
				for(int k=0;k<gradeVector.size();k++){
					if(gradeVector[k] == grades[i][j]){
						f.nbTotPref++;
						familyChildRank[i][j] = k;
						familyChildPosition[i][j] = f.preferences[k].size();
						f.preferences[k].push_back(j);
					}
				}
			}
			families.push_back(f);
		}

		// Create the child
		for (int i = 0; i < nbChildren; i++){
			set<float> grade; 
			for (int j = 0; j < nbFamilies; j++){
				if(grades[j][i] >= threshold) grade.insert(grades[j][i]);
			}
			Child c; c.id = i; c.nbPref = grade.size(); c.nbTotPref = 0; c.preferences.resize(c.nbPref); c.ranks.resize(c.nbPref); c.positions.resize(c.nbPref); 
			vector<float> gradeVector(grade.begin(), grade.end());
			reverse(gradeVector.begin(),gradeVector.end()); 
			for (int j = 0; j < nbFamilies; j++){ 
				for(int k=0;k<gradeVector.size();k++){
					if(gradeVector[k] == grades[j][i]){
						c.nbTotPref++;
						childFamilyRank[i][j] = k;
						childFamilyPosition[i][j] = c.preferences[k].size();
						c.preferences[k].push_back(j);			
					}
				}
			}
			children.push_back(c);
		}

		// Fill ranks and positions
		for (int i = 0; i < nbChildren; i++){
			for (int j = 0; j < children[i].nbPref; j++){
				for (int k = 0; k < children[i].preferences[j].size(); k++){
					int idxFam = children[i].preferences[j][k];
					children[i].ranks[j].push_back(familyChildRank[idxFam][i]);
					children[i].positions[j].push_back(familyChildPosition[idxFam][i]);
				}
			}
		}

		for (int i = 0; i < nbFamilies; i++){
			for (int j = 0; j < families[i].nbPref; j++){
				for (int k = 0; k < families[i].preferences[j].size(); k++){
					int idxChi = families[i].preferences[j][k];
					families[i].ranks[j].push_back(childFamilyRank[idxChi][i]);
					families[i].positions[j].push_back(childFamilyPosition[idxChi][i]);
				}
			}
		}

		file.close();
	}
	else cout << "Could not open the file " << nameFile << endl;
}

void Allocation::printProb(){
	cout << "Instance " << name << endl;
	for(int i=0; i<nbChildren; i++){
		children[i].print(false);
	}
	for(int i=0; i<nbFamilies; i++){
		families[i].print(true);
	}
}

int Allocation::reductionMine(bool children_side, int mode) {
	int nbTotRem = 0;
	int number_here = nbChildren;
	std::vector<Child> * thesep;
	std::vector<Child> * otherp;
	if (children_side) {
		thesep = &children;
		otherp = &families;
	} else {
		thesep = &families;
		otherp = &children;
	}
	std::vector<Child> & these = (*thesep);
	std::vector<Child> & other = (*otherp);

	for (int i = 0; i < number_here; i++) {
		set<int> candidates;
		set<int> positions;
		unsigned int count = 0;
		AgentIterator iter(these[i], candidates, positions, these, other, mode);
		for(std::pair<int, int> p: iter) {
			int j = p.first;
			int k = p.second;
			int idxFam = these[i].preferences[j][k];
			int idxRank = these[i].ranks[j][k];
			positions.insert(idxFam);
			count++;
			for (int l = 0; l <= idxRank; l++) {
				for (unsigned int m = 0; m < other[idxFam].preferences[l].size();
							m++) {
					candidates.insert(other[idxFam].preferences[l][m]);
				}
			}
			if (count >= candidates.size()) {
				for (int k = j + 1; k < these[i].nbPref; k++) {
					nbTotRem += these[i].preferences[k].size();
					std::cout << "Removing from " << i << " : [";
					for(auto pref: these[i].preferences[k]) {
						std::cout << pref << ", ";
					}
					std::cout << "]" << std::endl;
					these[i].nbTotPref -= these[i].preferences[k].size();
					for (unsigned int l = 0; l < these[i].preferences[k].size(); l++) {
						int idxFam = these[i].preferences[k][l];
						int idxPos = these[i].positions[k][l];
						int idxRank = these[i].ranks[k][l];
						for (unsigned int m = idxPos + 1;
									m < other[idxFam].preferences[idxRank].size(); m++) {
							these[other[idxFam].preferences[idxRank][m]]
									.positions[other[idxFam].ranks[idxRank][m]]
														[other[idxFam].positions[idxRank][m]]--;
						}
						other[idxFam].nbTotPref--;
						other[idxFam].positions[idxRank].erase(
								other[idxFam].positions[idxRank].begin() + idxPos);
						other[idxFam].ranks[idxRank].erase(
								other[idxFam].ranks[idxRank].begin() + idxPos);
						other[idxFam].preferences[idxRank].erase(
								other[idxFam].preferences[idxRank].begin() + idxPos);
					}
				}
				these[i].nbPref = j + 1;
				these[i].preferences.resize(these[i].nbPref);
				these[i].ranks.resize(these[i].nbPref);
				these[i].positions.resize(these[i].nbPref);
				break;
			}
		}
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

void Allocation::reduction(int mode){
	total_reduced = 0;
	int i = 0;
	int num = 0;
	do{
		num = reductionMine(true, mode);
		num += reductionMine(false, mode);
		cout << "Iteration " << i << " in mode " << mode << " removed " << num << std::endl;
		i++;
		total_reduced += num;
	}while(num != 0);
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
	file << name << "\t" << infos.opt << "\t" << infos.timeCPU << "\t" << infos.timeCPUPP << "\t" << total_reduced << "\t"<< infos.LB << "\t" << infos.UB << "\t" << infos.altInfo << "\t" << infos.contUB << "\t" << infos.nbVar << "\t" << infos.nbCons << "\t" << infos.nbNZ 
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

	// Check for stability by child
	for(int i=0; i<nbChildren; i++){
		if(assignmentByChild[i] >= 0){
			for(int j=0;j<children[i].nbPref;j++){
				bool cont = true;
				for(int k=0;k<children[i].preferences[j].size();k++){
					if(children[i].preferences[j][k] == assignmentByChild[i]) cont = false;
				}
				if(!cont) break;
				for(int k=0;k<children[i].preferences[j].size();k++){
					int idxFam = children[i].preferences[j][k];
					if(grades[idxFam][assignmentByFamily[idxFam]] < grades[assignmentByChild[i]][i]){
						mistake = true;
						cout << "Stability error: blocking pair family " << idxFam << " child " << assignmentByFamily[idxFam] << " has a worse grade ( " 
							<< grades[idxFam][assignmentByFamily[idxFam]] << ") than family " << assignmentByChild[i] << " child " << i << "(" << grades[assignmentByChild[i]][i] << ")" << endl;
					}
				}
			}
		}
	}

	// Check for stability by family
	for(int i=0; i<nbFamilies; i++){
		if(assignmentByFamily[i] >= 0){
			for(int j=0;j<families[i].nbPref;j++){
				bool cont = true;
				for(int k=0;k<families[i].preferences[j].size();k++){
					if(families[i].preferences[j][k] == assignmentByFamily[i]) cont = false;
				}
				if(!cont) break;
				for(int k=0;k<families[i].preferences[j].size();k++){
					int idxChi = families[i].preferences[j][k];
					if(grades[assignmentByChild[idxChi]][idxChi] < grades[i][assignmentByFamily[i]]){
						mistake = true;
						cout << "Stability error: blocking pair child " << idxChi << " family " << assignmentByChild[idxChi] << " has a worse grade ( " 
							<< grades[assignmentByChild[idxChi]][idxChi] << ") than family " << i << " child " << assignmentByFamily[i] << "(" << grades[i][assignmentByChild[i]] << ")" << endl;
					}
				}
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
