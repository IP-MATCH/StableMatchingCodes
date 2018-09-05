/* Runs the Gale-Shapley algorithm. */

#include <algorithm>
#include <cstdlib>

#include "gale_shapley.h"
#include "time.h"

std::vector<int> gale_shapley(Allocation& allo, int num_runs, bool score) {
	std::vector<int> best_result;
	float best_value = 0;
	auto rng = std::default_random_engine {};
	do {
		std::vector<int> result = run_gale_shapley(allo, rng);
		float value = 0;
		if (score) {
			for(unsigned int i = 0; i < result.size(); ++i) {
				if (result[i] != -1) {
					value += allo.grades[i][result[i]];
				}
			}
		} else {
			for_each(result.begin(), result.end(), [&value](int i) {if (i != -1) value+=1;});
		}
		if (value > best_value) {
			best_result = result;
			best_value = value;
		}
		num_runs--;
	} while (num_runs > 0);
	std::cout << "Best Gale-Shapley has a value of " << best_value << std::endl;
	return best_result;
}

int getRank(std::vector<int> &family_prefs, int other_child) {
	for(unsigned int i = 0; i < family_prefs.size(); ++i) {
		if (family_prefs[i] == other_child) {
			return i;
		}
	}
	return -1;
}

std::vector<int> run_gale_shapley(Allocation& allo, std::default_random_engine& rng) {
	double initTimeModelWC = getWallTime();
	double initTimeModelCPU = getCPUTime();
	std::vector<std::vector<int> > family_prefs;
	std::vector<std::vector<int> > child_prefs;
	for(int c = 0; c < allo.nbChildren; ++c) {
		Child child = allo.children[c];
		std::vector<int> prefs;
		// We're breaking ties by just reading in each rank from left to right.
		for(unsigned int r = 0; r < child.preferences.size(); ++r) {
			std::vector<int> this_rank(child.preferences[r]);
			std::shuffle(this_rank.begin(), this_rank.end(), rng);
			for(unsigned int i = 0; i < this_rank.size(); ++i) {
				prefs.push_back(this_rank[i]);
			}
		}
		child_prefs.push_back(prefs);
	}
	for(int f = 0; f < allo.nbFamilies; ++f) {
		Family family = allo.families[f];
		std::vector<int> prefs;
		// We're breaking ties by just reading in each rank from left to right.
		for(unsigned int r = 0; r < family.preferences.size(); ++r) {
			std::vector<int> this_rank(family.preferences[r]);
			std::shuffle(std::begin(this_rank), std::end(this_rank), rng);
			for(unsigned int i = 0; i < this_rank.size(); ++i) {
				prefs.push_back(this_rank[i]);
			}
		}
		family_prefs.push_back(prefs);
	}
	std::vector<bool> child_free;
	child_free.resize(allo.nbChildren, true);
	std::vector<unsigned int> next_proposal;
	next_proposal.resize(allo.nbChildren, 0);
	std::vector<int> family_matched_to;
	family_matched_to.resize(allo.nbFamilies, -1);
	std::vector<bool> family_free;
	family_free.resize(allo.nbFamilies, true);
	// Assume that we have less children than families, otherwise children_free
	// will never reach 0.
	int children_free = allo.nbChildren;
	int matches = 0;
	while (children_free > 0) {
		//std::cout << "New round, " << children_free << " not yet matched." << std::endl;
		int next_child = 0;
		while (child_free[next_child] == false) {
			next_child += 1;
		}
		if (next_proposal[next_child] == child_prefs[next_child].size()) {
			// This child has no match possible??
			children_free--;
			child_free[next_child] = false;
			continue;
		}
		int next_family = child_prefs[next_child][next_proposal[next_child]];
		//std::cout << "child " << next_child << " (at proposal " << next_proposal[next_child] << ") proposing to " << next_family << std::endl;
		// This child is about to propose to a family, so next time round it must
		// progress further.
		next_proposal[next_child]++;
		// If not matched, then match
		if (family_free[next_family] == true) {
			matches++;
			family_free[next_family] = false;
			child_free[next_child] = false;
			children_free--;
			family_matched_to[next_family] = next_child;
		} else {
			int other_child = family_matched_to[next_family];
			int currentRank = getRank(family_prefs[next_family], other_child);
			int newRank = getRank(family_prefs[next_family], next_child);
			// if f prefers c to current
			if (newRank < currentRank) {
				family_matched_to[next_family] = next_child;
				child_free[next_child] = false;
				child_free[other_child] = true;
			}
		}
	}
	std::cout << "Gale-Shapley completed with " << matches << " matches in " << (getWallTime() - initTimeModelWC) << "s (wall) " << (getCPUTime() - initTimeModelCPU) << "s (CPU)." << std::endl;
	return family_matched_to;
}
