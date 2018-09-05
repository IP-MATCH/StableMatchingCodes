/* Runs the Gale-Shapley algorithm. */

#include <algorithm>
#include <cstdlib>

#include "gale_shapley.h"
#include "time.h"


// #define DEBUG

#ifdef DEBUG
#define DLOG(x) std::cout << x
#else
#define DLOG(x) do {} while (0)
#endif


std::vector<std::vector<int>> gale_shapley(Allocation& allo, int num_runs) {
	std::vector<std::vector<int>> best_result;
	float best_value = 0;
	auto rng = std::default_random_engine {};
	do {
		std::vector<std::vector<int>> result = run_gale_shapley(allo, rng);
		float value = 0;
		for_each(result.begin(), result.end(), [&value](std::vector<int> i) {value+=i.size();});
		if (value > best_value) {
			best_result = result;
			best_value = value;
		}
		num_runs--;
	} while (num_runs > 0);
	std::cout << "Best Gale-Shapley has a value of " << best_value << std::endl;
	return best_result;
}

unsigned int rankOfDoctor(std::vector<int> &prefs, int doctor) {
	// Find the rank of doctor by going through each rank in the hospitals
	// preference list.
	unsigned int rank;
	for(rank = 0; rank < prefs.size(); ++rank) {
		if (prefs[rank] == doctor+1) {
			break;
		}
	}
	return rank;
}

