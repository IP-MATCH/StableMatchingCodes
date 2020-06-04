#ifndef GRAPH_H
#define GRAPH_H

#include <limits>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>


// The type of our vertex ID.
typedef unsigned int vert_id; // Identifier from instance
typedef unsigned int int_id; // Our internal identifier

static_assert(std::numeric_limits<size_t>::max() >= ((long unsigned)1 << 31), "size_t is too small");
static_assert(std::numeric_limits<vert_id>::max() >= ((long unsigned)1 << 31), "unsigned int is too small");

typedef std::pair<short , vert_id> Vertex;

struct pairhash {
	public:
		// Note that we have to allow x.first == 2 for our source/sink.
		// 1 << 28 == 2^29, so 2 * (1 << 28) = 2^31.
		std::size_t constexpr operator()(const Vertex &x) const {
			return (long)x.first * ((long)1 << 28) + x.second;
		}
};

#if defined(DEBUG_GRAPH) || defined(DEBUG)
#include <iostream>
inline std::ostream& operator<<(std::ostream& o, const Vertex &x) {
  o << "(" << x.first << ", " << x.second << ")";
  return o;
}
#endif /* DEBUG_GRAPH */

struct Edge {
    size_t id;
    int cap;
    int flow;
    bool reverse;
};


class Graph {
  public:
    Graph(int cap_right);
    void addVertex(Vertex name, int capacity);
    bool containsVertex(Vertex name) const;
    void addEdge(Vertex v1, Vertex v2);
    const std::list<int> adjacent(Vertex vertex) const;
    int matched(Vertex vertex) const;
    bool augment(Vertex source);

    bool can_preprocess();

    int size() const;
    int maxFlow() const;

    int cap_total() const;
    int cap_left() const;
    int cap_right() const;

    Vertex name(int vert_index);

    constexpr static vert_id SOURCE = 0;
    constexpr static vert_id SINK = 1;

#ifdef DEBUG_GRAPH
    void printGraph();
#endif /* DEBUG_GRAPH */

  private:
    int _cap_original;
    int _cap_total;
    int _left_total;
    int max_flow;
    std::unordered_map<int_id, Vertex> _indices;
    std::unordered_map<Vertex, int_id, pairhash> _names;
    std::vector<std::vector<char>> _exists;
    std::vector<std::vector<Edge>> _adjacents;
    std::vector<signed int> _cap_remaining;

    Edge & getEdge(int_id a, int_id b) { return _adjacents[a][b]; }

    bool internal_augment(int_id now, std::vector<char> & visited, std::list<int_id> & path);
};

#endif /* GRAPH_H */
