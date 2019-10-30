#ifndef GRAPH_H
#define GRAPH_H

#include <limits>
#include <list>
#include <map>
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

#if defined(DEBUG_GRAPH) || defined(DEBUG)
#include <iostream>
inline std::ostream& operator<<(std::ostream& o, const std::pair<int, int> &x) {
  o << "(" << x.first << ", " << x.second << ")";
  return o;
}
#endif /* DEBUG_GRAPH */

struct Edge {
    int id;
    int cap;
    int flow;
    bool reverse;
};

typedef std::pair<int, int> Vertex;

class Graph {
  public:
    Graph(int cap_right);
    void addVertex(Vertex name, int capacity);
    bool containsVertex(Vertex name) const;
    void addEdge(Vertex v1, Vertex v2);
    const std::list<int> adjacent(Vertex vertex) const;
    int matched(Vertex vertex) const;
    bool augment();

    bool can_preprocess();

    int size() const;
    int maxFlow() const;

    int cap_total() const;
    int cap_left() const;
    int cap_right() const;

    Vertex name(int vert_index);

    constexpr static int SOURCE = 0;
    constexpr static int SINK = 1;

#ifdef DEBUG_GRAPH
    void printGraph();
#endif /* DEBUG_GRAPH */

  private:
    int _cap_original;
    int _cap_total;
    int _left_total;
    int max_flow;
    std::unordered_map<Vertex, int, pairhash> _names;
    std::unordered_map<int, Vertex> _indices;
    // Each adjacent is a triple containing (other_vertex, capacity,
    // current_flow)
    std::map<int, std::map<int, Edge>> _adjacents;

    Edge & getEdge(int a, int b) { return _adjacents[a][b]; }

    bool internal_augment(int now, std::vector<bool> & visited, std::list<int> & path);
};

#endif /* GRAPH_H */