// family -> hospital
// children -> doctor
std::vector<std::vector<int>> run_gale_shapley(Allocation& allo, std::default_random_engine& rng) {
	double initTimeModelWC = getWallTime();
	double initTimeModelCPU = getCPUTime();
	std::vector<std::vector<int> > hospital_prefs;
	std::vector<std::vector<int> > doctor_prefs;
	for(auto doctor: allo.doctors) {
		std::vector<int> prefs;
		for(unsigned int r = 0; r < doctor.preferences.size(); ++r) {
			std::vector<int> this_rank(doctor.preferences);
			for(unsigned int i = 0; i < this_rank.size(); ++i) {
				prefs.push_back(this_rank[i]-1); // doctors have IDs starting at 1, but are indexed from 0
			}
		}
		doctor_prefs.push_back(prefs);
	}
	for(auto hospital: allo.hospitals) {
		std::vector<int> prefs;
		// We're breaking ties by just reading in each rank from left to right.
		for(unsigned int r = 0; r < hospital.preferences.size(); ++r) {
			std::vector<int> this_rank(hospital.preferences[r]);
			std::shuffle(std::begin(this_rank), std::end(this_rank), rng);
			for(unsigned int i = 0; i < this_rank.size(); ++i) {
				prefs.push_back(this_rank[i]);
			}
		}
		hospital_prefs.push_back(prefs);
	}
	std::vector<bool> doctor_free;
	doctor_free.resize(allo.nbDoctors, true);
	std::vector<unsigned int> next_proposal;
	next_proposal.resize(allo.nbDoctors, 0);
	std::vector<std::vector<int>> hospital_matched_to;
	hospital_matched_to.resize(allo.nbHospitals);
	std::vector<int> hospital_free;
	hospital_free.resize(allo.nbHospitals);
	int hospital_spaces = 0;
	for(int h = 0; h < allo.nbHospitals; ++h) {
		hospital_free[h] = allo.hospitals[h].cap;
		hospital_spaces += allo.hospitals[h].cap;
	}
	int agents_free = allo.nbDoctors;
	int matches = 0;
	while (agents_free > 0) {
		int next_doctor = 0;
		while (doctor_free[next_doctor] == false) {
			next_doctor += 1;
		}
		if (next_proposal[next_doctor] == doctor_prefs[next_doctor].size()) {
			// This doctor has no match possible??
			agents_free--;
			doctor_free[next_doctor] = false;
			continue;
		}
		int next_hospital = doctor_prefs[next_doctor][next_proposal[next_doctor]];
		DLOG("d " << next_doctor << " (at proposal " << next_proposal[next_doctor] << ") proposing to " << next_hospital << std::endl);
		next_proposal[next_doctor]++;
		// If not matched, then match
		if (hospital_free[next_hospital] > 0) {
			matches++;
			hospital_free[next_hospital]--;
			DLOG("Matched due to empty spot (" << hospital_free[next_hospital] << " of " << allo.hospitals[next_hospital].cap << " free)" << std::endl);
			doctor_free[next_doctor] = false;
			agents_free--;
			unsigned int newRank = rankOfDoctor(hospital_prefs[next_hospital], next_doctor);
			bool added = false;
			for(unsigned int i = 0; i < hospital_matched_to[next_hospital].size(); ++i) {
				unsigned int rankHere = rankOfDoctor(hospital_prefs[next_hospital], hospital_matched_to[next_hospital][i]);
				if (rankHere > newRank) {
					auto location = hospital_matched_to[next_hospital].begin() + i;
					hospital_matched_to[next_hospital].insert(location, next_doctor);
					added = true;
					break;
				}
			}
			if (! added) {
				hospital_matched_to[next_hospital].push_back(next_doctor);
			}
#ifdef DEBUG
				std::cout << "New list:";
				for(auto d: hospital_matched_to[next_hospital]) {
					std::cout << " " << d << "[r" << rankOfDoctor(hospital_prefs[next_hospital], d) << "]";
				}
				std::cout << std::endl;
#endif
		} else {
			// Find worst doctor assigned to next_hospital.
			int other_doctor = hospital_matched_to[next_hospital].back();
			// Find the rank of the current worst doctor (other_doctor) by going
			// through each rank in the new hospitals preference list.
			unsigned int currentRank = rankOfDoctor(hospital_prefs[next_hospital], other_doctor);

			// Find the rank of the new doctor (next_doctor) by going through
			// each rank in the new hospitals preference list.
			unsigned int newRank = rankOfDoctor(hospital_prefs[next_hospital], next_doctor);
			// if h prefers new_doctor to current_doctor
			if (newRank < currentRank) {
				DLOG("Matched due to kicking out " << hospital_matched_to[next_hospital].back() << std::endl);
#ifdef DEBUG
				std::cout << "Old list:";
				for(auto d: hospital_matched_to[next_hospital]) {
					std::cout << " " << d << "[r" << rankOfDoctor(hospital_prefs[next_hospital], d) << "]";
				}
				std::cout << std::endl;
#endif
				// Make the "last doctor" free
				doctor_free[hospital_matched_to[next_hospital].back()] = true;
				// Remove the last doctor
				hospital_matched_to[next_hospital].erase(hospital_matched_to[next_hospital].end()-1);
				// Insert next_doctor in right spot
				bool added = false;
				for(unsigned int i = 0; i < hospital_matched_to[next_hospital].size(); ++i) {
					unsigned int rankHere = rankOfDoctor(hospital_prefs[next_hospital], hospital_matched_to[next_hospital][i]);
					if (rankHere > newRank) {
						auto location = hospital_matched_to[next_hospital].begin() + i;
						hospital_matched_to[next_hospital].insert(location, next_doctor);
						added = true;
						break;
					}
				}
				if (! added) {
					hospital_matched_to[next_hospital].push_back(next_doctor);
				}
#ifdef DEBUG
				std::cout << "New list:";
				for(auto d: hospital_matched_to[next_hospital]) {
					std::cout << " " << d << "[r" << rankOfDoctor(hospital_prefs[next_hospital], d) << "]";
				}
				std::cout << std::endl;
#endif
				doctor_free[next_doctor] = false;
			}
		}
	}
	std::cout << "Gale-Shapley completed with " << matches << " matches in " << (getWallTime() - initTimeModelWC) << "s (wall) " << (getCPUTime() - initTimeModelCPU) << "s (CPU)." << std::endl;
	return hospital_matched_to;
}
