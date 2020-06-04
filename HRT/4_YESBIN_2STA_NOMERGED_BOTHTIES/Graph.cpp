#include <algorithm>
#include <iostream>
#include "Graph.h"


const size_t expected_size = 1 << 16;

Graph::Graph(int cap_original) : _cap_original(cap_original), _cap_total(0),
  _left_total(0), max_flow(0), _exists(2), _adjacents(expected_size),
  _cap_remaining(expected_size) {
#ifdef DEBUG_GRAPH
  std::cout << "New graph" << std::endl;
#endif /* DEBUG_GRAPH */
  this->_exists[0] = std::vector<char>(expected_size, (char)false);
  this->_exists[1] = std::vector<char>(expected_size, (char)false);
}

void Graph::addVertex(Vertex name, int capacity) {
  int index = size();
  this->_names[name] = index;
  this->_indices[index] = name;
#ifdef DEBUG_GRAPH
  std::cout << "Add vertex " << name << " with cap " << capacity << std::endl;
#endif /* DEBUG_GRAPH */
  this->_adjacents[index] = std::vector<Edge>(expected_size);
  _cap_total += capacity;
  if (name.first == 1) {
    // position
    this->_cap_remaining[index] = capacity;
  } else {
    // candidate
    // Note that this is just a marker, it's not actually tracking the capacity
    // remaining for candidates
    this->_cap_remaining[index] = -1;
  }
  this->_exists[name.first][name.second] = (char)true;
}

Vertex Graph::name(int vert_index) {
  return this->_indices[vert_index];
}

bool Graph::containsVertex(Vertex name) const {
  return (bool)this->_exists[name.first][name.second];
}

/**
 * Adds an edge to the graph. Note that this edge must always be added in the
 * form (right, left) for things to work.
 */
void Graph::addEdge(Vertex v1, Vertex v2) {
  int i1 = this->_names[v1];
  int i2 = this->_names[v2];
  Edge e1, e2;
  e1.id = i1;
  e1.cap = 1;
  e1.flow = 0;
  e1.reverse = false;
  e2.id = i2;
  e2.cap = 1;
  e2.flow = 0;
  e2.reverse = true;
  _adjacents[i1][i2] = e2;
  _adjacents[i2][i1] = e1;
}

int Graph::size() const {
  return _names.size();
}

int Graph::maxFlow() const {
  return max_flow;
}

bool Graph::can_preprocess() {
  return _cap_original + cap_left() <= _cap_total - max_flow;
}

int Graph::cap_left() const {
  return _left_total;
}

int Graph::cap_right() const {
  return _cap_total - _left_total;
}

int Graph::cap_total() const {
  return _cap_total;
}

/**
 * Augment the flow.
 */
bool Graph::augment(Vertex source) {
#ifdef DEBUG_GRAPH
  std::cout << "Augmenting" << std::endl;
  this->printGraph();
#endif /* DEBUG_GRAPH */
  std::list<int_id> path;
  int source_name = this->_names[source];
  std::vector<char> visited(size(), false);
  visited[source_name] = (char)true;
  path.push_back(source_name);
  return internal_augment(source_name, visited, path);
}

#ifdef DEBUG_GRAPH
// TODO Rewrite using new data structures.
void Graph::printGraph() {
  for(auto & tuple : _adjacents) {
    int ind = tuple.first;
    std::cout << _indices[ind] << ":";
    for(auto & inner_tuple: tuple.second) {
      const Edge & adj = inner_tuple.second;
      std::cout << " " << _indices[adj.id] << "{" << adj.flow << "/" << adj.cap << "}";
    }
    std::cout << std::endl;
  }
}

#endif /* DEBUG_GRAPH */

/**
 * Continues an augmentation, on vertex now, which is on the right.
 */
bool Graph::internal_augment(int_id now, std::vector<char> & visited,
    std::list<int_id> & path) {
  //for(const auto & next_edge: _adjacents[now]) {
  for(size_t i = 0; i < _adjacents[now].size(); ++i) {
    const auto & next_edge = _adjacents[now][i];
    if ((bool)visited[next_edge.id]) {
      continue;
    }
    // Can't add any more flow to this edge.
    if (next_edge.flow >= next_edge.cap) {
      continue;
    }
    // If we're going "backwards" we need to use up (subtract) flow, so check we
    // have some. Note that since we are checking backwards, the flow will also
    // be negative
    if (next_edge.reverse) {
      if (next_edge.flow >= 0) {
        continue;
      }
    }
    if (_cap_remaining[next_edge.id] > 0) {
      // Found an augmenting flow. Modify flows used.
      path.push_back(next_edge.id);
      int start = path.front();
      path.pop_front();
      while(!path.empty()) {
        int next = path.front();
        path.pop_front();
        _adjacents[start][next].flow += 1;
        _adjacents[next][start].flow -= 1;
        start = next;
      }
      max_flow += 1;
      return true;
    }
    path.push_back(next_edge.id);
    visited[next_edge.id] = (char)true;
    if (internal_augment(next_edge.id, visited, path)) {
      return true;
    }
    path.pop_back();
  }
  return false;
}

