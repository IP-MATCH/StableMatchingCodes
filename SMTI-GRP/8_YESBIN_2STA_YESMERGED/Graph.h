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


template<class t_name>
class Graph {
  public:
    Graph();
    void addVertex(t_name name, bool open=false);
    bool containsVertex(t_name name) const;
    void addEdge(t_name v1, t_name v2);
    const std::list<int> adjacent(t_name vertex) const;
    int matched(t_name vertex) const;
    void augment(t_name vertex);

    int size() const;
    int matchingSize() const;

    t_name name(int vert_index);

#ifdef DEBUG
    void printGraph();
    void printMatching() const;
#endif /* DEBUG */

  private:
    std::unordered_map<t_name, int, pairhash> _names;
    std::unordered_map<int, t_name > _indices;
    std::vector<std::list<int>> _adjacents;
    std::unordered_map<int, int> _matching;

    bool internal_augment(int now, std::vector<bool> & visited, std::list<int> & path);
};

#endif /* GRAPH_H */
