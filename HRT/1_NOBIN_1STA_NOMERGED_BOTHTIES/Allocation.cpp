#include <algorithm>

#include "Allocation.h" 
#include "AgentIterator.h"
#include "Graph.h"
#include "time.h"

/*	*************************************************************************************
	***********************************  DOCTOR *****************************************
	************************************************************************************* */

void Doctor::print(){
	cout << "Doctor " << id << "\t Preferences (" << nbPref << " groups, " << nbTotPref << " in total)\t";
	for(int i=0; i<nbPref; i++){
		cout << "(";
			for(size_t j=0; j<preferences[i].size(); j++){
				if(j > 0) cout << " ";
				cout << preferences[i][j];
			}
			cout << ") "; 
	}
	cout << endl;
}

/*	*************************************************************************************
	*********************************** HOSPITAL ****************************************
	************************************************************************************* */

void Hospital::print(){
	cout << "Hospital " << id << "\t Capacity " << cap << "\t Preferences (" << nbPref << " groups, " << nbTotPref << " in total)\t";
	for(int i=0; i<nbPref; i++){
		cout << "(";
			for(size_t j=0; j<preferences[i].size(); j++){
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
			istringstream tempIss;
			string tempString;

			d.nbPref = 0;
			
			getline(file, parser);
			iss.str(parser);
			iss >> d.id;
			int rank = 1;
			for(;;){
				char tempChar = iss.get();
				if(iss.eof()) 
					break;
				else{
					if(tempChar == '('){
						getline(iss, tempString, ')');
						tempIss.str(tempString);
						vector<int> tempPref;
						while(tempIss >> temp){
							tempPref.push_back(temp);
						}
						d.ranks.emplace_back(tempPref.size());
						d.preferences.push_back(tempPref);
						d.nbTotPref+= tempPref.size();
						tempIss.clear();
					}
					else{
						if((tempChar >= '0') && (tempChar <= '9')){
							iss.putback(tempChar);
							vector<int> tempPref;
							iss >> temp;
							tempPref.push_back(temp);
							d.ranks.emplace_back(tempPref.size());
							d.preferences.push_back(tempPref);
							d.nbTotPref+= tempPref.size();
							tempIss.clear();
						}
					}
					rank += 1;
				}
			}
			d.nbPref = d.preferences.size();
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
								for(size_t id = 0; id < doctors[temp-1].preferences[j].size(); ++id) {
									if(doctors[temp-1].preferences[j][id] == i+1)
										doctors[temp-1].ranks[j][id] = currRank;
								}
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
								for(size_t id = 0; id < doctors[temp-1].preferences[j].size(); ++id) {
									if(doctors[temp-1].preferences[j][id] == i+1)
										doctors[temp-1].ranks[j][id] = currRank;
								}
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


int Allocation::reductionMineDoctors(int mode) {
	int nbTotRem = 0;

	for (int i = 0; i < nbDoctors; i++) {
		set<int> candidates;
		set<int> positions;
		int worst_rank = 0;
		unsigned int count = 0;
		AgentIterator<Doctor,Hospital> iter(doctors[i], candidates, positions, doctors, hospitals, mode);
		for(std::pair<int, int> p: iter) {
			int j = p.first;
			if (j > worst_rank) {
				worst_rank = j;
			}
			int idxHos = doctors[i].preferences[j][p.second] - 1;
			positions.insert(idxHos+1);
			count += hospitals[idxHos].cap;
			for (auto & group: hospitals[idxHos].preferences) {
				bool break_yet = false;
				for(int pref: group) {
					candidates.insert(pref);
					if (pref == doctors[i].id) {
						break_yet = true;
					}
				}
				if (break_yet) {
					break;
				}
			}
			if (count >= candidates.size()) {
#ifdef DEBUG
				std::cout << "doctor worst rank of " << doctors[i].id << " is " << worst_rank << std::endl;
				int remHere = 0;
#endif /* DEBUG */
				for (int k = worst_rank + 1; k < doctors[i].nbPref; k++) {
					for (size_t id = 0; id < doctors[i].preferences[k].size(); ++id) {
						nbTotRem += 1;
#ifdef DEBUG
						remHere += 1;
#endif /* DEBUG */
						int idxHos = doctors[i].preferences[k][id] - 1;
						for(size_t rank = 0; rank < hospitals[idxHos].preferences.size(); ++rank) {
							auto posDoc = std::find(hospitals[idxHos].preferences[rank].begin(), hospitals[idxHos].preferences[rank].end(), i+1);
							if (posDoc != hospitals[idxHos].preferences[rank].end()) {
								hospitals[idxHos].preferences[rank].erase(posDoc);
								hospitals[idxHos].nbTotPref--;
							}
						}
					}
				}
#ifdef DEBUG
				std::cout << "removed " << remHere << std::endl;
#endif /* DEBUG */
				doctors[i].nbPref = worst_rank + 1;
				doctors[i].preferences.resize(doctors[i].nbPref);
				break;
			}
		}
	}
	if (nbTotRem > 0) {
		polish();
	}
	return nbTotRem;
}

int Allocation::reductionMineHospitals(int mode) {
	int nbTotRem = 0;

	for (int i = 0; i < nbHospitals; i++) {
		set<int> candidates;
		set<int> positions;
		unsigned int candidate_cap = 0;
		int worst_rank = 0;
		unsigned int count = 0;
#ifdef DEBUG
		std::cout << "Preprocessing hospital " << i << std::endl;
#endif /* DEBUG */
		AgentIterator<Hospital,Doctor> iter(hospitals[i], candidates, positions, hospitals, doctors, mode);
		for(std::pair<int, int> p: iter) {
			int j = p.first;
			int k = p.second;
			if (j > worst_rank) {
				worst_rank = j;
			}
			int idxDoc = hospitals[i].preferences[j][k] - 1;
			positions.insert(idxDoc+1);
			count += 1;
			for(const auto & group: doctors[idxDoc].preferences) {
				for (int pref: group) {
					if (candidates.count(pref) == 0) {
						candidates.insert(pref);
						candidate_cap += hospitals[pref-1].cap;
					}
					if (pref == hospitals[i].id) {
						break;
					}
				}
			}
			if (count >= candidate_cap) {
#ifdef DEBUG
				std::cout << "hospital worst rank of " << i << " is " << worst_rank << std::endl;
				int remHere = 0;
#endif /* DEBUG */
				for (int k = worst_rank + 1; k < hospitals[i].nbPref; k++) {
					nbTotRem += hospitals[i].preferences[k].size();
#ifdef DEBUG
					remHere += hospitals[i].preferences[k].size();
#endif /* DEBUG */
					hospitals[i].nbTotPref -= hospitals[i].preferences[k].size();
					for (unsigned int l = 0; l < hospitals[i].preferences[k].size(); l++) {
						int idxDoc = hospitals[i].preferences[k][l] - 1;
						bool break_now = false;
						for (size_t m = 0 ; (!break_now) && m < doctors[idxDoc].preferences.size(); ++m) {
							for (size_t id = 0; id < doctors[idxDoc].preferences[m].size(); ++id) {
								if (doctors[idxDoc].preferences[m][id] == i + 1) {
									doctors[idxDoc].preferences[m].erase(doctors[idxDoc].preferences[m].begin() + id);
									doctors[idxDoc].ranks[m].erase(doctors[idxDoc].ranks[m].begin() + id);
									doctors[idxDoc].nbTotPref--;
									break_now = true;
									break;
								}
							}
						}
					}
				}
				hospitals[i].nbPref = worst_rank + 1;
				hospitals[i].preferences.resize(hospitals[i].nbPref);
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



/**
 * Reduce on the doctor's preference lists, removing hospitals.
 */
int Allocation::reductionExactDoctor(bool supp) {
	bool newMustBeAllocated = false;
	int nbTotRem = 0;
	for (int i = 0; i < nbDoctors; i++) {
		Graph g(1, nbHospitals + nbDoctors);
		// First add all positions that must be filled.
		for(int position: hospitalsMustBeAllocated) {
			// If position is acceptable to i, then skip it.
			bool isAcceptable = false;
			for(size_t rank = 0; (!isAcceptable) && rank < hospitals[position].preferences.size(); ++rank) {
				for(size_t ind = 0; ind < hospitals[position].preferences[rank].size(); ++ind) {
					if (hospitals[position].preferences[rank][ind] == doctors[i].id) {
						isAcceptable = true;
						break;
					}
				}
			}
			if (isAcceptable) {
				continue;
			}
			auto pos_vert = std::make_pair(1, position);
			g.addVertex(pos_vert, hospitals[position].cap);
			for(size_t rank = 0; rank < hospitals[position].preferences.size(); ++rank) {
				for(size_t ind = 0; ind < hospitals[position].preferences[rank].size(); ++ind) {
					int candidate = hospitals[position].preferences[rank][ind] - 1;
					auto cand_vert = std::make_pair(0, candidate);
					if (!g.containsVertex(cand_vert)) {
						g.addVertex(cand_vert, 1);
					}
					g.addEdge(pos_vert, cand_vert);
				}
			}
			int times_to_augment = hospitals[position].cap;
			while (times_to_augment > 0) {
				if (!g.augment(pos_vert)) {
					break;
				}
				times_to_augment--;
				}
		}
		for(size_t rank = 0; rank < doctors[i].preferences.size(); rank++) {
			// No point in checking the last rank if we already know this agent must
			// be allocated, or if we don't care
			if ((rank == doctors[i].preferences.size() - 1) && (doctors[i].mustBeAllocated || !supp)) {
				continue;
			}
			for(size_t id = 0; id < doctors[i].preferences[rank].size(); ++id) {
				if ((getCPUTime() - initTimePP) > MAXTIME) {
					cout << "Time elapsed, breaking";
					return 0;
				}
				int position = doctors[i].preferences[rank][id] - 1;
				auto pos_vert = std::make_pair(1, position);
				g.addVertex(pos_vert, hospitals[position].cap);
				bool break_yet = false;
				for(size_t k = 0; k < hospitals[position].preferences.size(); ++k) {
					for(size_t l = 0; l < hospitals[position].preferences[k].size(); ++l) {
						int doctor_cand = hospitals[position].preferences[k][l] - 1;
						if (doctor_cand == i) { // Don't add the current candidate to the graph
							// Don't add doctors after this rank.
							break_yet = true;
							continue;
						}
						std::pair<int,int> doctor_cand_vert = std::make_pair(0, doctor_cand);
						if (! g.containsVertex(doctor_cand_vert)) {
							g.addVertex(doctor_cand_vert, 1);
						}
						g.addEdge(pos_vert, doctor_cand_vert);
					}
					if (break_yet) {
						break;
					}
				}
				// max_flow is <= min(g.cap_left(), g.cap_right()), so we can do simple
				// checks which might mean we can not bother trying to augment
				bool can_def_preprocess = 1 + 2*g.cap_left() <= g.cap_total();
				if (1 + g.cap_left() + g.cap_right() <= g.cap_total()) {
					can_def_preprocess = true;
				}
				if (!can_def_preprocess) {
					int times_to_augment = hospitals[position].cap;
					while (times_to_augment > 0) {
						if (!g.augment(pos_vert)) {
							break;
						}
						times_to_augment--;
					}
				}
				if (can_def_preprocess || g.can_preprocess()) {
					// preprocess on rank!
					// Firstly, they must be allocated, so mark as such (if we're in that
					// mode)
					if (supp && !doctors[i].mustBeAllocated) {
						doctorsMustBeAllocated.push_back(i);
						doctors[i].mustBeAllocated = true;
						newMustBeAllocated = true;
					}
#ifdef DEBUG
					if (i == 6) {
						//std::cout << "g.size() = " << g.size() << ", g.matchingSize() = " << g.matchingSize();
						//std::cout << ", n_1 = " << n_1 << std::endl;
						//g.printGraph();
						//g.printMatching();
					}
					std::cout << "doctor worst rank of " << i << " is " << rank << " ";
					int remHere = 0;
#endif /* DEBUG */
					for (int k = rank + 1; k < doctors[i].nbPref; k++) {
						for (size_t id = 0; id < doctors[i].preferences[k].size(); ++id) {
							nbTotRem += 1;
#ifdef DEBUG
							remHere += 1;
#endif /* DEBUG */
							int idxHos = doctors[i].preferences[k][id] - 1;
							// remove from idxHos any reference to i.
							for(size_t rank = 0 ; rank < hospitals[idxHos].preferences.size(); ++rank) {
								auto posDoc = std::find(hospitals[idxHos].preferences[rank].begin(), hospitals[idxHos].preferences[rank].end(), i + 1);
								if (posDoc != hospitals[idxHos].preferences[rank].end()) {
									hospitals[idxHos].preferences[rank].erase(posDoc);
								}
							}
						}
					}
					doctors[i].nbPref = rank + 1;
					doctors[i].preferences.resize(doctors[i].nbPref);
#ifdef DEBUG
					std::cout << "removed " << remHere << std::endl;
#endif /* DEBUG */
					break;
				}
			}
		}
	}
	if (nbTotRem > 0) {
		polish();
	}
	if ((nbTotRem == 0) && (newMustBeAllocated == true)) {
		return -1;
	}
	return nbTotRem;
}

/**
 * Reduce on the hospital's preference lists, removing doctors.
 */
int Allocation::reductionExactHospital(bool supp) {
	bool newMustBeAllocated = false;
	int nbTotRem = 0;
	for (size_t i = 0; i < (size_t)nbHospitals; i++) {
		Graph g(hospitals[i].cap, nbHospitals + nbDoctors);
		// First add all positions that must be filled.
		for(int position: doctorsMustBeAllocated) {
			// If position is acceptable to i, then skip it.
			bool isAcceptable = false;
			for(size_t ind = 0; (!isAcceptable) && ind < doctors[position].preferences.size(); ++ind) {
				for(size_t id = 0; id < doctors[position].preferences[ind].size(); ++id) {
					if ((doctors[position].preferences[ind][id] - 1) == (int)i) {
						isAcceptable = true;
						break;
					}
				}
			}
			if (isAcceptable) {
				continue;
			}
			auto pos_vert = std::make_pair(1, position);
			g.addVertex(pos_vert, 1);
			for(size_t ind = 0; ind < doctors[position].preferences.size(); ++ind) {
				for(size_t id = 0; id < doctors[position].preferences[ind].size(); ++id) {
					int candidate = doctors[position].preferences[ind][id] - 1;
					auto cand_vert = std::make_pair(0, candidate);
					if (!g.containsVertex(cand_vert)) {
						g.addVertex(cand_vert, hospitals[candidate].cap);
					}
					g.addEdge(pos_vert, cand_vert);
				}
			}
			int times_to_augment = 1;
			while (times_to_augment > 0) {
				if (!g.augment(pos_vert)) {
					break;
				}
				times_to_augment--;
			}
		}
		for(size_t rank = 0; rank < hospitals[i].preferences.size(); rank++) {
			// No point in checking the last rank if we already know this agent must
			// be allocated, or if we don't care
			if ((rank == hospitals[i].preferences.size()- 1) && (hospitals[i].mustBeAllocated || !supp)) {
				continue;
			}
			if ((getCPUTime() - initTimePP) > MAXTIME) {
				cout << "Time elapsed, breaking";
				return 0;
			}
			for(size_t ind = 0; ind < hospitals[i].preferences[rank].size(); ind++) {
				int position = hospitals[i].preferences[rank][ind] - 1;
				auto pos_vert = std::make_pair(1, position);
				g.addVertex(pos_vert, 1);
				bool break_yet = false;
				for(size_t k = 0; k < doctors[position].preferences.size() && (!break_yet); k++) {
					for(size_t id = 0; id < doctors[position].preferences[k].size(); ++id) {
						int hosp_cand = doctors[position].preferences[k][id] - 1;
						if (hosp_cand == (int)i) { // Don't add the current candidate to the graph
							break_yet = true;
							continue;
						}
						std::pair<int,int> hosp_cand_vert = std::make_pair(0, hosp_cand);
						if (! g.containsVertex(hosp_cand_vert)) {
							g.addVertex(hosp_cand_vert, hospitals[hosp_cand].cap);
						}
						g.addEdge(pos_vert, hosp_cand_vert);
					}
				}
				// max_flow is <= min(g.cap_left(), g.cap_right()), so we can do simple
				// checks which might mean we can not bother trying to augment
				bool can_def_preprocess = hospitals[i].cap + 2*g.cap_left() <= g.cap_total();
				if (hospitals[i].cap + g.cap_left() + g.cap_right() <= g.cap_total()) {
					can_def_preprocess = true;
				}
				if (!can_def_preprocess) {
					int times_to_augment = 1;
					while (times_to_augment > 0) {
						if (!g.augment(pos_vert)) {
							break;
						}
						times_to_augment--;
					}
				}
			}
			bool can_def_preprocess = hospitals[i].cap + 2*g.cap_left() <= g.cap_total();
			if (can_def_preprocess || g.can_preprocess()) {
				// preprocess on rank!
				// Firstly, they must be allocated, so mark as such (if we're in that
				// mode)
				if (supp && !hospitals[i].mustBeAllocated) {
					hospitalsMustBeAllocated.push_back(i);
					hospitals[i].mustBeAllocated = true;
					newMustBeAllocated = true;
				}
#ifdef DEBUG
				if (i == 0) {
					std::cout << "g.cap_original() = " << hospitals[i].cap << ", g.cap_left() = " << g.cap_left();
					std::cout << ", cap_total = " << g.cap_total() << ", g.maxFlow() = " << g.maxFlow() << std::endl;
					//g.printGraph();
					//g.printMatching();
				}
				std::cout << "hospital worst rank of " << i << " is " << rank << " ";
				int remHere = 0;
#endif /* DEBUG */
				for (int k = rank + 1; k < hospitals[i].nbPref; k++) {
					nbTotRem += hospitals[i].preferences[k].size();
#ifdef DEBUG
					remHere += hospitals[i].preferences[k].size();
#endif /* DEBUG */
					hospitals[i].nbTotPref -= hospitals[i].preferences[k].size();
					for (unsigned int l = 0; l < hospitals[i].preferences[k].size(); l++) {
						int idxDoc = hospitals[i].preferences[k][l] - 1;
						for(size_t n=0; n < doctors[idxDoc].preferences.size();n++){
							for(size_t id = 0; id < doctors[idxDoc].preferences[n].size(); ++id) {
								if(doctors[idxDoc].preferences[n][id]-1 == (int)i){
									doctors[idxDoc].preferences[n].erase(doctors[idxDoc].preferences[n].begin() + id); 
									doctors[idxDoc].ranks[n].erase(doctors[idxDoc].ranks[n].begin() + id); 
									doctors[idxDoc].nbTotPref--;
								}
							}
						}
					}
				}
				hospitals[i].nbPref = rank + 1;
				hospitals[i].preferences.resize(hospitals[i].nbPref);
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
	if ((nbTotRem == 0) && (newMustBeAllocated == true)) {
		return -1;
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
				for(size_t i=0;i<hospitals[j].preferences[k].size();i++){
					int idxDoc = hospitals[j].preferences[k][i] - 1;
					for(int group_id = 0; group_id < doctors[idxDoc].nbPref; group_id++){
						for(size_t id = 0; id < doctors[idxDoc].preferences[group_id].size(); ++id) {
							if(doctors[idxDoc].preferences[group_id][id]-1 == j){
								doctors[idxDoc].ranks[group_id][id] -= idxRem;
							}
						}
					}
				}
			}
		}
		hospitals[j].preferences = preferences;
		hospitals[j].nbPref = preferences.size();
	}
}

void Allocation::reduction(int mode){
	total_removed = 0;
	int i = 0;
	int this_time = 0;
	pp_mode = mode;
	initTimePP = getCPUTime();
	if (mode == 7) {
		do {
			this_time = reductionMineHospitals(1);
			this_time += reductionMineDoctors(1);
			cout << "Iteration " << i << " in heuristic removed " << this_time << std::endl;
			total_removed += this_time;
			i++;
		} while (this_time != 0);
		do {
			this_time = reductionExactHospital(1);
			this_time += reductionExactDoctor(1);
			cout << "Iteration " << i << " in heuristic removed " << this_time << std::endl;
			total_removed += this_time;
			i++;
		} while (this_time != 0);
	} else if (mode == 11) {
		do {
			this_time = reductionMineHospitals(1);
			this_time += reductionMineDoctors(1);
			cout << "Iteration " << i << " in heuristic removed " << this_time << std::endl;
			total_removed += this_time;
			i++;
		} while (this_time != 0);
		do {
			this_time = reductionExactDoctor(1);
			this_time += reductionExactHospital(1);
			cout << "Iteration " << i << " in heuristic removed " << this_time << std::endl;
			total_removed += this_time;
			i++;
		} while (this_time != 0);
	} else {
		bool keep_going;
		do{
			keep_going = false;
			if (mode == 6) {
				this_time = reductionExactHospital(false);
				this_time += reductionExactDoctor(false);
				total_removed += this_time;
			} else if (mode == 8) {
				this_time = reductionExactDoctor(false);
				this_time += reductionExactHospital(false);
				total_removed += this_time;
			} else if (mode == 9) {
				int num = reductionExactHospital(true);
				if (num == -1) {
					keep_going = true;
					this_time = 0;
				} else {
					this_time = num;
				}
				num = reductionExactDoctor(true);
				if (num == -1) {
					keep_going = true;
				} else {
					this_time += num;
				}
				total_removed += this_time;
			} else if (mode == 10) {
				int num = reductionExactDoctor(true);
				if (num == -1) {
					keep_going = true;
					this_time = 0;
				} else {
					this_time = num;
				}
				num = reductionExactHospital(true);
				if (num == -1) {
					keep_going = true;
				} else {
					this_time += num;
				}
				total_removed += this_time;
			} else {
				this_time = reductionMineHospitals(mode);
				this_time += reductionMineDoctors(mode);
				total_removed += this_time;
			}
			cout << "Reduction mode " << mode << " iteration " << i << " removed " << this_time << endl;
			i++;
			if ((getCPUTime() - initTimePP) > MAXTIME) {
				cout << "Time elapsed, breaking";
				break;
			}
		}while(keep_going || this_time != 0);
	}
	polish();
}

void Allocation::printSol(){
	cout << "Allocation by hospitals" << endl;
	for(int i=0; i<nbHospitals; i++){
		cout << i+1 << ":";
		for(size_t j=0; j<assignmentByHospital[i].size();j++){
			cout << "\t" << assignmentByHospital[i][j];
		}
		cout << endl;
	}
}

void Allocation::printInfo(const string& pathAndFileout){
	string nameFile = pathAndFileout;
	std::ofstream file(nameFile.c_str(), std::ios::out | std::ios::app);
	file << name << "\t" << infos.opt << "\t" << pp_mode << "\t" << infos.timeCPU << "\t" << infos.timeCPUPP << "\t" << total_removed << "\t"<< infos.LB << "\t" << infos.UB << "\t" << infos.contUB << "\t" << infos.nbVar << "\t" << infos.nbCons << "\t" << infos.nbNZ 
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
		for(int group_id=0; group_id<doctors[i].nbPref; group_id++){
			for(size_t id =0; id < doctors[i].preferences[group_id].size(); ++id) {
				if(assignmentByDoctor[i] == doctors[i].preferences[group_id][id]){
					maxRankHospital[doctors[i].preferences[group_id][id]-1] = max(maxRankHospital[doctors[i].preferences[group_id][id]-1], doctors[i].ranks[group_id][id]);
					capacityHospital[doctors[i].preferences[group_id][id]-1]++ ;
					solByDoctor++;
				}
			}
		}
	}

	// Check for stability
	for(int i=0; i<nbDoctors; i++){
		int group_id = 0;
		while(group_id < doctors[i].nbPref) {
			if (std::find(doctors[i].preferences[group_id].begin(),
						doctors[i].preferences[group_id].end(),
						assignmentByDoctor[i]) != doctors[i].preferences[group_id].end()) {
				break;
			}
			for(size_t id = 0; id < doctors[i].preferences[group_id].size(); ++id){
				if(maxRankHospital[doctors[i].preferences[group_id][id]-1] > doctors[i].ranks[group_id][id]){
					cout << "Stability error: blocking pair Hospital " << doctors[i].preferences[group_id][id] << " (rank " << maxRankHospital[doctors[i].preferences[group_id][id]-1] << ") Doctor "
						<< i+1 << "(rank" << doctors[i].ranks[group_id][id] << ")"<< endl;
					mistake = true;
				}
			}
			group_id++;
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
		for(size_t j = 0; j< assignmentByHospital[i].size(); j++){
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

