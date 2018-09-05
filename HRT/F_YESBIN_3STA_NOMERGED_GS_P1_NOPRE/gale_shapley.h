#ifndef GALE_SHAPLEY_H
#define GALE_SHAPLEY_H

#include <algorithm>
#include <random>
#include <vector>
#include "Allocation.h"

/**
 * Runs the Gale-Shapley algorithm to find a stable (but possible not optimal)
 * matching. Ties are broken randomly.
 *
 * @param allo An allocation object representing the problem instance.
 * @param an RNG to use.
 * @return A vector of integers, with one vector for each family. The value at
 * each point in this vector is either -1 (family is not matched) or the index
 * of the child which is matched with the given family.
 */
std::vector<std::vector<int>> run_gale_shapley(Allocation& allo, std::default_random_engine& rng);

/**
 * Call the Gale-Shapley algorithm a number of times, returning the best result
 * obtained.
 *
 * @param allo An allocation object representing the problem instance.
 * @param galeShapleyRuns The number of times the G-S algorithm should be run
 * @return The best allocation, as a vector of integers.  The value at each
 * point in this vector is either -1 (family is not matched) or the index of
 * the child which is matched with the given family.
 */
std::vector<std::vector<int>> gale_shapley(Allocation& allo, int galeShapleyRuns);

#endif /* GALE_SHAPLEY_H */
