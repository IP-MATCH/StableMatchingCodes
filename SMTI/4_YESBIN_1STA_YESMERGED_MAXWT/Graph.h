#ifndef GRAPH_H
#define GRAPH_H

#include <limits>
#include <list>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>


static_assert(std::numeric_limits<size_t>::max() >= ((long unsigned)1 << 63), "size_t is too small");

struct pairhash {
	public:
		std::size_t operator()(const std::pair<int,int> &x) const {
			return (long)x.first * ((long)1 << 32) + x.second;
		}
};

#ifdef DEBUG
#include <iostream>
inline std::ostream& operator<<(std::ostream& o, const std::pair<int, int> &x) {
  o << "(" << x.first << ", " << x.second << ")";
  return o;
}
#endif /* DEBUG */


class Graph {
  public:
    Graph();
    void addVertex(int side, int name);
    bool containsVertex(int side, int name) const;
    void addEdge(int v1, int v2);
    int matched(int vertex) const;
    void augment(int vertex);

    int size() const;
    int matchingSize() const;

    int name(int vert_index);

#ifdef DEBUG
    void printGraph();
    void printMatching() const;
#endif /* DEBUG */

  private:
    std::vector<std::vector<bool>> _exists;
    std::vector<std::vector<std::vector<int>>> _adjacents;
    std::vector<std::vector<signed int>> _matching;

    int _size;
    int _matching_size;

    bool internal_augment(int now, std::vector<bool> & visited, std::list<int> & path);
};

#endif /* GRAPH_H */
