#include "Allocation.h" 
#include "AgentIterator.h"
#include "Graph.h"

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
			c.mustBeAllocated = false;
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
			f.mustBeAllocated = false;
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
		children[i].print(false);
	}
	for(int i=0; i<nbFamilies; i++){
		families[i].print(true);
	}
}

int Allocation::reductionExact(bool children_side, bool supp) {
	int nbTotRem = 0;
	int number_here = nbChildren;
	std::vector<Child> * thesep;
	std::vector<Child> * otherp;
	std::list<int> *theseMustBeAllocatedp;
	std::list<int> *otherMustBeAllocatedp;
	if (children_side) {
		// Processing the lists of the children, removing families
		thesep = &children;
		otherp = &families;
		theseMustBeAllocatedp = &childrenMustBeAllocated;
		otherMustBeAllocatedp = &familiesMustBeAllocated;
		number_here = nbChildren;
	} else {
		thesep = &families;
		otherp = &children;
		theseMustBeAllocatedp = &familiesMustBeAllocated;
		otherMustBeAllocatedp = &childrenMustBeAllocated;
		number_here = nbFamilies;
	}
	std::vector<Child> & these = (*thesep);
	std::vector<Child> & other = (*otherp);
	std::list<int> & theseMustBeAllocated = (*theseMustBeAllocatedp);
	std::list<int> & otherMustBeAllocated = (*otherMustBeAllocatedp);
	for (int i = 0; i < number_here; i++) {
		// A graph is "named" with two integers. The first is a 0 for a candidate
		// (aka these) or a 1 for a position (aka other)
		Graph g;
		int n_1 = 0;
		for(int rank = 0; rank < these[i].nbPref; rank++) {
			// No point in checking the last rank if we already know this agent must
			// be allocated, or if we don't care
			if ((rank == these[i].nbPref - 1) && (these[i].mustBeAllocated || !supp)) {
				continue;
			}
			for(size_t ind = 0; ind < these[i].preferences[rank].size(); ind++) {
				int position = these[i].preferences[rank][ind];
				g.addVertex(1, position);
				int idxRank = these[i].ranks[rank][ind];
				for(int l = 0; l <= idxRank; l++) {
					for(size_t k = 0; k < other[position].preferences[l].size(); k++) {
						int other_cand = other[position].preferences[l][k];
						if (other_cand == i) { // Don't add the current candidate to the graph
							continue;
						}
						if (! g.containsVertex(0, other_cand)) {
							g.addVertex(0, other_cand);
							n_1 += 1;
						}
						g.addEdge(position, other_cand);
					}
				}
			}
			// If n_1 is sufficiently small, then the largest matching must also be
			// small, as the matching can use each vertex from n_1 at most once, so
			// we don't even need to try to find a bigger matching.
			bool matching_cant_exist = (2*n_1 + 1 <= g.size());
			if (! matching_cant_exist) {
				for(size_t ind = 0; ind < these[i].preferences[rank].size(); ind++) {
					int position = these[i].preferences[rank][ind];
					g.augment(position);
				}
			}
			// Add P' in this
			// Yes, I'm abusing while statements, so I can break out easier.
			while (rank == 0) {
				// Don't add P' if we don't need to.
				if (matching_cant_exist || (g.size() - g.matchingSize() >= n_1 + 1)) {
					break;
				}
				// First add all positions that must be filled.
				for(int position: otherMustBeAllocated) {
					// If position is acceptable to i, then skip it.
					bool isAcceptable = false;
					for(int otherRank = 0; (!isAcceptable) && otherRank < other[position].nbPref; otherRank++) {
						for(size_t ind = 0; ind < other[position].preferences[otherRank].size(); ++ind) {
							if (other[position].preferences[otherRank][ind] == i) {
								isAcceptable = true;
								break;
							}
						}
					}
					if (isAcceptable) {
						continue;
					}
					g.addVertex(1, position);
					for(int otherRank = 0; otherRank < other[position].nbPref; otherRank++) {
						for(size_t ind = 0; ind < other[position].preferences[otherRank].size(); ++ind) {
							int candidate = other[position].preferences[otherRank][ind];
							if (!g.containsVertex(0, candidate)) {
								g.addVertex(0, candidate);
								n_1 += 1;
							}
							g.addEdge(position, candidate);
						}
					}
					g.augment(position);
				}
				// I'm using a while loop as an if statement, so I need to break out.
				break;
			}
			if (matching_cant_exist || (g.size() - g.matchingSize() >= n_1 + 1)) {
				// preprocess on rank!
				// Firstly, they must be allocated, so mark as such (if we're in that
				// mode)
				if (supp && !these[i].mustBeAllocated) {
					theseMustBeAllocated.push_back(i);
					these[i].mustBeAllocated = true;
				}
#ifdef DEBUG
				if ((!children_side) and (i == 6)) {
					std::cout << "g.size() = " << g.size() << ", g.matchingSize() = " << g.matchingSize();
					std::cout << ", n_1 = " << n_1 << std::endl;
					g.printGraph();
					g.printMatching();
				}
				if (children_side) {
					std::cout << "child ";
				} else {
					std::cout << "family ";
				}
				std::cout << "worst rank of " << i << " is " << rank << " ";
				int remHere = 0;
#endif /* DEBUG */
				for (int k = rank + 1; k < these[i].nbPref; k++) {
					nbTotRem += these[i].preferences[k].size();
#ifdef DEBUG
					remHere += these[i].preferences[k].size();
#endif /* DEBUG */
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
				these[i].nbPref = rank + 1;
				these[i].preferences.resize(these[i].nbPref);
				these[i].ranks.resize(these[i].nbPref);
				these[i].positions.resize(these[i].nbPref);
#ifdef DEBUG
				std::cout << "removed " << remHere << std::endl;
#endif /* DEBUG */
				break;
			}
		}
	}
	if (nbTotRem > 0) {
		polish();
	}
	return nbTotRem;
}

int Allocation::reductionMine(bool children_side, int mode) {
	int nbTotRem = 0;
	int number_here;
	std::vector<Child> * thesep;
	std::vector<Child> * otherp;
	if (children_side) {
		thesep = &children;
		otherp = &families;
		number_here = nbChildren;
	} else {
		thesep = &families;
		otherp = &children;
		number_here = nbFamilies;
	}
	std::vector<Child> & these = (*thesep);
	std::vector<Child> & other = (*otherp);

	for (int i = 0; i < number_here; i++) {
		set<int> candidates;
		set<int> positions;
		int worst_rank = 0;
		unsigned int count = 0;
		AgentIterator iter(these[i], candidates, positions, these, other, mode);
		for(std::pair<int, int> p: iter) {
			int j = p.first;
			int k = p.second;
			if (j > worst_rank) {
				worst_rank = j;
			}
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
#ifdef DEBUG
				if (children_side) {
					std::cout << "child ";
				} else {
					std::cout << "family ";
				}
				std::cout << "worst rank of " << i << " is " << worst_rank << std::endl;
#endif /* DEBUG */
				for (int k = worst_rank + 1; k < these[i].nbPref; k++) {
					nbTotRem += these[i].preferences[k].size();
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
				these[i].nbPref = worst_rank + 1;
				these[i].preferences.resize(these[i].nbPref);
				these[i].ranks.resize(these[i].nbPref);
				these[i].positions.resize(these[i].nbPref);
				break;
			}
		}
	}
	if (nbTotRem > 0) {
		polish();
	}
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
	if (mode == 7) {
		do {
			num = reductionMine(false, 1);
			num += reductionMine(true, 1);
			cout << "Iteration " << i << " in heuristic removed " << num << std::endl;
			i++;
			total_reduced += num;
		} while (num != 0);
		do {
			num = reductionExact(false);
			num += reductionExact(true);
			cout << "Iteration " << i << " in exact mode removed " << num << std::endl;
			i++;
			total_reduced += num;
		} while (num != 0);
	} else {
		do{
			if (mode == 6) {
				// True means preprocess childrens' lists, removing families.
				// False means preprocess families' lists, removing children.
				num = reductionExact(false);
				num += reductionExact(true);
			} else if (mode == 8) {
				num = reductionExact(true);
				num += reductionExact(false);
			} else if (mode == 9) {
				num = reductionExact(false, true);
				num += reductionExact(true, true);
			} else if (mode == 10) {
				num = reductionExact(true, true);
				num += reductionExact(false, true);
			} else {
				num = reductionMine(false, mode);
				num += reductionMine(true, mode);
			}
			cout << "Iteration " << i << " in mode " << mode << " removed " << num << std::endl;
			i++;
			total_reduced += num;
		}while(num != 0);
	}
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
	file << name << "\t" << infos.opt << "\t" << infos.timeCPU << "\t" << infos.timeCPUPP << "\t" << total_reduced << "\t"<< infos.LB << "\t" << infos.UB << "\t" << infos.contUB << "\t" << infos.nbVar << "\t" << infos.nbCons << "\t" << infos.nbNZ 
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
